#ifndef _interrupts_H
#define _interrupts_H

#include <xc.h>

#define _XTAL_FREQ 64000000

extern volatile unsigned int count;
void Interrupts_init(void);
void __interrupt(high_priority) HighISR();

#endif
