/*
 * object_wine_degustation.c
 *
 *  Created on: 8 févr. 2021
 *      Author: lebihata
 */
#include "../config.h"
#if OBJECT_ID == OBJECT_WINE_DEGUSTATION

#include "bsp/nrf52_i2c.h"

#include <stdio.h>
#include "object_wine_degustation.h"
#include "../common/leds.h"
#include "../common/battery.h"
#include "../common/serial_dialog.h"
#include "../common/parameters.h"



uint8_t wd_reg[2];
uint8_t wd_write;
uint8_t wd_read;
uint8_t temp_wd;
int prctBatt;

void MCP9804_write(uint8_t reg, uint16_t value)
{
	uint8_t wd_datas[3];
	wd_datas[0] = reg;
	wd_datas[1] = HIGHINT(value);
	wd_datas[2] = LOWINT(value);
	while(I2C_write(wd_datas, 3)==IN_PROGRESS);
}

void MCP9804_read(uint8_t reg, uint16_t * value)
{
	uint8_t wd_datas[2];
	while(I2C_write(&reg, 1)==IN_PROGRESS);
	while(I2C_read(wd_datas, 2)==IN_PROGRESS);
	*value = U16FROMU8(wd_datas[0], wd_datas[1]);
}

void Wine_Degustation_Main(void) {

	//init battery
	MEASURE_VBAT_init();
	prctBatt= MEASURE_VBAT_get_level();
	debug_printf("La batterie est à %u", prctBatt,"%.") ;

	//initialisation led verte et jaune
	LEDS_init(I_HAVE_LED_BATTERY);

	LED_add(LED_ID_USER0, PIN_LED_VERTE);
	LED_add(LED_ID_USER1, PIN_LED_JAUNE);

	LED_set(LED_ID_USER0, LED_MODE_BLINK);
	LED_set(LED_ID_USER1, LED_MODE_BLINK);

	SERIAL_DIALOG_init();
	callback_fun_i32_t callback;


	//configuration registre du mcp9804 via I2C
	I2C_init(0x18);


	/*wd_datas=&wd_device_address;
	wd_reg=wd_device_address[1];



	//ecrire dans un registre
	while(wd_write!=1){
		wd_write=
	}
	//lire dans un registre
	while(wd_read!=1){
		I2C_write(wd_datas, 1);
		wd_read=I2C_read(wd_reg, 2);
	}

	PARAMETERS_enable(PARAM_TEMPERATURE, 0xC, TRUE, callback, NULL);

*/
	while(1)
	{
		uint16_t value;
		MCP9804_read(0x05, &value);	//Temperature register
		debug_printf("t=%d\n",value);
		SYSTICK_delay_ms(100);
	}


	/*//also, make sure bit 0 is Set ‘1’
	int UpperByte;
	uint8_t* ACK;
	UpperByte = I2C_read(ACK,1); // READ 8 bits
	//and Send ACK bit
	int LowerByte;
	uint8_t* NAK;
	LowerByte = I2C_read(NAK,1); // READ 8 bits
	//and Send NAK bit

	//Convertire la donnée du capteur en degres Celsus (°C)
	//First Check flag bits
	if ((UpperByte & 0x80) == 0x80){ //TA ≥ TCRIT
	}
	if ((UpperByte & 0x40) == 0x40){ //TA > TUPPER
	}
	if ((UpperByte & 0x20) == 0x20){ //TA < TLOWER
	}
	UpperByte = UpperByte & 0x1F; //Clear flag bits

	if ((UpperByte & 0x10) == 0x10){ //TA < 0°C
		UpperByte = UpperByte & 0x0F; //Clear SIGN
		temp_wd = 256 - (UpperByte*16 + LowerByte / 16);
	}
	else {  //TA ≥ 0°C
		temp_wd = (UpperByte*16 + LowerByte / 16);
	}
	// exemple d'assistance de dégustation avec un Reuilly "pinot gris" entre 11 et 13 °C
	if( temp_wd >=11 && temp_wd <=13){
		LED_set(LED_ID_USER0, LED_MODE_OFF);
		LED_set(LED_ID_USER1, LED_MODE_ON);
	}
	else if(temp_wd <11){
		LED_set(LED_ID_USER0, LED_MODE_BLINK);
		LED_set(LED_ID_USER1, LED_MODE_OFF);
	}
	else if(temp_wd >13){
		LED_set(LED_ID_USER0, LED_MODE_ON);
		LED_set(LED_ID_USER1, LED_MODE_OFF);
	}*/

}

#endif
