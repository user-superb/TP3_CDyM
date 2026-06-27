#include <avr/io.h>

#ifndef dht11_H_
#define dht11_H_





#endif /* dht11_H_ */

void request(void);
uint8_t check_response(void);
uint8_t read_byte(void);
uint8_t dht11_read_data(uint8_t *temp, uint8_t *hum);
