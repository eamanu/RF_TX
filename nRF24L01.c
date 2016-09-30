/**
 * @file nRF24L01.c
 * @date Jul 9, 2016
 * @author Arias Emmanuel
 * @brief driver del nRF24L01.
 * @see http://freektc.blogspot.com.ar/2016/09/nrf24l01-usando-arduino-avr-ii.html
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "spi.h"
// comm
#include "nRF24L01.h"
#include "uart.h"

//volatile int PTX = 0;
volatile int DATA_TO_SEND = 0;

/*
#define PORTB &= ~(1<<CE) PORTB &= ~(1<<CE);
#define PORTB |= (1<<CE) PORTB |= (1<<CE);

#define PORTB &= ~(1<<CSN) PORTB &= ~(1<<CSN);
#define PORTB |= (1<<CSN) PORTB |= (1<<CSN);
*/

/**
 *	@brief Setea los pines de salida. Inicializa SPI.
 *	@param void
 *	@return void
 */
void rf_init(void){
	init_spi();
	//set output pin
	DDRB |= (1<<CE)|(1<<CSN);

	PORTB &= ~(1<<CE); // LOW CE
	PORTB |= (1<<CSN); // CSN HIGH
}

/**
 *	@brief Configura el radio
 *	@param[in] channel Indica el canal de transmisión
 *	@param[in] pay_length Indica el lenght del payload
 *
 *	@return void
 */
void rf_config(uint8_t channel, uint8_t pay_length){
	//set power level
	//setPowerLevel();

	pay_len = 1;

	// set rf channel
	nrf24_configRegister(RF_CH, channel);

	//set payload lenght data
	nrf24_configRegister(RX_PW_P0, 0x00);
	nrf24_configRegister(RX_PW_P1, pay_len); // Data payload pipe
	nrf24_configRegister(RX_PW_P2, 0x00);
	nrf24_configRegister(RX_PW_P3, 0x00);
	nrf24_configRegister(RX_PW_P4, 0x00);
	nrf24_configRegister(RX_PW_P5, 0x00);

	// 1 Mbps, TX gain: 0dbm
	nrf24_configRegister(RF_SETUP, (0<<RF_DR_HIGH) | ((0x03)<<RF_PWR));

	// CRC enable, 1 byte CRC length
	nrf24_configRegister(CONFIG_NRF24L01,((1<<EN_CRC)|(0<<CRCO)));

	//enable auto ACK
	nrf24_configRegister(EN_AA, (1<<ENAA_P0)|(1<<ENAA_P1)|(0<<ENAA_P2)|(0<<ENAA_P3)|(0<<ENAA_P4)|(0<<ENAA_P5));

	// Enable RX addresses
	nrf24_configRegister(EN_RXADDR,(1<<ERX_P0)|(1<<ERX_P1)|(0<<ERX_P2)|(0<<ERX_P3)|(0<<ERX_P4)|(0<<ERX_P5));

	// set retries
	// Auto retransmit delay: 1000 us and Up to 15 retransmit trials
	nrf24_configRegister(SETUP_RETR,((0x04<<ARD)|(0x0F<<ARC)));

	 // Dynamic length configurations: No dynamic length
	nrf24_configRegister(DYNPD,(0<<DPL_P0)|(0<<DPL_P1)|(0<<DPL_P2)|(0<<DPL_P3)|(0<<DPL_P4)|(0<<DPL_P5));

	// start receiver mode
	RX_MODE();

}

/**
 *	@brief Setea la dirección de receptor
 *	@param[in] adr dirección
 *
 *	@return void
 */
void set_rx_address(uint8_t * adr)
{
    PORTB &= ~(1<<CE);
    rf_write_register(RX_ADDR_P1,adr,5);
    PORTB |= (1<<CE);
}

/**
 *	@brief Setea la dirección del transmisor
 * 	@param[in] adr	Dirección transmisor
 *
 * 	@return void
 */
void set_tx_address(uint8_t* adr)
{
	rf_write_register(RX_ADDR_P0,adr,5);
	rf_write_register(TX_ADDR,adr,5);

}

/**
 *	@brief Función que escribe los registros del nRF24L01.
 * 	@param[in] reg	Registro a escribir
 * 	@param[in] data	Dato a escribir
 *
 * 	@return void
 */
void nrf24_configRegister(uint8_t reg, uint8_t data){
	//PORTB &= ~(1<<CSN);
	PORTB &= ~(1<<CSN);
	spi_tranceiver(W_REGISTER | (REGISTER_MASK & reg));
	spi_tranceiver(data);
	//PORTB |= (1<<CSN);
	PORTB |= (1<<CSN);
}

/**
 *	@brief Función que escribe los registros del nRF24L01. Y que requiere como parámetro
 *	el ancho del dato que se va a escribir.
 *
 * 	@param[in] reg	Registro a escribir
 * 	@param[in] data	Dato a escribir
 * 	@param[in] len	Tamaño del ancho
 *
 * 	@return void
 */
void rf_write_register(uint8_t reg, uint8_t * data, uint8_t len){
	PORTB &= ~(1<<CSN);
	spi_tranceiver(W_REGISTER | (REGISTER_MASK & reg));
	spi_send(data, len);
	PORTB |= (1<<CSN);
}

/**
 *	@brief Función que lee los registros del nRF24L01.
 *
 * 	@param[in] reg	Registro a leer
 * 	@param[out] data Donde se va a escribir el dato
 * 	@param[in] len	Tamaño del ancho
 *
 * 	@return void
 */
void rf_read_register(uint8_t reg, uint8_t * data, uint8_t len){
	PORTB &= ~(1<<CSN);
	spi_tranceiver(R_REGISTER | (REGISTER_MASK & reg));
	spi_transfer(data,data,len);
	PORTB |= (1<<CSN);
}

/**
 *	@brief Setea el power del transmisor.
 */
void setPowerLevel(){
	uint8_t setup;
	rf_read_register(RF_SETUP, &setup, 1);
	setup |= (1<<NRF24L01_REG_RF_PWR_LOW) | (1<<NRF24L01_REG_RF_PWR_HIGH);
	rf_write_register(RF_SETUP, &setup, 1);
}



/**
 *	@brief Setea el ancho del payload.
 *
 */
uint8_t RXFifoPayloadLength()
{
    uint8_t status;
    PORTB &= ~(1<<CSN);
    spi_tranceiver(R_RX_PL_WID);
    status = spi_tranceiver(0x00);
    PORTB |= (1<<CSN);
    return status;
}


/**
 * @brief se limpia el FIFO de RX
 */
void nrf24l01_flushRXfifo() {
	PORTB &= ~(1<<CSN);
	spi_tranceiver(FLUSH_RX);
	PORTB |= (1<<CSN);
}

/**
 *	@brief Se limpia el FIFO de TX
 */
void nrf24l01_flushTXfifo() {
	PORTB &= ~(1<<CSN); //low CSN
	spi_tranceiver(FLUSH_TX);
	PORTB |= (1<<CSN); //high CSN
}

/**
 * @brief se pasa al modo RX.
 *
 */
void RX_MODE(){
	//PTX = 0; // RX MODE

	nrf24l01_flushRXfifo();

	nrf24_configRegister(STATUS, (1<<MASK_RX_DR | 1<<MASK_TX_DS | 1<<MASK_MAX_RT));

	//PORTB &= ~(1<<CE);
	PORTB &= ~(1<<CE);
	nrf24_configRegister(CONFIG_NRF24L01, (1<<EN_CRC)|(0<<CRCO) | (1<<PRIM_RX) | (1<<PWR_UP));
	//PORTB |= (1<<CE);
	PORTB |= (1<<CE);
	/*nrf24l01_flushTXfifo();
	clear rx_dr bit
	nrf24_configRegister(STATUS, (1<<RX_DR));*/
}

/**
 * @brief se pasa al modo standby
 *
 */
void StandbyI(void){
	nrf24_configRegister(CONFIG_NRF24L01, (1<<PWR_UP));
	_delay_ms(2);

	/*configure rf*/
	//rf_config();

}

/**
 * @brief Se pregunta si hay datos para leer
 *
 * return 1 si hay. Otro valor si no lo hay
 */
uint8_t rf_data_ready(){
	/*no data. It's tx mode*/
	//if (PTX) return 0;
	/*else*/
	uint8_t status;
	PORTB &= ~(1<<CSN);
	status = spi_tranceiver(NOP);
	PORTB |= (1<<CSN);

	if(status & (1<<RX_DR)){
		return 1;
	}
	return !isRXFifoEmpty();

}

/**
 * @brief chekea si el FIFO de RX está vacia
 *
 * @return 1 vacio. Otro no
 *
 */
uint8_t isRXFifoEmpty()
{
    uint8_t fifoStatus;

    rf_read_register(FIFO_STATUS,&fifoStatus,1);

    return (fifoStatus & (1 << RX_EMPTY));

}


/**
 * 	@brief extrae datos del FIFO RX.
 *
 * 	@param[out] data Dónde se almacena el dato leido
 *
 */
void rf_get_data(uint8_t * data){
	PORTB &= ~(1<<CSN);

	spi_tranceiver(R_RX_PAYLOAD); //send command to read payload data

	spi_transfer(data,data,pay_len); //read data payload

	PORTB |= (1<<CSN);

	nrf24_configRegister(STATUS,(1<<RX_DR));

}

/**
 * @brief Devuelve la cantidad de retransmisiones
 *
 * @return cantidad de retransmisiones
 */
uint8_t nrf24_retransmissionCount()
{
    uint8_t cnt;
    rf_read_register(OBSERVE_TX,&cnt,1);
    cnt = cnt & 0x0F;
    return cnt;
}

/**
 * 	@brief Pasa al modo TX
 * 	@param[in] data Dato a enviar
 *
 */
void TX_MODE(uint8_t *data){
	//while (PTX) {} //wait until the last packet is sent
	//PTX = 1; //TX mode
	//PORTB &= ~(1<<CE);
	PORTB &= ~(1<<CE);

	/*Power up TX*/
	nrf24_configRegister(STATUS, (1<<MASK_RX_DR | 1<<MASK_TX_DS | 1<<MASK_MAX_RT));
	nrf24_configRegister(CONFIG_NRF24L01, (1<<EN_CRC)|(0<<CRCO) | (0<<PRIM_RX) | (1<<PWR_UP));

	nrf24l01_flushTXfifo();

	PORTB &= ~(1<<CSN);

	spi_tranceiver(W_TX_PAYLOAD); // Write command to write payload
	spi_send(data, pay_len); //write data payload

	PORTB |= (1<<CSN);

	//PORTB |= (1<<CE);
	PORTB |= (1<<CE);

}

/**
 * 	@brief Indica si hay datos enviandose
 *
 * 	@return 0 si no hay datos. 1 si hay datos enviandose
 */
uint8_t nrf24_isSending()
{
    /* read the current status */
    uint8_t status;
    PORTB &= ~(1<<CSN);
    status = spi_tranceiver(NOP);
    PORTB |= (1<<CSN);

    //put_int(status);
    /* if sending successful (TX_DS) or max retries exceded (MAX_RT). */
    if((status & ((1 << TX_DS)  | (1 << MAX_RT))))
    {
        return 0; /* false */
    }

    return 1; /* true */

}

/**
 * 	@brief estado del últimomensaje que se envió
 *
 * 	@return 0 si la transmisión está ok. 1 Mensaje probablemente fue enviado. 0xFF probablemente enviado
 */
uint8_t rf_lastMessageStatus()
{
	/* read the current status */
	uint8_t status;
	PORTB &= ~(1<<CSN);
	status = spi_tranceiver(NOP);
	PORTB |= (1<<CSN);

    /* Transmission went OK */
    if((status & ((1 << TX_DS))))
    {
        return 0;
    }
    /* Maximum retransmission count is reached */
    /* Last message probably went missing ... */
    else if((status & ((1 << MAX_RT)))){
        return 1;
    }else{ /* Probably still sending ... */
        return 0xFF;
    }
}

/**
 * @brief Se apaga el RF
 *
 */
void rf_powerDown()
{
	PORTB &= ~(1<<CE);
	nrf24_configRegister(CONFIG_NRF24L01, ((1<<EN_CRC)|(0<<CRCO)));

	/*@Correct: Hasta acá ok*/
}

/**
 * @brief Manejo de las interrupciones. No implementado hasta el momento.
 *
 */

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
		nrf24_configRegister(CONFIG_NRF24L01, ((1<<MASK_TX_DS) | (1<<MASK_RX_DR) | (1<<EN_CRC) | (1<<CRCO) ) | (1<<PRIM_RX | 1<<PWR_UP));
		PORTB |= (1<<CE);

		// receive mode
		PTX = 0;

		// reset status register
		nrf24_configRegister(STATUS, (1<<TX_DS)|(1<<MAX_RT));

	}
}*/

