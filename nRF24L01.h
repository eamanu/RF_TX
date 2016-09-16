/*
 * nRF24L01.h
 *
 *  Created on: Jul 9, 2016
 *      Author: root
 */

#ifndef COMM_NRF24L01_H_
#define COMM_NRF24L01_H_

#include "nRF24L01_RegisterMap.h"
#include <avr/io.h>

#define CE PORTB1 /*Digital pin 09*/
#define CSN PORTB2 /*Digital pin 10*/

uint8_t pay_len;

/*write a cmd*/
void rf_w_config_register(uint8_t reg, uint8_t data);

/*write register*/
void rf_write_register(uint8_t reg, uint8_t * data, uint8_t len);

/*read a register*/
void rf_read_register(uint8_t reg, uint8_t * data, uint8_t len);

/*RF config*/
void rf_config(uint8_t channel, uint8_t pay_length);

/*Set RX addr*/
void set_rx_address(uint8_t * adr);

/* Set the TX address */
void set_tx_address(uint8_t* adr);

/* Checks if RX FIFO is empty*/
uint8_t isRXFifoEmpty();

/*Init RF*/
void rf_init(void);

/*Standby-I*/
void StandbyI(void);

/*flush RX fifo*/
void nrf24l01_flushRXfifo(void);

/* flush RX fifo */
void nrf24l01_flushTXfifo(void);

/*RX MODE*/
void RX_MODE(void);

/*Set power Level*/
void setPowerLevel(void);

/*Know if there're data to read*/
uint8_t rf_data_ready();

/*Get Data from RX FIFO payload*/
void rf_get_data(uint8_t * data);

/* Checks if receive FIFO is empty or not */
//uint8_t isRXFifoEmpty();

/* Returns the length of data waiting in the RX fifo */
uint8_t RXFifoPayloadLength();

/* Returns the number of retransmissionn*/
uint8_t nrf24_retransmissionCount();

/*Send data pack to the default addr.*/
void TX_MODE(uint8_t *data);

/*Check if data is sending*/
uint8_t nrf24_isSending();

/*Power Down RF*/
void rf_powerDown();

/*Status of last message*/
uint8_t rf_lastMessageStatus();

#endif /* COMM_NRF24L01_H_ */

