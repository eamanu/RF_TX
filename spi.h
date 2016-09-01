/*
 * spi.h
 *
 *  Created on: Jul 9, 2016
 *      Author: root
 */

#ifndef SPI_SPI_H_
#define SPI_SPI_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#define MOSI PORTB3 /*Digital Pin 11*/
#define MISO PORTB4 /*Digital pin 12*/
#define SCK PORTB5 /*Digital pin 13 (SCK)*/
#define SS PORTB2 /*Digital pin 10 (SS)*/

//#define DD_MOSI     DDB3

/*init spi*/
void init_spi(void);
/*spi send array with return data*/
void spi_transfer(uint8_t *out, uint8_t *in, uint8_t len);
/*spi send array with without return data*/
void spi_send(uint8_t *out, uint8_t len);
/*send a byte*/
uint8_t spi_tranceiver(uint8_t data);


#endif /* SPI_SPI_H_ */
