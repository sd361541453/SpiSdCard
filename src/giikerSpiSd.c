/*
 * giikerSpiSd.c
 *
 *  Created on: 2021-1-25
 *      Author: Jason
 *
 * V1.0 Epaper
 * V1.2 spiSdCard
 */


#include "giikerSpiSd.h"
#include "giikerConfig.h"

/**************** log ****************/

#include "nrf_log_instance.h"
#include "nrf_log_types.h"

#ifndef GIIKER_CONFIG
#error "GIIKER_CONFIG Not Define."
#endif

#define NRF_LOG_MODULE_NAME GSSD
#define NRF_LOG_LEVEL       GSSD_CONFIG_LOG_LEVEL

#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

/**************** log ****************/




#include "giikerLog.h"
#include "nrf_delay.h"
#include "nrf_spim.h"
#include "pin_debuger.h"


#define USE_GPIO_SPI	1



#if USE_GPIO_SPI
/************************ SPI-GPIO ***************************/

static void giikerSpiInitSdcard(){

    nrf_gpio_cfg_output(PIN_SD_CS);
    GPIO_HIGH(PIN_SD_CS);

    nrf_gpio_cfg(PIN_SD_MISO,
    		NRF_GPIO_PIN_DIR_INPUT,
    		NRF_GPIO_PIN_INPUT_CONNECT,
    		NRF_GPIO_PIN_PULLUP,
    		NRF_GPIO_PIN_S0S1,
    		NRF_GPIO_PIN_NOSENSE);

    nrf_gpio_cfg_output(PIN_SD_SCK);
    GPIO_LOW(PIN_SD_SCK);

    nrf_gpio_cfg_output(PIN_SD_MOSI);
    GPIO_HIGH(PIN_SD_MOSI);

}

uint8_t gpioSpiHighSpeed = 0;
static void giikerSpiSdSpeedUp(){
	gpioSpiHighSpeed = 1;
}

#define GPIO_DELAY()	nrf_delay_us(3);
static uint8_t spiGpioByte(uint8_t out){
	uint8_t i;
	uint8_t in = 0;

	if(gpioSpiHighSpeed){
		for (i = 0; i < 8; i++){
			if(out & 0x80){
			    GPIO_HIGH(PIN_SD_MOSI);
			}else{
			    GPIO_LOW(PIN_SD_MOSI);
			}
		    GPIO_HIGH(PIN_SD_SCK);
		    in <<= 1;
		    if(GPIO_READ(PIN_SD_MISO)){
		    	in |= 0x01;
		    }
		    out <<= 1;
		    GPIO_LOW(PIN_SD_SCK);
		}
	    GPIO_HIGH(PIN_SD_MOSI);
		return in;
	}else{
		for (i = 0; i < 8; i++){
			if(out & 0x80){
			    GPIO_HIGH(PIN_SD_MOSI);
			}else{
			    GPIO_LOW(PIN_SD_MOSI);
			}
			GPIO_DELAY();
		    GPIO_HIGH(PIN_SD_SCK);
			GPIO_DELAY();
		    in <<= 1;
		    if(GPIO_READ(PIN_SD_MISO)){
		    	in |= 0x01;
		    }
		    out <<= 1;
		    GPIO_LOW(PIN_SD_SCK);
		}
	    GPIO_HIGH(PIN_SD_MOSI);
		return in;
	}
}

static void spiSdSendBytes(const uint8_t * tx, uint8_t * rx, uint32_t txLen, uint32_t rxLen){

	uint32_t i;
	uint32_t max;
	uint8_t temp;

	if(txLen > rxLen){
		max = txLen;
	}else{
		max = rxLen;
	}

	for (i = 0; i < max; i++) {
		if(txLen){
			txLen--;
			temp = spiGpioByte(*tx);
			tx++;

			if (rxLen) {
				rxLen--;
				*rx = temp;
				rx++;
			}
		} else {
			temp = spiGpioByte(0xff);
			if (rxLen) {
				rxLen--;
				*rx = temp;
				rx++;
			}
		}
	}

}

#else

/************************ SPI-DMA ***************************/

static void giikerSpiInitSdcard(){

    nrf_gpio_cfg_output(PIN_SD_CS);
    GPIO_HIGH(PIN_SD_CS);

    nrf_gpio_cfg(PIN_SD_MISO,
    		NRF_GPIO_PIN_DIR_INPUT,
    		NRF_GPIO_PIN_INPUT_DISCONNECT,
    		NRF_GPIO_PIN_PULLUP,
    		NRF_GPIO_PIN_S0S1,
    		NRF_GPIO_PIN_NOSENSE);


    nrf_spim_pins_set(NRF_SPIM0, PIN_SD_SCK, PIN_SD_MOSI, PIN_SD_MISO);
//    nrf_spim_frequency_set(NRF_SPIM0, NRF_SPIM_FREQ_8M);
    nrf_spim_frequency_set(NRF_SPIM0, NRF_SPIM_FREQ_125K);
    nrf_spim_configure(NRF_SPIM0, NRF_SPIM_MODE_0, NRF_SPIM_BIT_ORDER_MSB_FIRST);

    nrf_spim_orc_set(NRF_SPIM0, 0xFF);


    nrf_spim_tx_list_enable(NRF_SPIM0);
    nrf_spim_rx_list_enable(NRF_SPIM0);

    //int
//    nrf_spim_int_enable(NRF_SPIM0, NRF_SPIM_INT_END_MASK);
    //52810
//    NRFX_IRQ_PRIORITY_SET(SPIM0_SPIS0_IRQn, 6);
//    NRFX_IRQ_ENABLE(SPIM0_SPIS0_IRQn);


    nrf_spim_enable(NRF_SPIM0);

}

static void giikerSpiSdCardUninit(){

    nrf_gpio_cfg_default(PIN_SD_CS);
    GPIO_HIGH(PIN_SD_CS);


    nrf_spim_int_disable(NRF_SPIM0, NRF_SPIM_INT_END_MASK);

    //52810
//    NRFX_IRQ_PRIORITY_SET(SPIM0_SPIS0_IRQn, 2);
    NRFX_IRQ_DISABLE(SPIM0_SPIS0_IRQn);


    nrf_spim_disable(NRF_SPIM0);

}

static void giikerSpiSdSpeedUp(){
//	nrf_spim_frequency_set(NRF_SPIM0, NRF_SPIM_FREQ_500K);
//	nrf_spim_frequency_set(NRF_SPIM0, NRF_SPIM_FREQ_2M);
//	nrf_spim_frequency_set(NRF_SPIM0, NRF_SPIM_FREQ_4M);
	nrf_spim_frequency_set(NRF_SPIM0, NRF_SPIM_FREQ_8M);
}

static void giikerSpiSdSpeedDown(){
	nrf_spim_frequency_set(NRF_SPIM0, NRF_SPIM_FREQ_125K);
}

/************************ SPI-DMA ***************************/

static void spiSdSendBytes(const uint8_t * tx, uint8_t * rx, uint32_t txLen, uint32_t rxLen){

    nrf_spim_tx_buffer_set(NRF_SPIM0, tx, txLen);
    nrf_spim_rx_buffer_set(NRF_SPIM0, rx, rxLen);

    nrf_spim_event_clear(NRF_SPIM0, NRF_SPIM_EVENT_END);
    nrf_spim_task_trigger(NRF_SPIM0, NRF_SPIM_TASK_START);

	for (;;) {
		if (nrf_spim_event_check(NRF_SPIM0, NRF_SPIM_EVENT_END)) {
			break;
		} else {
			nrf_delay_us(5);
		}
	}

}

#endif



/***************************************************/


#define SPI_SD_DELAY() nrf_delay_us(50);


#define SD_TYPE_V1 1
#define SD_TYPE_V2SC 2
#define SD_TYPE_V2HC 3



static uint8_t txBuff[50];
static uint8_t rxBuff[50];

static uint8_t spiSdCardType;



/************************ CMD ***************************/


uint8_t spiSdSendCmd(uint8_t cmd, uint32_t arg, uint8_t crc) {

	txBuff[0] = 0xff;
	txBuff[1] = cmd | 0x40;
	txBuff[2] = arg >> 24;
	txBuff[3] = arg >> 16;
	txBuff[4] = arg >> 8;
	txBuff[5] = arg;
	txBuff[6] = crc;

	spiSdSendBytes(txBuff, rxBuff, 7, 0);

	for (uint8_t i = 0; i < 10; i++) {
		txBuff[0] = 0xff;
		rxBuff[0] = 0xff;

		spiSdSendBytes(txBuff, rxBuff, 1, 1);

		if(rxBuff[0] != 0xff){
			return rxBuff[0];
		}
	}

	return 0xff;
}


void spiSdSendReset512() {

    GPIO_HIGH(PIN_SD_CS);

    uint8_t i;
	for (i = 0; i < 20; i++) {
		txBuff[i] = 0xff;
	}

	for (i = 0; i < 60; i++) {
		spiSdSendBytes(txBuff, rxBuff, 20, 0);
	}

    GPIO_HIGH(PIN_SD_CS);

}


void spiSdSendReset74() {

    GPIO_HIGH(PIN_SD_CS);


	txBuff[0] = 0xff;
	txBuff[1] = 0xff;
	txBuff[2] = 0xff;
	txBuff[3] = 0xff;
	txBuff[4] = 0xff;
	txBuff[5] = 0xff;
	txBuff[6] = 0xff;
	txBuff[7] = 0xff;
	txBuff[8] = 0xff;
	txBuff[9] = 0xff;


	spiSdSendBytes(txBuff, rxBuff, 10, 10);

    GPIO_HIGH(PIN_SD_CS);

}

//0x08, 0x1aa, 0x87
uint8_t spiSdSendCmd8() {

	uint8_t r1 = spiSdSendCmd(CMD8, 0x000001aa, 0x87);

	txBuff[0] = 0xff;
	txBuff[1] = 0xff;
	txBuff[2] = 0xff;
	txBuff[3] = 0xff;

	spiSdSendBytes(txBuff, rxBuff, 4, 4);

	return r1;

}

/**
 * 0x58, 0x0, 0x0
 * OCR
 */
uint8_t spiSdSendCmd58() {

	uint8_t r1 = spiSdSendCmd(CMD58, 0, 0);


	txBuff[0] = 0xff;
	txBuff[1] = 0xff;
	txBuff[2] = 0xff;
	txBuff[3] = 0xff;

	spiSdSendBytes(txBuff, rxBuff, 4, 4);

	return r1;

}

//csd
uint8_t spiSdSendCmd9() {

	uint8_t r1 = spiSdSendCmd(CMD9, 0, 0);

	for (uint8_t i = 0; i < 21; i++) {
		txBuff[i] = 0xff;
	}

	spiSdSendBytes(txBuff, rxBuff, 21, 21);

	csd_t * csd = (csd_t *)&rxBuff[2];
	uint32_t cap = sdCardCapacity(csd);

//	uint32_t size = csd->v2.c_size_high;
//	size <<= 8;
//	size |= csd->v2.c_size_mid;
//	size <<= 8;
//	size |= csd->v2.c_size_low;
//	size += 1;
//	print("size %d mb", size);

	print("CSD: ");
	print("%x %x %x %x %x %x %x %x", rxBuff[2], rxBuff[3], rxBuff[4], rxBuff[5], rxBuff[6], rxBuff[7], rxBuff[8], rxBuff[9]);
	print("%x %x %x %x %x %x %x %x", rxBuff[10], rxBuff[11], rxBuff[12], rxBuff[13], rxBuff[14], rxBuff[15], rxBuff[16], rxBuff[17]);
	print("CSD ver: %d", csd->v1.csd_ver + 1);
	print("cap %d mb", cap >> 11);


	return r1;

}


uint8_t spiSdSendCmd10() {

	uint8_t r1 = spiSdSendCmd(CMD10, 0, 0);

	for (uint8_t i = 0; i < 21; i++) {
		txBuff[i] = 0xff;
	}

	spiSdSendBytes(txBuff, rxBuff, 21, 21);

	cid_t * cid = (cid_t *)&rxBuff[2];


	print("oid:%c%c", cid->oid[0], cid->oid[1]);
	print("pnm:%c%c%c%c%c", cid->pnm[0], cid->pnm[1], cid->pnm[2], cid->pnm[3], cid->pnm[4]);

	return r1;

}



/************************ RW ***************************/

static uint8_t SpiSdErrR1(uint8_t r1) {
	if(r1 == 0xFF){
		return SPI_SD_ERR_CMD_TIMEOUT;
	}
	if(r1 & 0x80){
		return SPI_SD_ERR_R1_INVALID;
	}
	if(r1 & 0x40){
		return SPI_SD_ERR_PARAM;
	}
	if(r1 & 0x20){
		return SPI_SD_ERR_ADDR;
	}
	if(r1 & 0x10){
		return SPI_SD_ERR_ERASE_SEQ;
	}
	if(r1 & 0x08){
		return SPI_SD_ERR_CRC;
	}
	if(r1 & 0x04){
		return SPI_SD_ERR_CMD;
	}
	if(r1 & 0x02){
		return SPI_SD_ERR_ERASE_RES;
	}
	if(r1 & 0x01){
		return SPI_SD_ERR_IDLE;
	}
	return 0;
}


static uint8_t SpiSdErrTokenWrite(uint8_t token) {

	if(token & 0x10){
		return SPI_SD_ERR_TKN_INVALID;
	}else if(token & 0x01 == 0){
		return SPI_SD_ERR_TKN_INVALID;
	}else if(token & 0x0E == 0x0A){
		return SPI_SD_ERR_TKN_CRC;
	}else if(token & 0x0E == 0x0C){
		return SPI_SD_ERR_TKN_WRT;
	}else if(token & 0x0E == 0x04){
		return 0;
	}else{
		return SPI_SD_ERR_TKN_INVALID;
	}

}


/**
 * 0x00: success
 */
uint8_t spiSdReadSingleBlock(uint32_t sec, uint8_t *data) {
	uint8_t r1;
    GPIO_LOW(PIN_SD_CS);

	if (spiSdCardType == SD_TYPE_V1 || spiSdCardType == SD_TYPE_V2SC) {
		sec <<= 9;
	}

	// send CMD17
	r1 = spiSdSendCmd(CMD17, sec, 0);

	// if response received from card
	if (r1 == 0x00) {

		/**
		 * wait for a response token (timeout = 100ms)
		 * V2SC: 0.8ms
		 * V2HC: 0.6ms
		 * V2HC power on reset: 10ms
		 */
		for(uint16_t i = 0; i < 500; i++){

			txBuff[0] = 0xff;
			rxBuff[0] = 0xff;
			spiSdSendBytes(txBuff, rxBuff, 1, 1);

			if (rxBuff[0] != 0xFF){
				break;
			}
			SPI_SD_DELAY();
		}

		if(rxBuff[0] == 0xff){
		    GPIO_HIGH(PIN_SD_CS);
			return SPI_SD_ERR_BUSY_TIMEOUT;
		}else if(rxBuff[0] == 0xfe){
		}else if(rxBuff[0] == 0xfc){
//		}else if(rxBuff[0] == 0xfd){//???
		}else{
		    GPIO_HIGH(PIN_SD_CS);
			return SPI_SD_ERR_TKN_INVALID;
		}

		spiSdSendBytes(txBuff, data, 0, 512);

		// read 16-bit CRC
		txBuff[0] = 0xff;
		txBuff[1] = 0xff;
		txBuff[2] = 0xff;
		spiSdSendBytes(txBuff, rxBuff, 3, 0);

	    GPIO_HIGH(PIN_SD_CS);
		return 0;
	}else{
	    GPIO_HIGH(PIN_SD_CS);
		return SpiSdErrR1(r1);
	}

}


/**
 *	0x00: success
 */
uint8_t spiSdWriteSingleBlock(uint32_t sec, const uint8_t *data) {
	uint8_t r1;
	uint8_t token;

    GPIO_LOW(PIN_SD_CS);

	if (spiSdCardType == SD_TYPE_V1 || spiSdCardType == SD_TYPE_V2SC) {
		sec <<= 9;
	}

	r1 = spiSdSendCmd(CMD24, sec, 0x00);

	if (r1 != 0x00) {
	    GPIO_HIGH(PIN_SD_CS);
		return SpiSdErrR1(r1);
	}

	//token
	txBuff[0] = 0xff;
	txBuff[1] = 0xff;
	txBuff[2] = 0xff;
	txBuff[3] = 0xfe;
	spiSdSendBytes(txBuff, rxBuff, 4, 4);

	//data
	spiSdSendBytes(data, rxBuff, 512, 0);

	//crc
	txBuff[0] = 0xff;
	txBuff[1] = 0xff;
	spiSdSendBytes(txBuff, rxBuff, 2, 0);

	//token
	txBuff[0] = 0xff;
	rxBuff[0] = 0xff;
	spiSdSendBytes(txBuff, rxBuff, 1, 1);

	//token
	token = rxBuff[0] & 0x1F;
//	if (token != 0x05 && token != 0x00) {
	if (token != 0x05) {
	    GPIO_HIGH(PIN_SD_CS);
		return SpiSdErrTokenWrite(rxBuff[0]);
	}

	/**
	 * V2SC: 2.9ms
	 * 8G V2HC: 2.12ms
	 * 512 V2SC: 174ms, 215ms
	 */
	for(uint16_t i = 0; i < 8000; i++){

		txBuff[0] = 0xff;
		rxBuff[0] = 0;
		spiSdSendBytes(txBuff, rxBuff, 1, 1);

		if(rxBuff[0] != 0x00){
		    GPIO_HIGH(PIN_SD_CS);
			return 0;
		}
		SPI_SD_DELAY();
	}

    GPIO_HIGH(PIN_SD_CS);
	return SPI_SD_ERR_BUSY_TIMEOUT;
}


/************************ INIT ***************************/




/**
 * ret
 * 	0x00: init ok
 * 	0x01: err
 * 	0x02: err
 */
uint8_t spiSdInit(){

	uint8_t r1 = 0;

	giikerSpiInitSdcard();

	spiSdSendReset512();

	spiSdSendReset74();

    GPIO_LOW(PIN_SD_CS);

    r1 = spiSdSendCmd(CMD0, 0, 0x95);

    if(r1 != 0x01){
		print("cmd0 Err. r1=%x", r1);
        GPIO_HIGH(PIN_SD_CS);
    	return 1;
    }

    r1 = spiSdSendCmd8();
    r1 &= 0xfe;


	if (r1 == 0x00) {//sd2.0
		print("SD_TYPE_V2.0");

		/**
		 * 8G SDHC Power on reset: 800ms
		 */
		for (uint16_t i = 0; i < 2800; i++) {
			r1 = spiSdSendCmd(CMD55, 0, 0);
			r1 = spiSdSendCmd(ACMD41, 0x40000000, 1);
			if (r1 == 0x01) {
				SPI_SD_DELAY();
				continue;
			}else if(r1 == 0x00){
				print("i = %d", i);
				break;
			}else{
				print("acmd41 Err. r1 = %x", r1);
			    GPIO_HIGH(PIN_SD_CS);
				return 3;
			}
		}

		if (r1 == 0x01) {
			print("acmd41 Err. timeout");
		    GPIO_HIGH(PIN_SD_CS);
			return 3;
		}else{
			r1 = spiSdSendCmd58();
			if(rxBuff[0] & 0x40){//ccs
				print("SD_TYPE_V2HC");
				spiSdCardType = SD_TYPE_V2HC;
			}else{
				print("SD_TYPE_V2SC");
				spiSdCardType = SD_TYPE_V2SC;
			}
		}


	}else if(r1 & 0x04){//sd1.0
		print("SD_TYPE_V1");
		spiSdCardType = SD_TYPE_V1;


		r1 = spiSdSendCmd58();

		for (uint16_t i = 0; i < 2800; i++) {
			r1 = spiSdSendCmd(CMD55, 0, 0);
			r1 = spiSdSendCmd(ACMD41, 0, 0);
			if (r1 == 0x01) {
				SPI_SD_DELAY();
				continue;
			}else if(r1 == 0x00){
				break;
			}else{
				print("acmd41 Err. r1 = %x", r1);
			    GPIO_HIGH(PIN_SD_CS);
				return 3;
			}
		}

		if (r1 == 0x01) {
			print("acmd41 Err. timeout");
		    GPIO_HIGH(PIN_SD_CS);
			return 3;
		}else{
			r1 = spiSdSendCmd58();
		}

	}else{
		print("cmd8 Err. r1=%x", r1);
        GPIO_HIGH(PIN_SD_CS);
		return 2;
	}

    giikerSpiSdSpeedUp();

    print("****************** CID ******************");

    spiSdSendCmd10();

    print("****************** CSD ******************");

    spiSdSendCmd9();

    print("******************  ******************");

    GPIO_HIGH(PIN_SD_CS);

    return 0;
}

















