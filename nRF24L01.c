/*
 * nRF24L01.c
 *
 *  Created on: Jul 9, 2016
 *      Author: root
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "spi.h"
// comm
#include "nRF24L01.h"
#include "uart.h"

// Flag which denotes transmitting mode
volatile uint8_t PTX;

void rf_init(void){
	//set output pin
	DDRB |= (1<<CE)|(1<<CSN);

	PORTB |= (1<<CSN);
	PORTB &= ~(1<<CE);

	//set interrupt in pin PWM 10
	PORTB &= ~(1<<PB4);
	PCMSK0 = (1<<PCINT4);
	PCICR  = (1<<PCIE0);

	//inizialize spi
	init_spi();
}

void rf_w_config_register(uint8_t reg, uint8_t data){
	PORTB &= ~(1<<CSN);
	spi_tranceiver(W_REGISTER | (REGISTER_MASK & reg));
	spi_tranceiver(data);
	PORTB |= (1<<CSN);
}

void rf_config(){
	// set rf channel
	rf_w_config_register(RF_CH, 2);

	//set payload lenght data
	rf_w_config_register(RX_PW_P0, 16);

	//set enabled data pipe
	rf_w_config_register(EN_RXADDR,(1<<ENAA_P0));

	// start receiver mode
	PTX = 0;
	rf_w_config_register(CONFIG_NRF24L01, ( (1<<MASK_RX_DR) | (1<<EN_CRC) | (0<<CRCO) ) | (1<<PRIM_RX | 1<<PWR_UP));
	PORTB |= (1<<CE);
}

//write register
void rf_write_register(uint8_t reg, uint8_t * data, uint8_t len){
	PORTB &= ~(1<<CSN);
	spi_tranceiver(W_REGISTER | (REGISTER_MASK & reg));
	spi_send(data, len);
	PORTB |= (1<<CSN);
}

//read a register
void rf_read_register(uint8_t reg, uint8_t * data, uint8_t len){
	PORTB &= ~(1<<CSN);
	spi_tranceiver(R_REGISTER | (REGISTER_MASK & reg));
	spi_transfer(data,data,len);
	PORTB |= (1<<CSN);
}

//set address receiver
void set_RADDR(uint8_t *add){
	PORTB &= ~(1<<CE);
	rf_write_register(RX_ADDR_P0, add, 5);
	PORTB |= (1<<CE);
}

//set address transmisser
void set_TADDR(uint8_t *add){
	rf_write_register(TX_ADDR, add, 5);
}

uint8_t rf_data_ready(){
	if (PTX) return 0;
	uint8_t status;
	PORTB &= ~(1<<CSN);
	status = spi_tranceiver(NOP);
	PORTB |= (1<<CSN);
	return status & (1<<RX_DR);
}

void rf_get_data(uint8_t * data){
	PORTB &= ~(1<<CSN);
	spi_tranceiver(R_RX_PAYLOAD); //send command to read payload data
	spi_transfer(data,data,32); //read data payload
	PORTB |= (1<<CSN);
	rf_w_config_register(STATUS,(1<<RX_DR));
}

void rf_send(uint8_t * data, uint8_t len){
	while (PTX) {} //wait until the last packet is sent
	put_string("debug1");

	PORTB &= ~(1<<CE);

	PTX = 1; // transmiting
	rf_w_config_register(CONFIG_NRF24L01, ( (1<<MASK_RX_DR) | (1<<EN_CRC) | (0<<CRCO) ) | (1<<PRIM_RX | 0<<PWR_UP));

	PORTB &= ~(1<<CSN);
	spi_tranceiver(FLUSH_TX); // Write cmd to flush tx fifo
	PORTB |= (1<<CSN);

	PORTB &= ~(1<<CSN);
	spi_tranceiver(W_TX_PAYLOAD); // Write command to write payload
	spi_send(data, len); //write data payload
	PORTB |= (1<<CSN);

	PORTB |= (1<<CE);

}

SIGNAL (PCINT0_vect){
	uint8_t status;
	if(PTX){
		// read the rf status
		PORTB &= ~(1<<CSN);
		status = spi_tranceiver(NOP);// read the status
		PORTB |= (1<<CSN);

		PORTB &= ~(1<<CE);
		// go to receive mode
		rf_w_config_register(CONFIG_NRF24L01, ( (1<<MASK_RX_DR) | (1<<EN_CRC) | (0<<CRCO) ) | (1<<PRIM_RX | 1<<PWR_UP));
		PORTB |= (1<<CE);

		// receive mode
		PTX = 0;

		// reset status register
		rf_w_config_register(STATUS, (1<<TX_DS)|(1<<MAX_RT));

	}
}

