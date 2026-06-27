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
	
	DDRD |= (1 << PORTD6);
	DDRD |= (1 << PORTD7);//estos dos pines los inicializo para debug.
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
		
		tarea_foreground();
		
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