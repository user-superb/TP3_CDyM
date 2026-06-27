#include "monitor.h"
#include "uart.h"
#include "reloj.h"
#include "dht11.h" // Agregamos el sensor
#include <stdio.h>

volatile uint8_t periodo_telemetria = 2;
static uint8_t contador_segundos = 0;
static uint8_t ultimo_segundo_visto = 255;
static uint8_t contador_tramas_alerta = 0; // Para contar "cada dos tramas"

void monitor_init(void) {
}

void monitor_procesar_telemetria(void) {
	if (seg_dec != ultimo_segundo_visto) {
		ultimo_segundo_visto = seg_dec;
		contador_segundos++;

		if (contador_segundos >= periodo_telemetria) {
			contador_segundos = 0;

			uint8_t temp = 0, hum = 0;
			uint8_t dht_status = dht11_read_data(&temp, &hum);
			
			char buffer[100];
			uint8_t hay_alerta = 0;
			char mensaje_emergencia[100] = "";

			if (dht_status) {
				// Evaluamos si es de día (07:00 a 18:59)
				uint8_t es_dia = (horas_dec >= 7 && horas_dec < 19);
				
				// Límites según Tabla 1
				uint8_t t_min = es_dia ? 20 : 15;
				uint8_t t_max = es_dia ? 30 : 22;
				uint8_t h_min = es_dia ? 50 : 60;
				uint8_t h_max = es_dia ? 70 : 80;
				const char* periodo = es_dia ? "diurno" : "nocturno";

				// Verificamos si se sale de rango
				if (temp < t_min || temp > t_max) {
					hay_alerta = 1;
					sprintf(mensaje_emergencia, "[ALERTA] [%02u:%02u:%02u] Temperatura fuera de rango %s! Valor: %u C\r\n",
					horas_dec, min_dec, seg_dec, periodo, temp);
					} else if (hum < h_min || hum > h_max) {
					hay_alerta = 1;
					sprintf(mensaje_emergencia, "[ALERTA] [%02u:%02u:%02u] Humedad fuera de rango %s! Valor: %u %%\r\n",
					horas_dec, min_dec, seg_dec, periodo, hum);
				}

				// Armamos la cadena normal de telemetría
				sprintf(buffer, "[%02u:%02u:%02u] T: %u C H: %u %% | Estado: %s\r\n",
				horas_dec, min_dec, seg_dec, temp, hum, hay_alerta ? "ALERTA" : "NORMAL");

				} else {
				// Decisión de diseño: ¿Qué pasa si el DHT11 no está?
				sprintf(buffer, "[%02u:%02u:%02u] T: -- C H: -- %% | Estado: ERROR DHT11\r\n",
				horas_dec, min_dec, seg_dec);
			}

			// Transmitimos la trama base
			uart_send_string(buffer);

			// Regla de emergencia: Enviar el string detallado cada dos tramas
			if (hay_alerta) {
				contador_tramas_alerta++;
				if (contador_tramas_alerta >= 2) {
					uart_send_string(mensaje_emergencia);
					contador_tramas_alerta = 0; // Reseteamos el contador
				}
				} else {
				contador_tramas_alerta = 0; // Si vuelve a la normalidad, limpiamos el contador
			}
		}
	}
}