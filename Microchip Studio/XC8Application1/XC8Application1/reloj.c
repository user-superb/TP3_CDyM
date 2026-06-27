/*
 * reloj.c
 *
 * Created: 27/6/2026 02:44:53 p. m.
 *  Author: maide
 */ 

#include "reloj.h"

void I2C_Stop(void) {
	// Transmitir condición de STOP
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

void I2C_Write(uint8_t data) {
	// Cargar el dato en el registro de datos
	TWDR = data;
	// Iniciar transmisión
	TWCR = (1 << TWINT) | (1 << TWEN);
	// Esperar a que termine de transmitir
	while ((TWCR & (1 << TWINT)) == 0);
}

void I2C_Init(void) {
	// Se ponen los bits TWPS0 y TWPS1 en 0 dentro del registro TWSR.
	TWSR &= ~((1 << TWPS1) | (1 << TWPS0));

	// Configurar el Bit Rate Register (TWBR).
	TWBR = (uint8_t)(((F_CPU / SCL_CLOCK) - 16) / 2);

	// Habilitar el módulo TWI.
	TWCR = (1 << TWEN);
}

void I2C_Start(void) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
}

uint8_t I2C_Read() {
	// Iniciar recepción sin devolver ACK
	TWCR = (1 << TWINT) | (1 << TWEN);
	// Esperar a recibir el dato
	while ((TWCR & (1 << TWINT)) == 0);
	return TWDR;
}

// Variable global para comunicar el Background con el Foreground
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

void init_reloj(void) {
	I2C_Init();
	Timer1_Init();
}

char tarea_foreground(void) {
	static uint8_t segundos_raw;
	if (flag_1seg == 1) {
		flag_1seg = 0;
		
		I2C_Start();
		I2C_Write(0xD0);
		I2C_Write(0x00);
		
		I2C_Start();
		I2C_Write(0xD1);
		
		segundos_raw = I2C_Read(1); // Leer el dato
		I2C_Stop();
	}
	
	return flag_1seg;
}