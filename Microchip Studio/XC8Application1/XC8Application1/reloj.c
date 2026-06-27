/*
 * reloj.c
 *
 * Created: 27/6/2026 02:44:53 p. m.
 *  Author: maide
 */ 

#include "reloj.h"
#include "timer1.h"
#include "i2c.h"

volatile uint8_t seg_dec	= 0;
volatile uint8_t min_dec	= 0;
volatile uint8_t horas_dec	= 0;

void init_reloj(void) {
	I2C_Init();
	Timer1_Init();
}

void reloj_foreground(void) {
	static uint8_t	seg_raw = 0,
					min_raw	= 0,
					horas_raw = 0;
					
	if (flag_1seg == 1) {
		flag_1seg = 0;
		
		// Se apunta al registro 0x00 (Segundos)
		I2C_Start();
		I2C_Write(0xD0);
		I2C_Write(0x00);
		
		// Se cambia al modo lectura (Repeated Start)
		I2C_Start();
		I2C_Write(0xD1);
		
		seg_raw = I2C_Read_Ack();  // Lee 0x00
		min_raw  = I2C_Read_Ack();  // Lee 0x01
		horas_raw    = I2C_Read_Nack(); // Lee 0x02
		
		I2C_Stop();
		
		// CONVERSIÓN DE BCD A DECIMAL
		// Se tiene que los números estarán guardados de tal forma que los 4 bits altos son las decenas y los 4 bits bajos son las unidades.
		seg_dec = ((seg_raw >> 4) & 0x0F) * 10 + (seg_raw & 0x0F);
		min_dec = ((min_raw >> 4) & 0x0F) * 10  + (min_raw & 0x0F);
		
		// En el registro de horas, los bits 4 y 5 son las decenas. (El bit 6 es el formato 12/24h)
		horas_dec = ((horas_raw >> 4) & 0x03) * 10 + (horas_raw & 0x0F);
	}
}