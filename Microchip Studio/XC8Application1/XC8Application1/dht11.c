#define F_CPU 16000000UL
#include "dht11.h"
#include <avr/io.h>
#include <util/delay.h>

#define T_RL 18 //Tiempo low para hacer el request (Time request low) en ms.
#define T_RH 30 //Tiempo en high para hacer el request (Time request high) en us.

uint8_t dht11_read_data(uint8_t *temp, uint8_t *hum) {
	uint8_t h_int, h_dec, t_int, t_dec, chk;
	
	request(); // Enviamos el pulso de inicio
	
	if (check_response()) { 
		h_int = read_byte();
		h_dec = read_byte();
		t_int = read_byte();
		t_dec = read_byte();
		chk = read_byte();
		
		
		if (chk == (uint8_t)(h_int + h_dec + t_int + t_dec)) {
			*hum = h_int;
			*temp = t_int;
			return 1; // Lectura exitosa
		}
	}
	return 0; // sensor desconectado
}
void request(void)
{
	DDRC |= (1 << PORTC0); //inicializar como salida.
	PORTC &= ~(1 << PORTC0); //mando el 0
	_delay_ms(T_RL);
	PORTC |= (1 << PORTC0); //mando el 1
	_delay_us(T_RH);
	DDRC &= ~(1 << PORTC0); //configuro el puerto como entrada para esperar la recepción.
}

uint8_t check_response(void)
{
	uint8_t response = 0;
	_delay_us(40);
	
	if (!(PINC & (1 << PINC0)))
	{
		_delay_us(80);
		if (PINC & (1 << PINC0))
		{
			response = 1;
		}
		while (PINC & (1 << PINC0));
	}
	return response;
}

uint8_t read_byte(void)
{
	uint8_t data = 0;
	
	for (int i = 0; i < 8; i++)
	{
		while (!(PINC & (1 << PINC0)));
		
		_delay_us(30);
		
		if (PINC & (1 << PINC0))
		{
			data = (data << 1) | 1;
			while (PINC & (1 << PINC0));
		}
		else
		{
			data = (data << 1);
		}
	}
	return data;
}