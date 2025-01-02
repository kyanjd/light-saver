#ifndef _clock_H
#define	_clock_H

#include <xc.h>

#define _XTAL_FREQ 64000000

struct Clock { // defining the Clock struct to store date/time information
    unsigned int hrs;
    unsigned int mins;
    unsigned int secs;
    unsigned int days;
    unsigned int mons;
    unsigned int yrs;
    unsigned int dotw; // day of the week (0 = Monday, 6 = Sunday)
};

unsigned int list_check(unsigned int list[], unsigned int len, unsigned int item);
void clock_update(struct Clock *c);

#endif	

