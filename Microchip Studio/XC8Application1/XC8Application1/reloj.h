/*
 * reloj.h
 *
 * Created: 27/6/2026 02:42:07 p. m.
 *  Author: maide
 */ 


#ifndef RELOJ_H_
#define RELOJ_H_

#include <avr/io.h>

// Las hice variables globales cosa de poder verlas en el debugger del proteus
extern volatile uint8_t	seg_dec;
extern volatile uint8_t	min_dec;
extern volatile uint8_t	horas_dec;

void Timer1_Init(void);
void init_reloj(void);
void reloj_foreground(void);

#endif /* RELOJ_H_ */