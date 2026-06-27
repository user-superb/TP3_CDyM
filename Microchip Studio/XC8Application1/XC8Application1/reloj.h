/*
 * reloj.h
 *
 * Created: 27/6/2026 02:42:07 p. m.
 *  Author: maide
 */ 


#ifndef RELOJ_H_
#define RELOJ_H_

#include <avr/interrupt.h>

// Vel. del relojo de I2C
#define SCL_CLOCK 100000L
#define F_CPU 16000000UL

void I2C_Stop(void);
void I2C_Write(uint8_t data);
void I2C_Init(void);
void I2C_Start(void);
uint8_t I2C_Read();
void Timer1_Init(void);

void init_reloj(void);
char tarea_foreground(void);



#endif /* RELOJ_H_ */