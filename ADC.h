#ifndef _ADC_H
#define _ADC_H

#include <xc.h>
#include "clock.h"

#define _XTAL_FREQ 64000000

struct LED_rules_params { // defining the Clock struct to store date/time information
    unsigned int sunrise_check;
    unsigned int sunset_check;
    unsigned int last_tmpval;
    unsigned int diff;
};

void ADC_init(void);
unsigned int ADC_getval(void);
unsigned int LED_rules(unsigned int threshold, unsigned int tmpval, const struct Clock *c, unsigned int *sunrise, unsigned int *sunset, struct LED_rules_params *p );

#endif
