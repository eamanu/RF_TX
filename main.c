/*
 * main.c
 *
 *  Created on: Jul 12, 2016
 *      Author: root
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//USART communication
#include "uart.h"
//nRF24L01 lib
#include "nRF24L01.h"

int main(){
	//inizialize usart
	usart_init();
	//inizialize rf tx
	rf_init();
	rf_config();

	//uint8_t reg = 0x07;
	uint8_t reg = CONFIG_NRF24L01;

	uint8_t data;

	while(1){
		rf_read_register(STATUS, &data,1);
		put_int(data);
		for(uint8_t i = 0; i<10; i++){
			TX_MODE(&i, 2);

			//put_int(i);
		}
		put_int(data);
//		rf_read_register(reg, &data, 1);
//		put_int(data);

		_delay_ms(1000);
	}


}
