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


// Vel. del relojo de I2C
#define SCL_CLOCK 100000L

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


int main(void)
{
	I2C_Init();
	Timer1_Init();
	sei();
	DDRD |= (1 << PORTD6);
	DDRD |= (1 << PORTD7);//estos dos pines los inicializo para debug.
	uint8_t segundos_raw;
	// Variables para almacenar la lectura del DHT11
	uint8_t hum_int = 0, hum_dec = 0, temp_int = 0, temp_dec = 0,datos_validos = 0, checksum = 0;
    
	while(1)
    {
		_delay_ms(1000);
		request();
		if (check_response() == 1)
		{
			hum_int  = read_byte();
			hum_dec  = read_byte();
			temp_int = read_byte();
			temp_dec = read_byte();
			checksum = read_byte();
			
			// Validar si la transmisión fue correcta
			if ((hum_int + hum_dec + temp_int + temp_dec) == checksum) {
				datos_validos = 1; // El dato de temperatura es real y confiable
				PORTD |= (1 << PORTD6); //mando el 1
				} else {
				
				datos_validos = 0; // Error de Checksum: hubo ruido o desfase temporal
			}
		}
		else
		{
			PORTD |= (1 << PORTD7); //No se respondió.
		}
		
        // Tarea de Foreground
	/*
        if (flag_1seg == 1) {
	        flag_1seg = 0;
			
	        I2C_Start();
	        I2C_Write(hum_int); 
	        I2C_Write(temp_int); 
	        
	        I2C_Start();    
	        I2C_Write(0xD1); 
	        
	        segundos_raw = I2C_Read(1); // Leer el dato
	        I2C_Stop();
			
			// segundos_raw está re pijeado pero creo que no lo vamos a necesitar
		
		}
		*/
	
    }
}