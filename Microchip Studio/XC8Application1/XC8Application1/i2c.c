/*
 * CFile1.c
 *
 * Created: 27/6/2026 03:33:38 p. m.
 *  Author: maide
 */ 

#include "i2c.h"

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

uint8_t I2C_Read_Nack(void) {
	// Iniciar recepción sin devolver ACK
	TWCR = (1 << TWINT) | (1 << TWEN);
	// Esperar a recibir el dato
	while ((TWCR & (1 << TWINT)) == 0);
	return TWDR;
}

uint8_t I2C_Read_Ack(void) {
	// Iniciar recepción y devolver ACK seteando el bit TWEA
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	// Esperar a recibir el dato
	while ((TWCR & (1 << TWINT)) == 0);
	return TWDR;
}