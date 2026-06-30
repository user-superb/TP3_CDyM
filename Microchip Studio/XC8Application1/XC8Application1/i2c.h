/*
 * i2c.h
 *
 * Created: 27/6/2026 03:32:37 p. m.
 *  Author: maide
 */ 


#ifndef I2C_H_
#define I2C_H_

#include <avr/interrupt.h>
#define F_CPU 16000000UL
#define SCL_CLOCK 100000L // Vel. del reloj de I2C
#define I2C_TIMEOUT_LIMIT 10000

extern volatile uint8_t i2c_error;

void I2C_Stop(void);
void I2C_Write(uint8_t data);
void I2C_Init(void);
void I2C_Start(void);
uint8_t I2C_Read_Nack(void);
uint8_t I2C_Read_Ack(void);

#endif /* I2C_H_ */