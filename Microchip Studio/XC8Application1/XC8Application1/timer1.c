/*
 * timer1.c
 *
 * Created: 27/6/2026 03:28:59 p. m.
 *  Author: maide
 */

#include "timer1.h"

volatile uint8_t flag_1seg = 0;

void Timer1_Init(void) {
	TCCR1B |= (1 << WGM12) | (1 << CS12);
	OCR1A = 62499;
	TIMSK1 |= (1 << OCIE1A);
}

// Rutina de Servicio de Interrupción
ISR(TIMER1_COMPA_vect) {
	flag_1seg = 1; // Se levanta la bandera
}
