#include <xc.h>
#include "interrupts.h"

volatile unsigned int count = 0;
/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/
void Interrupts_init(void)
{
    INTCONbits.PEIE = 1; // turn on peripheral interrupts
    PIE0bits.TMR0IE = 1; //turn on timer0 interrupts
    PIR0bits.TMR0IF = 0; // overflow interrupt off by default    
    INTCONbits.GIE = 1; // turn on global interrupts
}

/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/
void __interrupt(high_priority) HighISR()
{
	//add your ISR code here i.e. check the flag, do something (i.e. toggle an LED), clear the flag...
    if (PIR0bits.TMR0IF == 1) { // if overflow interrupt is detected
        count ++;
        TMR0H=11;            // change initial value to account for slightly off prescaler division (256-244) * 2^8
        TMR0L=219;
        PIR0bits.TMR0IF = 0; // reset overflow interrupt (always exit interrupt as final step)
        
    }
}

