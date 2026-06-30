#include "monitor.h"
#include "uart.h"
#include "reloj.h"
#include "dht11.h" // Agregamos el sensor
#include "timer1.h"
#include <stdio.h>

volatile uint8_t periodo_telemetria = 2;
static uint8_t contador_segundos = 0;
static uint8_t contador_tramas_alerta = 0; // Para contar "cada dos tramas"

void monitor_init(void) {
}

void monitor_procesar_telemetria(void) {
	if (flag_1seg_monitor == 1) {
		flag_1seg_monitor = 0;
		contador_segundos++;

		if (contador_segundos >= periodo_telemetria) {
			contador_segundos = 0;

			uint8_t temp = 0, hum = 0;
			uint8_t dht_status = dht11_read_data(&temp, &hum);
			
			char buffer[100];
			uint8_t hay_alerta = 0;
			char mensaje_emergencia[100] = "";

			if (dht_status) {
				if (error_reloj == 1) {
					// Falla el reloj, pero el DHT11 funciona.
					sprintf(buffer, "[--:--:--] T: %u C H: %u %% | Estado: ERROR RTC\r\n", temp, hum);
					
					} else {
					// Todo funciona normal.
					uint8_t es_dia = (horas_dec >= 7 && horas_dec < 19);
					
					uint8_t t_min = es_dia ? 20 : 15;
					uint8_t t_max = es_dia ? 30 : 22;
					uint8_t h_min = es_dia ? 50 : 60;
					uint8_t h_max = es_dia ? 70 : 80;
					const char* periodo = es_dia ? "diurno" : "nocturno";

					if (temp < t_min || temp > t_max) {
						hay_alerta = 1;
						sprintf(mensaje_emergencia, "[ALERTA] [%02u:%02u:%02u] Temperatura fuera de rango %s! Valor: %u C\r\n",
						horas_dec, min_dec, seg_dec, periodo, temp);
						} else if (hum < h_min || hum > h_max) {
						hay_alerta = 1;
						sprintf(mensaje_emergencia, "[ALERTA] [%02u:%02u:%02u] Humedad fuera de rango %s! Valor: %u %%\r\n",
						horas_dec, min_dec, seg_dec, periodo, hum);
					}

					sprintf(buffer, "[%02u:%02u:%02u] T: %u C H: %u %% | Estado: %s\r\n",
					horas_dec, min_dec, seg_dec, temp, hum, hay_alerta ? "ALERTA" : "NORMAL");
				}
				} else {
				if (error_reloj == 1) {
					sprintf(buffer, "[--:--:--] T: -- C H: -- %% | Estado: FALLA GENERAL\r\n");
					} else {
					sprintf(buffer, "[%02u:%02u:%02u] T: -- C H: -- %% | Estado: ERROR DHT11\r\n",
					horas_dec, min_dec, seg_dec);
				}
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