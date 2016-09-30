/**
 * @file main.c
 * @author Arias Emmanuel.
 * @date Jul 12, 2016
 * @brief Ejemplo de transmisor usando nRF24L01+.
 *
 * Para el transmisor se utiliza el módulo nRF24L01 + arduino Nano.
 *
 * La disposición de los pines es la siguiente
 *
 * - GND    >  GND
 * - VCC    >  3.3V
 * - CE     >  D9
 * - CSN    >  D10
 * - SCK    >  D13
 * - MOSI 	 >  D11
 * - MISO 	 >  D12
 * - IRQ    >  No usado
 *
 *	@see http://freektc.blogspot.com.ar/2016/09/nrf24l01-usando-arduino-avr-ii.html
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>

//USART communication
#include "uart.h"
//nRF24L01 lib
#include "nRF24L01.h"


/*define addr*/
uint8_t tx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};/**<Dirección del transmisor*/
uint8_t rx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};/**<Dirección del receptorr*/

int main(){
	//inizialize usart
	usart_init();

	//inizialize rf tx
	rf_init();

	rf_config(2,1);/**<Se configura el radio. Chanel 2. Size payload: 1byte*/

	//set the device addr
	set_rx_address(rx_address);
	set_tx_address(tx_address);

	//uint8_t reg = 0x07;
	uint8_t reg = CONFIG_NRF24L01;
	uint8_t data;
	uint8_t temp;
	uint8_t sending = 10;
	while(1){

		for(uint8_t i = 0; i<10; i++){
			/*Go to TX_MODE*/
			TX_MODE(&i);

			/*Wait for transmission end*/
			while(nrf24_isSending());

			/* Make analysis on last tranmission attempt */
			temp = rf_lastMessageStatus();

			if(temp == 0)
			{
				put_string("> Tranmisión ok\r\n");
			}
			else if(temp == 1)
			{
				put_string("> Mensaje perdido...\r\n");
			}


			/* Go to RX MODE ... */
			RX_MODE();

			_delay_ms(10);
		//}
	}
}
