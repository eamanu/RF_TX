/*
 * nRF24L01.c
 *
 *  Created on: Jul 9, 2016
 *      Author: root
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "spi.h"
// comm
#include "nRF24L01.h"
#include "uart.h"

volatile int PTX = 0;
volatile int DATA_TO_SEND = 0;

void rf_w_config_register(uint8_t reg, uint8_t data){
	PORTB &= ~(1<<CSN);
	spi_tranceiver(W_REGISTER | (REGISTER_MASK & reg));
	spi_tranceiver(data);
	PORTB |= (1<<CSN);
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

/*Set power Level*/
void setPowerLevel(){
	uint8_t setup;
	rf_read_register(RF_SETUP, &setup, 1);
	setup |= (1<<NRF24L01_REG_RF_PWR_LOW) | (1<<NRF24L01_REG_RF_PWR_HIGH);
	rf_write_register(RF_SETUP, &setup, 1);
}

/*RF config*/
void rf_config(void){
	//set power level
	//setPowerLevel();

	// set rf channel
	rf_w_config_register(RF_CH, 76);

	//set payload lenght data
	rf_w_config_register(RX_PW_P0, 1);
	rf_w_config_register(RX_PW_P1, 1);
	rf_w_config_register(RX_PW_P2, 0x00);
	rf_w_config_register(RX_PW_P3, 0x00);
	rf_w_config_register(RX_PW_P4, 0x00);
	rf_w_config_register(RX_PW_P5, 0x00);

	// 1 Mbps, TX gain: 0dbm
	rf_w_config_register(RF_SETUP, (0<<RF_DR_HIGH) | ((0x03)<<RF_PWR));

	// CRC enable, 1 byte CRC length
	//nrf24_configRegister(CONFIG,nrf24_CONFIG);

	//enable auto ACK
	rf_w_config_register(EN_AA, (1<<ENAA_P0)|(1<<ENAA_P1)|(0<<ENAA_P2)|(0<<ENAA_P3)|(0<<ENAA_P4)|(0<<ENAA_P5));

	// Enable RX addresses
	rf_w_config_register(EN_RXADDR,(1<<ERX_P0)|(1<<ERX_P1)|(0<<ERX_P2)|(0<<ERX_P3)|(0<<ERX_P4)|(0<<ERX_P5));

	// set retries
	// Auto retransmit delay: 1000 us and Up to 15 retransmit trials
	rf_w_config_register(SETUP_RETR,(0x04<<ARD)|(0x0F<<ARC));

	//disable dynamic payloads
	rf_w_config_register(0x1C, 0x00);

	//set enabled data pipe
	//rf_w_config_register(EN_RXADDR,(1<<ENAA_P0));
	//rf_w_config_register(EN_RXADDR,0);
	//rf_w_config_register(EN_AA, ~(1<<ENAA_P0));
	//rf_w_config_register(EN_AA, 0);

	// start receiver mode
	RX_MODE();
}


/* Returns the length of data waiting in the RX fifo */
uint8_t RXFifoPayloadLength()
{
    uint8_t status;
    PORTB &= ~(1<<CSN);
    status = spi_tranceiver(R_RX_PL_WID);
    PORTB |= (1<<CSN);
    return status;
}


/*flush RX fifo*/
void nrf24l01_flushRXfifo() {
	PORTB &= ~(1<<CSN); //low CSN
	spi_tranceiver(FLUSH_RX);
	PORTB |= (1<<CSN); //high CSN
}

/* flush RX fifo */
void nrf24l01_flushTXfifo() {
	PORTB &= ~(1<<CSN); //low CSN
	spi_tranceiver(FLUSH_TX);
	PORTB |= (1<<CSN); //high CSN
}

void RX_MODE(){
	PTX = 0; // RX MODE
	nrf24l01_flushRXfifo();
	rf_w_config_register(STATUS, (1<<MASK_RX_DR | 1<<MASK_TX_DS | 1<<MASK_MAX_RT));

	PORTB &= ~(1<<CE);
	rf_w_config_register(CONFIG_NRF24L01, (1<<PRIM_RX) | (1<<PWR_UP));
	PORTB |= (1<<CE);
	/*nrf24l01_flushTXfifo();
	clear rx_dr bit
	rf_w_config_register(STATUS, (1<<RX_DR));*/

}

void StandbyI(void){
	rf_w_config_register(CONFIG_NRF24L01, (1<<PWR_UP));
	_delay_ms(2);

	/*configure rf*/
	rf_config();

}

void rf_init(void){
	//set output pin
	DDRB |= (1<<CE)|(1<<CSN);

	PORTB &= ~(1<<CE);
	PORTB |= (1<<CSN);


	_delay_ms(5);
	//cli();

	init_spi();

	/*Go to standby-I*/
	//StandbyI();
}

/*Know if there're data to read*/
uint8_t rf_data_ready(){
	/*no data. It's tx mode*/
	if (PTX) return 0;
	/*else*/
	uint8_t status;
	PORTB &= ~(1<<CSN);
	status = spi_tranceiver(NOP);
	PORTB |= (1<<CSN);
	return status & (1<<RX_DR);
}

/*Get Data from RX FIFO payload*/
void rf_get_data(uint8_t * data){
	PORTB &= ~(1<<CSN);
	spi_tranceiver(R_RX_PAYLOAD); //send command to read payload data
	spi_transfer(data,data,16); //read data payload
	PORTB |= (1<<CSN);
	rf_w_config_register(STATUS,(1<<RX_DR));
}

void TX_MODE(uint8_t *data, uint8_t len){
	//while (PTX) {} //wait until the last packet is sent
	PTX = 1; //TX mode
	PORTB &= ~(1<<CE);
	rf_w_config_register(STATUS, (1<<MASK_RX_DR | 1<<MASK_TX_DS | 1<<MASK_MAX_RT));
	rf_w_config_register(CONFIG_NRF24L01, (0<<PRIM_RX) | (1<<PWR_UP));

	nrf24l01_flushTXfifo();

	PORTB &= ~(1<<CSN);
	spi_tranceiver(W_TX_PAYLOAD); // Write command to write payload
	spi_send(data, len); //write data payload
	PORTB |= (1<<CSN);

	PORTB |= (1<<CE);

	uint8_t test;
	rf_read_register(STATUS, &test,1);
	put_int(test);
	_delay_us(150); //wait for the radio to power up
	//RX_MODE(); // go to RX_MODE
}

/*
ISR (INT0_vect){
	uint8_t status;
	put_string("pcint0");
	if(PTX){
		// read the rf status
		put_string("pcint0.if");
		PORTB &= ~(1<<CSN);
		status = spi_tranceiver(NOP);// read the status
		PORTB |= (1<<CSN);

		PORTB &= ~(1<<CE);
		// go to receive mode
		rf_w_config_register(CONFIG_NRF24L01, ((1<<MASK_TX_DS) | (1<<MASK_RX_DR) | (1<<EN_CRC) | (1<<CRCO) ) | (1<<PRIM_RX | 1<<PWR_UP));
		PORTB |= (1<<CE);

		// receive mode
		PTX = 0;

		// reset status register
		rf_w_config_register(STATUS, (1<<TX_DS)|(1<<MAX_RT));

	}
}*/

