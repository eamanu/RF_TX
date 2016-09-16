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


/*define addr*/
uint8_t tx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
uint8_t rx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};

int main(){
	//inizialize usart
	usart_init();

	//inizialize rf tx
	rf_init();

	rf_config(2,1);

	//set the device addr
	set_rx_address(rx_address);
	set_tx_address(tx_address);

	//uint8_t reg = 0x07;
	uint8_t reg = CONFIG_NRF24L01;
	uint8_t data;
	uint8_t temp;
	uint8_t sending = 1;
	while(1){

		//for(uint8_t i = 0; i<10; i++){
			/*Go to TX_MODE*/
			TX_MODE(&sending);
			put_string("here");

			/*Wait for transmission end*/
			while(nrf24_isSending());

			/* Make analysis on last tranmission attempt */
			temp = rf_lastMessageStatus();

			if(temp == 0)
			{
				put_string("> Tranmission went OK\r\n");
			}
			else if(temp == 1)
			{
				put_string("> Message is lost ...\r\n");
			}

			/* Retranmission count indicates the tranmission quality */
			temp = nrf24_retransmissionCount();
			put_string("> Retranmission count:\r\n");
			put_int(temp);
			/* Go to RX MODE ... */
			RX_MODE();

			//put_int(i);

			_delay_ms(10);
		//}
	}
}
