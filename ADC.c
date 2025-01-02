#include <xc.h>
#include "ADC.h"
#include "LEDarray.h"
#include "clock.h"

/************************************
/ ADC_init_init
/ Function used to initialise ADC module and set it up
/ to sample on pin RA3
************************************/
void ADC_init(void)
{
    TRISAbits.TRISA3=1; // Select pin RA3 as input
    ANSELAbits.ANSELA3=1; //Ensure analogue circuitry is active (it is by default - watch out for this later in the course!)

    // Set up the ADC module - check section 32 of the datasheet for more details
    ADREFbits.ADNREF = 0; // Use Vss (0V) as negative reference
    ADREFbits.ADPREF = 0b00; // Use Vdd (3.3V) as positive reference
    ADPCH=0b11; // Select channel RA3/ANA3 for ADC
    ADCON0bits.ADFM = 0; // Left-justified result (i.e. no leading 0s)
    ADCON0bits.ADCS = 1; // Use internal Fast RC (FRC) oscillator as clock source for conversion
    ADCON0bits.ADON = 1; // Enable ADC
}

unsigned int ADC_getval(void)
{
    unsigned int tmpval;
       
    ADCON0bits.GO = 1; // Start ADC conversion

    while (ADCON0bits.GO); // Wait until conversion done (bit is cleared automatically when done)
        
    tmpval = ADRESH; // Get 8 most significant bits of the ADC result - if we wanted the 
	// full 10bit result we would also need to read the 2 LSBs from ADRESL and combine the result.
	// An 8bit result is sufficient for our needs here

    tmpval = 255 - tmpval; //invert readings for LDR so brighter is a bigger number

    return tmpval; //return this value when the function is called
}

unsigned int LED_rules(unsigned int threshold, unsigned int tmpval, const struct Clock *c, unsigned int *sunrise, unsigned int *sunset, struct LED_rules_params *p)
{    
    // sunrise, sunset and current time in minutes for comparison
    unsigned int sunrise_sum = sunrise[c->mons - 1] * 60 + sunrise[c->mons + 11];
    unsigned int sunset_sum = sunset[c->mons - 1] * 60 + sunset[c->mons + 11];
    unsigned int time_sum = c->hrs * 60 + c->mins;
    
    LEDarray_disp_bin(0b00000000); // start with lights off
    unsigned int event = 0; // 0 indicates no event
    // p->sunrise_index = 1 if a sunrise event has occurred, retains value across function calls
    // p->sunset_index = 1 if a sunset event has occurred, retains value across function calls
    // p->last_tmpval = last LDR value
    // p->diff = abs of difference between current and last LDR value
     
    // check to see how drastic the light change is (should be smooth for real sunrise/sunset)
    if (p->last_tmpval > tmpval) {
        p->diff = p->last_tmpval - tmpval;
    }
    else if (p->last_tmpval <= tmpval) {
        p->diff = tmpval - p->last_tmpval;
    }
    
    if (p->diff < 5) { // ensures light change is gradual
        if ((c->hrs >= 1 && c->hrs < 5) || (c->hrs >=10 && c->hrs < 15)) { // always keep lights off between 1am-5am, and between 10am-3pm
            LEDarray_disp_bin(0b00000000);   
            }
        else if (tmpval <= threshold && time_sum <= sunrise_sum + 30 && c->hrs < 12) { // turn lights on during darkness between midnight and sunrise with a 30min buffer
            LEDarray_disp_bin(0b11111111); 
        }
        else if (tmpval <= threshold && time_sum >= sunset_sum - 30 && c->hrs >= 12) { // turn lights on during darkness between sunset with a 30 min buffer and midnight
            LEDarray_disp_bin(0b11111111);
        }

        if (tmpval > threshold && p->sunrise_check == 0 && time_sum < sunrise_sum + 30) { // return 1 for sunrise (first instance of LDR going above threshold after estimated sunrise time with buffer)
            event = 1;
            p->sunrise_check = 1;
            p->sunset_check = 0;
        }

        if (tmpval < threshold && p->sunset_check == 0 && time_sum >= sunset_sum - 30) { // return 2 for sunset (first instance of LDR going below threshold after estimated sunset time with buffer)
            event = 2;
            p->sunset_check = 1;
            p->sunrise_check = 0;
        }
    }
    p->last_tmpval = tmpval;
    __delay_ms(100);
    return event;
}


