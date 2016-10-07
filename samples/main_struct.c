/**
 * @file main.c
 * @author Arias Emmanuel.
 * @date Jul 12, 2016
 * @brief Ejemplo de Struct usando nRF24L01+. Con respuesta.
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

struct Data_struct {
	uint8_t id;
	uint64_t timing;
} data_to_send, data_responce;

volatile unsigned long timer1_millis;
long milliseconds_since;

// the CTC match value in OCR1A.
#define CTC_MATCH_OVERFLOW ((F_CPU / 1000) / 8)

ISR (TIMER1_COMPA_vect)
{
    timer1_millis++;
}

uint64_t millis ()
{
    return timer1_millis;
}

void init_timer1 ()
{
	// CTC mode, Clock/8
	TCCR1B |= (1 << WGM12) | (1 << CS11);

	// Load the high byte, then the low byte
	// into the output compare
	OCR1AH = (CTC_MATCH_OVERFLOW >> 8);
	OCR1AL = CTC_MATCH_OVERFLOW;

	// Enable the compare match interrupt
	TIMSK1 |= (1 << OCIE1A);

	// PC0/Analog 0 to Output
	DDRC |= (1 << PC0);
}

int main(){

	//inizialize timer
	init_timer1();

	//inizialize usart
	usart_init();

	//inizialize rf tx
	rf_init();

	//enable interruptions
	sei();
	rf_config(2,9);/**<Se configura el radio. Chanel 2. Size payload: 9byte*/

	//set the device addr
	set_rx_address(rx_address);
	set_tx_address(tx_address);

	uint8_t i = 0;
	uint64_t temp, lastTiming;
	while(1){

		data_to_send.id = i;
		data_to_send.timing = millis();
		i++;
		/*Go to TX_MODE*/
		TX_MODE(&data_to_send);

		/*Sending data*/
		uint64_t temp = millis();

		/*Wait for transmission end*/
		while(nrf24_isSending());

		RX_MODE();

		lastTiming = millis();
		do{
			if(rf_data_ready())
				{
					rf_get_data(&data_responce);
					put_string("\nData recieve correctly\n");
					put_int(data_responce.id);
					put_string("\n");
					put_string("El mensaje tardo: ");
					put_long(millis() - data_responce.timing);

					break;
				}

		}while((lastTiming - temp) < 10000);

		if((lastTiming - temp) > 1000){
			put_string("\nSending Fails\n");
		}

		_delay_ms(10);

	}
}
