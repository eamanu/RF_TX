/*
 * spi.c
 *
 *  Created on: Jul 9, 2016
 *      Author: Arias Emmanuel
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "spi.h"

void init_spi(void){

	//DDRB &= ~((1<<MOSI)|(1<<MISO)|(1<<SS)|(1<<SCK));
	//DDRB &= ~(1<<SCK);
	// pins like output
	DDRB = (1<<MOSI) | (1<<SCK) | (1<<SS);

	SPCR = ((1<<SPE)|               // SPI Enable
			(0<<SPIE)|              // SPI Interupt Enable
	        (0<<DORD)|              // Data Order (0:MSB first / 1:LSB first)
	        (1<<MSTR)|              // Master/Slave select
	        (0<<SPR1)|(1<<SPR0)|    // SPI Clock Rate
	        (0<<CPOL)|              // Clock Polarity (0:SCK low / 1:SCK hi when idle)
	        (0<<CPHA));             // Clock Phase (0:leading / 1:trailing edge sampling)

	SPSR = (1<<SPI2X);              // Double Clock Rate
}

uint8_t spi_tranceiver(uint8_t data){
	SPDR = data;
	while((SPSR & (1<<SPIF)) == 0);
	return SPDR;
}

/*spi send array with return data*/
void spi_transfer(uint8_t *out, uint8_t *in, uint8_t len){
	int i;
	for(i = 0; i<len; i++){
		SPDR = out[i];
		while((SPSR & (1<<SPIF))==0);
		in[i] = SPDR;
	}
}
void spi_send(uint8_t *out, uint8_t len){
	int i;
	for(i = 0; i<len; i++){
		SPDR = out[i];
		while((SPSR & (1<<SPIF))==0);
	}
}
