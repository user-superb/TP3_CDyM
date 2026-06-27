#include "comandos.h"
#include "uart.h"
#include "monitor.h"
#include "i2c.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// Función auxiliar para convertir de Decimal a BCD
static uint8_t dec_to_bcd(uint8_t val) {
    return ((val / 10) << 4) | (val % 10);
}

void procesar_comando(char *cmd) {
    // Comando 1: SET TM=SS (Actualiza tasa de muestreo)[cite: 11]
    if (strncmp(cmd, "SET_TM=", 7) == 0) {
        
        // Convertimos a entero lo que sigue después de "SET TM="
        int nuevo_t = atoi(cmd + 7);
        
        // Verificamos que esté en el rango permitido (2 y 60)[cite: 11]
        if (nuevo_t >= 2 && nuevo_t <= 60) {
            periodo_telemetria = (uint8_t)nuevo_t;
            uart_send_string(">> OK: Tasa de muestreo actualizada.\r\n");
        } else {
            uart_send_string(">> ERROR: La tasa debe estar entre 2 y 60 segundos.\r\n");
        }
    }
    // Comando 2: SET TIME=HH:MM:SS (Actualiza el RTC)[cite: 11]
    else if (strncmp(cmd, "SET_TIME=", 9) == 0) {
        
        // Parseamos los caracteres ASCII asumiendo formato estricto HH:MM:SS
        // Restamos '0' (48 en ASCII) para obtener el valor numérico
        uint8_t h = (cmd[9] - '0') * 10 + (cmd[10] - '0');
        uint8_t m = (cmd[12] - '0') * 10 + (cmd[13] - '0');
        uint8_t s = (cmd[15] - '0') * 10 + (cmd[16] - '0');

        // Validamos que la hora tenga sentido lógico
        if (h < 24 && m < 60 && s < 60) {
            
            // Convertimos a BCD
            uint8_t h_bcd = dec_to_bcd(h);
            uint8_t m_bcd = dec_to_bcd(m);
            uint8_t s_bcd = dec_to_bcd(s);

            // Secuencia de escritura I2C para el DS3231/DS3232
            I2C_Start();
            I2C_Write(0xD0);     // Dirección I2C del RTC en modo escritura
            I2C_Write(0x00);     // Apuntamos al registro 0x00 (Segundos)
            I2C_Write(s_bcd);    // Escribimos Segundos (reg 0x00) e incrementa auto
            I2C_Write(m_bcd);    // Escribimos Minutos (reg 0x01)
            I2C_Write(h_bcd);    // Escribimos Horas (reg 0x02)
            I2C_Stop();
            
            uart_send_string(">> OK: Reloj actualizado.\r\n");
        } else {
            uart_send_string(">> ERROR: Formato de hora invalido.\r\n");
        }
    }
    // Comando Desconocido
    else {
        uart_send_string(">> ERROR: Comando no reconocido.\r\n");
    }
}