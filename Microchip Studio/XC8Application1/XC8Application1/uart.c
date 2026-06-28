#include "uart.h"
#include <avr/interrupt.h>

// --- Variables globales para Recepción (RX) ---
volatile char rx_buffer[RX_BUFFER_SIZE];
volatile uint8_t rx_ready = 0;
static volatile uint8_t rx_index = 0;

// --- Variables globales para Transmisión (TX) ---
static volatile char tx_buffer[TX_BUFFER_SIZE];
static volatile uint8_t tx_head = 0;
static volatile uint8_t tx_tail = 0;

void uart_init(void) {
    // Configura BAUDRATE = 9600bps @ 16MHz
    UBRR0L = 103; 
    
    // Activa Tx, Rx y la interrupción de recepción (RXCIE0).
    // Nota: La interrupción UDRIE0 (TX) se activa SOLO cuando hay datos para enviar.
    UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
    
    // Configura formato 8N1
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_send_string(const char *str) {
    uint8_t i = 0;
    
    // 1. Cargar toda la cadena en el buffer circular de transmisión
    while (str[i] != '\0') {
        uint8_t next_head = (tx_head + 1) % TX_BUFFER_SIZE;
        
        // Espera de seguridad por si el buffer se llena (evita sobreescritura)
        while (next_head == tx_tail); 
        
        tx_buffer[tx_head] = str[i];
        tx_head = next_head;
        i++;
    }
    
    // 2. Activar la interrupción de "Registro de Datos Vacío" (UDRE)
    // Esto dispara la interrupción inmediatamente para empezar a vaciar el buffer
    UCSR0B |= (1 << UDRIE0);
}

// --- Rutina de Servicio de Interrupción: Byte Recibido (RX) ---
ISR(USART_RX_vect) {
	char data = UDR0;

	if (data == '\b' || data == 127) {
		if (rx_index > 0) {
			rx_index--;
		}
		} else if (data != '\r' && data != '\n') {
		if (rx_index < (RX_BUFFER_SIZE - 1)) {
			rx_buffer[rx_index++] = data;
		}
		} else if ((data == '\r' || data == '\n') && rx_index > 0) {
		rx_buffer[rx_index] = '\0';
		rx_index = 0;
		rx_ready = 1;
	}
}

// --- Rutina de Servicio de Interrupción: Registro de Datos Vacío (TX) ---
ISR(USART_UDRE_vect) {
    // ¿Hay datos pendientes en el buffer circular?
    if (tx_head != tx_tail) {
        UDR0 = tx_buffer[tx_tail]; // Transmite el byte
        tx_tail = (tx_tail + 1) % TX_BUFFER_SIZE; // Avanza la cola
    } else {
        // Buffer vacío: Apagamos la interrupción para que no se dispare infinitamente
        UCSR0B &= ~(1 << UDRIE0);
    }
}