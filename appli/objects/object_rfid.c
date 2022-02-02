/*
 * object_rfid.c
 *
 *  Created on: 16 févr. 2021
 *      Author: paulq
 */



#include "../config.h"
#include "object_rfid.h"
#include "nrfx_spi.h"
#include "bsp/RC522.h"



#if OBJECT_ID == OBJECT_RFID


static uint8_t Test[64];
static uint8_t status;

void object_rfid_process_main(void){

	mfrc522_init();
	status = mfrc522_get_card_serial(Test);
}



#endif
