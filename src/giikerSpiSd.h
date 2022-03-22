/*
 * giikerSpiSd.h
 *
 *  Created on: 2021-1-25
 *      Author: Jason
 */

#ifndef GIIKERSPISD_H_
#define GIIKERSPISD_H_

#include <stdint.h>
#include <stdbool.h>



uint8_t spiSdInit();

//void giikerSpiSdCardUninit();
//void giikerSpiInitSdcard();


uint8_t spiSdReadSingleBlock(uint32_t sec, uint8_t *data);
uint8_t spiSdWriteSingleBlock(uint32_t sec, const uint8_t *data);



// SD card commands
/** GO_IDLE_STATE - init card in spi mode if CS low */
#define CMD0 0x00
/** ALL_SEND_CID - Asks any card to send the CID. */
#define CMD2 0x02
/** SEND_RELATIVE_ADDR - Ask the card to publish a new RCA. */
#define CMD3 0x03
/** SWITCH_FUNC - Switch Function Command */
#define CMD6 0x06
/** SELECT/DESELECT_CARD - toggles between the stand-by and transfer states. */
#define CMD7 0x07
/** SEND_IF_COND - verify SD Memory Card interface operating condition.*/
#define CMD8 0x08
/** SEND_CSD - read the Card Specific Data (CSD register) */
#define CMD9 0x09
/** SEND_CID - read the card identification information (CID register) */
#define CMD10 0x0A
/** VOLTAGE_SWITCH -Switch to 1.8V bus signaling level. */
#define CMD11 0x0B
/** STOP_TRANSMISSION - end multiple sector read sequence */
#define CMD12 0x0C
/** SEND_STATUS - read the card status register */
#define CMD13 0x0D
/** READ_SINGLE_SECTOR - read a single data sector from the card */
#define CMD17 0x11
/** READ_MULTIPLE_SECTOR - read multiple data sectors from the card */
#define CMD18 0x12
/** WRITE_SECTOR - write a single data sector to the card */
#define CMD24 0x18
/** WRITE_MULTIPLE_SECTOR - write sectors of data until a STOP_TRANSMISSION */
#define CMD25 0x19
/** ERASE_WR_BLK_START - sets the address of the first sector to be erased */
#define CMD32 0x20
/** ERASE_WR_BLK_END - sets the address of the last sector of the continuous
    range to be erased*/
#define CMD33 0x21
/** ERASE - erase all previously selected sectors */
#define CMD38 0x26
/** APP_CMD - escape for application specific command */
#define CMD55 0x37
/** READ_OCR - read the OCR register of a card */
#define CMD58 0x3A
/** CRC_ON_OFF - enable or disable CRC checking */
#define CMD59 0x3B
/** SET_BUS_WIDTH - Defines the data bus width for data transfer. */
#define ACMD6 0x06
/** SD_STATUS - Send the SD Status. */
#define ACMD13 0x0D
/** SET_WR_BLK_ERASE_COUNT - Set the number of write sectors to be
     pre-erased before writing */
#define ACMD23 0x17
/** SD_SEND_OP_COMD - Sends host capacity support information and
    activates the card's initialization process */
#define ACMD41 0x29








//==============================================================================
/**
 * \class CID
 * \brief Card IDentification (CID) register.
 */
typedef struct CID {
  // byte 0
  /** Manufacturer ID */
  unsigned char mid;
  // byte 1-2
  /** OEM/Application ID */
  char oid[2];
  // byte 3-7
  /** Product name */
  char pnm[5];
  // byte 8
  /** Product revision least significant digit */
  unsigned char prv_m : 4;
  /** Product revision most significant digit */
  unsigned char prv_n : 4;
  // byte 9-12
  /** Product serial number */
  uint32_t psn;
  // byte 13
  /** Manufacturing date year high digit */
  unsigned char mdt_year_high : 4;
  /** not used */
  unsigned char reserved : 4;
  // byte 14
  /** Manufacturing date month */
  unsigned char mdt_month : 4;
  /** Manufacturing date year low digit */
  unsigned char mdt_year_low : 4;
  // byte 15
  /** not used always 1 */
  unsigned char always1 : 1;
  /** CRC7 checksum */
  unsigned char crc : 7;
} __attribute__((packed)) cid_t;

//==============================================================================
/**
 * \class CSDV1
 * \brief CSD register for version 1.00 cards .
 */
typedef struct CSDV1 {
  // byte 0
  unsigned char reserved1 : 6;
  unsigned char csd_ver : 2;
  // byte 1
  unsigned char taac;
  // byte 2
  unsigned char nsac;
  // byte 3
  unsigned char tran_speed;
  // byte 4
  unsigned char ccc_high;
  // byte 5
  unsigned char read_bl_len : 4;
  unsigned char ccc_low : 4;
  // byte 6
  unsigned char c_size_high : 2;
  unsigned char reserved2 : 2;
  unsigned char dsr_imp : 1;
  unsigned char read_blk_misalign : 1;
  unsigned char write_blk_misalign : 1;
  unsigned char read_bl_partial : 1;
  // byte 7
  unsigned char c_size_mid;
  // byte 8
  unsigned char vdd_r_curr_max : 3;
  unsigned char vdd_r_curr_min : 3;
  unsigned char c_size_low : 2;
  // byte 9
  unsigned char c_size_mult_high : 2;
  unsigned char vdd_w_cur_max : 3;
  unsigned char vdd_w_curr_min : 3;
  // byte 10
  unsigned char sector_size_high : 6;
  unsigned char erase_blk_en : 1;
  unsigned char c_size_mult_low : 1;
  // byte 11
  unsigned char wp_grp_size : 7;
  unsigned char sector_size_low : 1;
  // byte 12
  unsigned char write_bl_len_high : 2;
  unsigned char r2w_factor : 3;
  unsigned char reserved3 : 2;
  unsigned char wp_grp_enable : 1;
  // byte 13
  unsigned char reserved4 : 5;
  unsigned char write_partial : 1;
  unsigned char write_bl_len_low : 2;
  // byte 14
  unsigned char reserved5: 2;
  unsigned char file_format : 2;
  unsigned char tmp_write_protect : 1;
  unsigned char perm_write_protect : 1;
  unsigned char copy : 1;
  /** Indicates the file format on the card */
  unsigned char file_format_grp : 1;
  // byte 15
  unsigned char always1 : 1;
  unsigned char crc : 7;
} __attribute__((packed)) csd1_t;
//==============================================================================
/**
 * \class CSDV2
 * \brief CSD register for version 2.00 cards.
 */
typedef struct CSDV2 {
  // byte 0
  unsigned char reserved1 : 6;
  unsigned char csd_ver : 2;
  // byte 1
  /** fixed to 0X0E */
  unsigned char taac;
  // byte 2
  /** fixed to 0 */
  unsigned char nsac;
  // byte 3
  unsigned char tran_speed;
  // byte 4
  unsigned char ccc_high;
  // byte 5
  /** This field is fixed to 9h, which indicates READ_BL_LEN=512 Byte */
  unsigned char read_bl_len : 4;
  unsigned char ccc_low : 4;
  // byte 6
  /** not used */
  unsigned char reserved2 : 4;
  unsigned char dsr_imp : 1;
  /** fixed to 0 */
  unsigned char read_blk_misalign : 1;
  /** fixed to 0 */
  unsigned char write_blk_misalign : 1;
  /** fixed to 0 - no partial read */
  unsigned char read_bl_partial : 1;
  // byte 7
  /** high part of card size */
  unsigned char c_size_high : 6;
  /** not used */
  unsigned char reserved3 : 2;
  // byte 8
  /** middle part of card size */
  unsigned char c_size_mid;
  // byte 9
  /** low part of card size */
  unsigned char c_size_low;
  // byte 10
  /** sector size is fixed at 64 KB */
  unsigned char sector_size_high : 6;
  /** fixed to 1 - erase single is supported */
  unsigned char erase_blk_en : 1;
  /** not used */
  unsigned char reserved4 : 1;
  // byte 11
  unsigned char wp_grp_size : 7;
  /** sector size is fixed at 64 KB */
  unsigned char sector_size_low : 1;
  // byte 12
  /** write_bl_len fixed for 512 byte sectors */
  unsigned char write_bl_len_high : 2;
  /** fixed value of 2 */
  unsigned char r2w_factor : 3;
  /** not used */
  unsigned char reserved5 : 2;
  /** fixed value of 0 - no write protect groups */
  unsigned char wp_grp_enable : 1;
  // byte 13
  unsigned char reserved6 : 5;
  /** always zero - no partial sector read*/
  unsigned char write_partial : 1;
  /** write_bl_len fixed for 512 byte sectors */
  unsigned char write_bl_len_low : 2;
  // byte 14
  unsigned char reserved7: 2;
  /** Do not use always 0 */
  unsigned char file_format : 2;
  unsigned char tmp_write_protect : 1;
  unsigned char perm_write_protect : 1;
  unsigned char copy : 1;
  /** Do not use always 0 */
  unsigned char file_format_grp : 1;
  // byte 15
  /** not used always 1 */
  unsigned char always1 : 1;
  /** checksum */
  unsigned char crc : 7;
} __attribute__((packed)) csd2_t;
//==============================================================================













/**
 * \class csd_t
 * \brief Union of old and new style CSD register.
 */
typedef union {
  csd1_t v1;
  csd2_t v2;
}csd_t;

//-----------------------------------------------------------------------------

/**
 * xx blocks = ret >> 10
 * xx mb = ret >> 11
 */
inline uint32_t sdCardCapacity(csd_t* csd) {
  if (csd->v1.csd_ver == 0) {
    uint8_t read_bl_len = csd->v1.read_bl_len;
    uint16_t c_size = (csd->v1.c_size_high << 10)
                      | (csd->v1.c_size_mid << 2) | csd->v1.c_size_low;
    uint8_t c_size_mult = (csd->v1.c_size_mult_high << 1)
                          | csd->v1.c_size_mult_low;
    return (uint32_t)(c_size + 1) << (c_size_mult + read_bl_len - 7);
  } else if (csd->v2.csd_ver == 1) {
    return (((uint32_t)csd->v2.c_size_high << 16) +
           ((uint16_t)csd->v2.c_size_mid << 8) + csd->v2.c_size_low + 1) << 10;
  } else {
    return 0;
  }
}


#define SPI_SD_ERR_R1_INVALID 	0xE0
#define SPI_SD_ERR_PARAM 		0xE1
#define SPI_SD_ERR_ADDR 		0xE2
#define SPI_SD_ERR_ERASE_SEQ 	0xE3
#define SPI_SD_ERR_CRC		 	0xE4
#define SPI_SD_ERR_CMD		 	0xE5
#define SPI_SD_ERR_ERASE_RES 	0xE6
#define SPI_SD_ERR_IDLE		 	0xE7
#define SPI_SD_ERR_CMD_TIMEOUT 	0xE8

#define SPI_SD_ERR_TKN_CRC	 	0xE9
#define SPI_SD_ERR_TKN_WRT	 	0xEA
#define SPI_SD_ERR_TKN_INVALID 	0xEB

#define SPI_SD_ERR_BUSY_TIMEOUT	0xEB

//#define SPI_SD_ERR_ERASE_SEQ 	0xE0
//#define SPI_SD_ERR_ERASE_SEQ 	0xE0










#endif /* GIIKERSPISD_H_ */
