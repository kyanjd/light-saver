#ifndef _calibrate_H
#define	_calibrate_H

#include <xc.h>
#include "clock.h"

#define _XTAL_FREQ 64000000

struct calibrate_clock_params { // defining the Clock struct to store date/time information
    unsigned int sunrise_index;
    unsigned int sunset_index;
};

void calibrate_clock(unsigned int event, struct Clock *c, unsigned int *sunrise, unsigned int *sunset, struct calibrate_clock_params *p);


#endif
