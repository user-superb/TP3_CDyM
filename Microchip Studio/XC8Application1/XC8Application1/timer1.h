/*
 * timer1.h
 *
 * Created: 27/6/2026 03:26:47 p. m.
 *  Author: maide
 */ 


#ifndef TIMER1_H_
#define TIMER1_H_

#include <avr/io.h>
#include <avr/interrupt.h>

extern volatile uint8_t flag_1seg;
extern volatile uint8_t flag_1seg_monitor;

void timer1_init(void);

#endif /* TIMER1_H_ */