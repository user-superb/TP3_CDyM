#include "comandos.h"
#include "uart.h"
#include "monitor.h"
#include "i2c.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


static uint8_t dec_to_bcd(uint8_t val) {
    return ((val / 10) << 4) | (val % 10);
}

void procesar_comando(char *cmd) {
    
    if (strncmp(cmd, "SET_TM=", 7) == 0) {
        
        
        int nuevo_t = atoi(cmd + 7);
        
        
        if (nuevo_t >= 2 && nuevo_t <= 60) {
            periodo_telemetria = (uint8_t)nuevo_t;
            uart_send_string(">> OK: Tasa de muestreo actualizada.\r\n");
        } else {
            uart_send_string(">> ERROR: La tasa debe estar entre 2 y 60 segundos.\r\n");
        }
    }
    
    else if (strncmp(cmd, "SET_TIME=", 9) == 0) {
        
        
        uint8_t h = (cmd[9] - '0') * 10 + (cmd[10] - '0');
        uint8_t m = (cmd[12] - '0') * 10 + (cmd[13] - '0');
        uint8_t s = (cmd[15] - '0') * 10 + (cmd[16] - '0');

        
        if (h < 24 && m < 60 && s < 60) {
            
            
            uint8_t h_bcd = dec_to_bcd(h);
            uint8_t m_bcd = dec_to_bcd(m);
            uint8_t s_bcd = dec_to_bcd(s);

            
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