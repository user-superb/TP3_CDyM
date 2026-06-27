#ifndef MONITOR_H_
#define MONITOR_H_

#include <stdint.h>

// Tasa de muestreo (T) en segundos. El TP pide que esté entre 2 y 60.
// La inicializamos en 2 por defecto.
extern volatile uint8_t periodo_telemetria; 

void monitor_init(void);
void monitor_procesar_telemetria(void);

#endif /* MONITOR_H_ */