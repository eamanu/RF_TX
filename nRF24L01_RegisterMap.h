/*
 * nRF24L01_RegisterMap.h
 *
 *  Created on: Jul 9, 2016
 *      Author: root
 */

#ifndef COMM_NRF24L01_REGISTERMAP_H_
#define COMM_NRF24L01_REGISTERMAP_H_

#define CONFIG_NRF24L01 0x00 /*Configuration Register*/
#define MASK_RX_DR 6
#define MASK_TX_DS 5
#define MASK_MAX_RT 4
#define EN_CRC 3
#define CRCO 2
#define PWR_UP 1
#define PRIM_RX 0

#define EN_AA 0x01 /*Enable ‘Auto Acknowledgment’ Function Disable
						this functionality to be compatible with nRF2401*/
#define ENAA_P5 5
#define ENAA_P4 4
#define ENAA_P3 3
#define ENAA_P2 2
#define ENAA_P1 1
#define ENAA_P0 0

#define EN_RXADDR 0x02 /*Enabled RX Addresses*/
#define ERX_P5 5
#define ERX_P4 4
#define ERX_P3 3
#define ERX_P2 2
#define ERX_P1 1
#define ERX_P0 0

#define SETUP_AW 0x03 /*Setup of Address Widths (common for all data pipes)*/
#define AW 0

#define SETUP_RETR 0x04 /* Setup of Automatic Retransmission */
#define ARD 4
#define ARC 0

#define RF_CH 0x05 /*RF Channel*/
#define RF_CH_BIT 0

#define RF_SETUP 0x06
#define CONT_WAVE 7
#define RF_DR_LOW 5
#define PLL_LOCK 4
#define RF_DR_HIGH 3
#define RF_PWR 1

#define STATUS 0x07 /*Status Register*/
#define RX_DR 6
#define TX_DS 5
#define MAX_RT 4
#define RX_P_NO 1
#define TX_FULL_STATUS 0

#define OBSERVE_TX 0x08 /*Transmit observe register*/
#define PLOS_CNT 4
#define ARC_CNT 0

#define RPD 0x09
#define RPD_BIT 0
#define NRF24L01_REG_RF_PWR_LOW  1
#define NRF24L01_REG_RF_PWR_HIGH 2

#define RX_ADDR_P0 0x0A /*Receive address data pipe 0. 5 Bytes maximum
							length*/
#define RX_ADDR_P1 0x0B /*Receive address data pipe 1. 5 Bytes maximum
							length*/
#define RX_ADDR_P2 0x0C /*Receive address data pipe 2. 5 Bytes maximum
							length*/
#define RX_ADDR_P3 0x0D /*Receive address data pipe 3. 5 Bytes maximum
							length*/
#define RX_ADDR_P4 0x0E /*Receive address data pipe 4. 5 Bytes maximum
							length*/
#define RX_ADDR_P5 0x0F /*Receive address data pipe 5. 5 Bytes maximum
							length*/

#define TX_ADDR 0x10 /*Transmit address. Used for a PTX device only.*/

#define RX_PW_P0 0x11
#define RX_PW_P0_BIT 0

#define RX_PW_P1 0x12
#define RX_PW_P1_BIT 0

#define RX_PW_P2 0x13
#define RX_PW_P2_BIT 0

#define RX_PW_P3 0x14
#define RX_PW_P3_BIT 0

#define RX_PW_P4 0x15
#define RX_PW_P4_BIT 0

#define RX_PW_P5 0x16
#define RX_PW_P5_BIT 0

#define FIFO_STATUS 0x17
#define TX_REUSE 6
#define TX_FULL 5
#define TX_EMPTY 4
#define RX_FULL 1
#define RX_EMPTY 0

#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define NOP           0xFF
#define R_RX_PL_WID	  0x60

#endif /* COMM_NRF24L01_REGISTERMAP_H_ */
