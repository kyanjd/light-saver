#ifndef _LEDarray_H
#define _LEDarray_H

#include <xc.h>

#define _XTAL_FREQ 64000000

//function prototypes
void LED_init(void);
void LEDarray_init(void);
void LEDarray_disp_bin(unsigned int number);
void LEDarray_disp_dec(unsigned int number);
void LEDarray_disp_PPM(unsigned int cur_val, unsigned int max);
unsigned int button(unsigned int bcount);


#endif
