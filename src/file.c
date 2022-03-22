/*
 * file.c
 *
 *  Created on: 2022-3-21
 *      Author: Jason
 */


#include "file.h"







#include "giikerSpiSd.h"


#include "nrf_delay.h"

#include "ff.h"

#include "giikerConfig.h"


#include "giikerLog.h"

#include "pin_debuger.h"




/******************** Raw **********************/

static uint8_t testData[600];

void sectionRWTest(){

	uint8_t err = 0;

    for (uint16_t i = 0; i < 520; i++) {
		testData[i] = i;
	}
    err = spiSdWriteSingleBlock(0, testData);
    print("write err: %x", err);

//    for (uint16_t i = 0; i < 520; i++) {
//		testData[i] = 0x55;
//	}
//    err = spiSdWriteSingleBlock(1, testData);
//    print("write err: %x", err);

    err = spiSdReadSingleBlock(0, testData);
    print("read err: %x", err);
    if (err == 0) {
    	for (uint8_t i = 0; i < 10; i++) {
			print("[%x] = %x", i, testData[i]);
		}
		print("");
    }

}



/******************** LS **********************/

char sizeString[10];

char* fileSizeString(FSIZE_t size){
	uint32_t sizeHigh;
	sizeHigh = size;
	if(sizeHigh >= (1 << 30)){
		sizeHigh = sizeHigh >> 20;
        sprintf(sizeString, "%d.%dGB", sizeHigh / 1000, sizeHigh % 1000 / 100);
	}else if(sizeHigh >= (1 << 20)){
		sizeHigh = sizeHigh >> 10;
        sprintf(sizeString, "%d.%dMB", sizeHigh / 1000, sizeHigh % 1000 / 100);
	}else if(sizeHigh >= (1 << 10)){
		sizeHigh = sizeHigh;
        sprintf(sizeString, "%d.%dKB", sizeHigh / 1000, sizeHigh % 1000 / 100);
	}else{
        sprintf(sizeString, "%d B", sizeHigh);
	}
	return sizeString;
}

FRESULT scan_files(char* path) {
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;



    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) {  /* Break on error or end of dir */
            	break;
            }
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                i = strlen(path);

                sprintf(&path[i], "/%s", fno.fname);

                res = scan_files(path);                    /* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
                print("%s\t %s/%s", fileSizeString(fno.fsize), path, fno.fname);
            }
        }
        f_closedir(&dir);
    }

    return res;
}

void listFile(){
    FATFS fs;
    FRESULT res;
    char buff[256];
    res = f_mount(&fs, "", 1);
    if (res == FR_OK) {
        strcpy(buff, "/");
        res = scan_files(buff);
    }
}



/******************** Read **********************/

void fsReadTxt(char * name){
    FATFS  fs1;      /* Work area (filesystem object) for logical drives */
    FIL  fdst;      /* File objects */
    FRESULT fr;          /* FatFs function common result code */
    char line[82]; /* Line buffer */

    /* Give work areas to each logical drive */
    f_mount(&fs1, "", 1);

    /* Create destination file on the drive 0 */
    fr = f_open(&fdst, name, FA_READ);

    print("f_open: %x", fr);

    while (f_gets(line, sizeof line, &fdst)) {
		print(line);
	}

    f_close(&fdst);
}





/******************** Write **********************/

void fsWriteTxt(char * name){
    FATFS  fs1;      /* Work area (filesystem object) for logical drives */
    FIL  fdst;      /* File objects */
    FRESULT fr;          /* FatFs function common result code */

    /* Give work areas to each logical drive */
    fr = f_mount(&fs1, "", 1);
    print("f_mount: %x", fr);

    /* Create destination file on the drive 0 */
    fr = f_open(&fdst, name, FA_WRITE | FA_CREATE_ALWAYS);
    print("f_open: %x", fr);

    for (uint8_t i = 0; i < 20; i++) {
        fr = f_printf(&fdst, "%d : 888\r\n", i);
//        print("%d: f_printf: %x", i, fr);
    }

    fr = f_sync(&fdst);
    print("f_sync: %x", fr);
    fr = f_close(&fdst);
    print("f_close: %x", fr);
}

void fsAppendTxt(char * name){
    FATFS  fs1;      /* Work area (filesystem object) for logical drives */
    FIL  fdst;      /* File objects */
    FRESULT fr;          /* FatFs function common result code */

    /* Give work areas to each logical drive */
    f_mount(&fs1, "", 1);

    /* Create destination file on the drive 0 */
    fr = f_open(&fdst, name, FA_WRITE | FA_OPEN_APPEND);
    print("f_open: %x", fr);

    for (uint16_t i = 0; i < 30; i++) {
        fr = f_printf(&fdst, "%d : 666\r\n", i);
    }

    fr = f_sync(&fdst);
    print("f_sync: %x", fr);
    fr = f_close(&fdst);
    print("f_close: %x", fr);
}













void fileOp(){

    print("***************** LS *****************");
	listFile();

//    print("***************** Write *****************");
//	fsWriteTxt("0:6.txt");
//    print("***************** Append *****************");
//	fsAppendTxt("0:6.txt");
//
    print("***************** Read *****************");
	fsReadTxt("0:6.txt");

    print("***************** ok *****************");
}







