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
#include "uart.h"
#include "monitor.h"
#include "comandos.h"


// Vel. del relojo de I2C
#define SCL_CLOCK 100000L

int main(void)
{
	init_reloj(); // Inicializa el reloj
	uart_init();
	sei();
	uart_send_string("Sistema Iniciado. Esperando comandos...\r\n");
	while(1)
    {		
		reloj_foreground();
		monitor_procesar_telemetria();
		if (rx_ready == 1) {
			procesar_comando((char*)rx_buffer);
			rx_ready = 0; // Bajamos la bandera para esperar el próximo comando
		}
    }
}