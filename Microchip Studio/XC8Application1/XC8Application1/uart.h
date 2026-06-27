#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stdint.h>

// Tamaños de los buffers (ajustables según la memoria y necesidad)
#define RX_BUFFER_SIZE 32
#define TX_BUFFER_SIZE 128

// Variables expuestas para que el main (foreground) pueda procesar los comandos recibidos
extern volatile char rx_buffer[RX_BUFFER_SIZE];
extern volatile uint8_t rx_ready;

// Prototipos
void uart_init(void);
void uart_send_string(const char *str);

#endif /* UART_H_ */