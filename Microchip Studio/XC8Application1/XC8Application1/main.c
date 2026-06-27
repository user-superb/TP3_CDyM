/*
 * main.c
 *
 * Created: 6/16/2026 1:31:51 PM
 *  Author: maide
 */ 
#define F_CPU 16000000UL
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>
#include "dht11.h"
#include "reloj.h"


// Vel. del relojo de I2C
#define SCL_CLOCK 100000L

int main(void)
{
	init_reloj(); // Inicializa el reloj
	sei();
	
	while(1)
    {		
		reloj_foreground();
    }
}