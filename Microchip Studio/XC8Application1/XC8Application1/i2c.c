/*
 * CFile1.c
 *
 * Created: 27/6/2026 03:33:38 p. m.
 *  Author: maide
 */ 

#include "i2c.h"

volatile uint8_t i2c_error = 0;

void I2C_Stop(void) {
	// Transmitir condición de STOP
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

void I2C_Write(uint8_t data) {
	uint16_t timeout_counter = I2C_TIMEOUT_LIMIT;
	
	// Cargar el dato en el registro de datos
	TWDR = data;
	// Iniciar transmisión
	TWCR = (1 << TWINT) | (1 << TWEN);
	// Esperar a que termine de transmitir
	while ((TWCR & (1 << TWINT)) == 0) {
		timeout_counter--;
		if (timeout_counter == 0) {
			i2c_error = 1; 
			return;
		}	
	};
	
	// Chequeo del ACK
	// 0x18 = Dirección de escritura enviada, ACK recibido
	// 0x28 = Dato enviado, ACK recibido
	// 0x40 = Dirección de lectura enviada, ACK recibido
	uint8_t status = TWSR & 0xF8;
	if (status != 0x18 && status != 0x28 && status != 0x40) {
		i2c_error = 1;
	}
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
	uint16_t timeout_counter = I2C_TIMEOUT_LIMIT;
	
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0) {
		timeout_counter--;
		if (timeout_counter == 0) {
			i2c_error = 1;
			return;
		}	
	};
	
	// ¿Fue un START (0x08) o Repeated START (0x10) exitoso?
	uint8_t status = TWSR & 0xF8;
	if (status != 0x08 && status != 0x10) {
		i2c_error = 1;
	}
}

uint8_t I2C_Read_Nack(void) {
	uint16_t timeout_counter = I2C_TIMEOUT_LIMIT;
	
	// Iniciar recepción sin devolver ACK
	TWCR = (1 << TWINT) | (1 << TWEN);
	// Esperar a recibir el dato
	while ((TWCR & (1 << TWINT)) == 0) {
		timeout_counter--;
		if (timeout_counter == 0) {
			i2c_error = 1; 
			return 0;        
		}	
	};
	
	// 0x58 = Dato recibido, nosotros devolvimos un NACK
	if ((TWSR & 0xF8) != 0x58) {
		i2c_error = 1;
		return 0xFF;
	}
	
	return TWDR;
}

uint8_t I2C_Read_Ack(void) {
	uint16_t timeout_counter = I2C_TIMEOUT_LIMIT;
	
	// Iniciar recepción y devolver ACK seteando el bit TWEA
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	// Esperar a recibir el dato
	while ((TWCR & (1 << TWINT)) == 0) {
		timeout_counter--;
		if (timeout_counter == 0) {
			i2c_error = 1;
			return 0;        
		}	
	};
	
	// 0x50 = Dato recibido, nosotros devolvimos un ACK
	if ((TWSR & 0xF8) != 0x50) {
		i2c_error = 1;
		return 0;
	}
	
	return TWDR;
}