// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include <stdio.h>
#include "interrupts.h"
#include "timers.h"
#include "LEDarray.h"
#include "clock.h"
#include "LCD.h"
#include "ADC.h"
#include "calibrate.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  


void main(void) 
{
    // call all necessary init functions
    Interrupts_init();
    LCD_init();
    Timer0_init();
    LEDarray_init();
    LED_init();
    ADC_init();
    
    struct Clock datetime = {20, 0, 0, 27, 2, 2024, 2}; // datetime will passed to functions by pointers due to its size
    struct LED_rules_params led_p = {0, 0, 0, 0};
    struct calibrate_clock_params cal_p = {0, 0};
    unsigned int light_threshold = 50; // light threshold that must be passed to indicate dawn/dusk
    
    // arrays of average sunrise and sunset times by month (separated into hours [0-11] and min [12-23] to use lower memory variable types)
    // also passed with pointers due to size
    unsigned int sunrise[24] = {7, 7, 6, 6, 5, 4, 4, 5, 6, 7, 7, 7, 57, 13, 11, 1, 5, 40, 59, 45, 34, 23, 17, 57};
    unsigned int sunset[24] = {16, 17, 18, 19, 20, 21, 21, 20, 19, 18, 16, 15, 21, 16, 7, 59, 48, 21, 13, 24, 16, 8, 12, 53};

    char *weekdays[] = { // weekdays for LDR, first left blank to fix Monday issue
      "", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"  
    };
    
    while (1) {    
        unsigned int tmpval = ADC_getval(); // record current LDR output value   
        unsigned int event = LED_rules(light_threshold, tmpval, &datetime, sunrise, sunset, &led_p); // light up the LED array according to certain rules, and return event = 1 for sunrise, 2 for sunset, 0 otherwise
        calibrate_clock(event, &datetime, sunrise, sunset, &cal_p); // calibrate datetime depending on the event
        if (event == 1 || event == 2) { // flash RD7 LED to show a sunrise or sunset event
            LATDbits.LATD7 = 1;
            __delay_ms(200);
        }
        else {
            LATDbits.LATD7 = 0;
        }
        if (tmpval > light_threshold) { // light RH3 if LDR value is above the light threshold
            LATHbits.LATH3 = 1;
        }
        else {
            LATHbits.LATH3 = 0;
        }
        clock_update(&datetime); // update datetime
        
        // show the time, date and day of the week on the LCD screen
        char time[16];
        char date[16];
        LCD_setline(0);
        sprintf(time, "Time: %02u:%02u:%02u", datetime.hrs, datetime.mins, datetime.secs);
        LCD_sendstring(time);
        LCD_setline(1);
        sprintf(date, "%02u/%02u/%04u %s", datetime.days, datetime.mons, datetime.yrs, weekdays[datetime.dotw + 1]);
        LCD_sendstring(date);
        }
}





