/*
 * object_station_meteo_int.c
 *
 *  Created on: 02 fevr. 2021
 *      Author: Thibault.R
 */
#include "../config.h"

#if OBJECT_ID == OBJECT_STATION_METEO_INT

#include "object_station_meteo_int.h"
#include "../../bsp/nmos_gnd.h"
#include "../../bsp/dht11.h"
#include "../../bsp/bmp180.h"

void STATION_METEO_INT_MAIN(void) {
	typedef enum{
		INIT,
		DHT11,
		BMP180,
		OTHERS_MEASUREMENT,
		SEND_DATAS,
		EPAPER
	}state_e;

	static state_e state = INIT;
	switch(state){
	case INIT:
		state = DHT11;
		break;
	case DHT11:{
		NMOS_On();
		DHT11_main();
		NMOS_Off();
		state = BMP180;
		state = OTHERS_MEASUREMENT;
		break;}
	case BMP180:{
		NMOS_On();
		BMP180_demo();
		NMOS_Off();
		state = OTHERS_MEASUREMENT;
		break;}
	case OTHERS_MEASUREMENT:{
		state = SEND_DATAS;
		break;}
	case SEND_DATAS:{
		state = EPAPER;
		break;}
	case EPAPER:{
		debug_printf("DHT11 : (h=%d,%d | t=%d,%d)\n", humidity_int, humidity_dec, temperature_int, temperature_dec);
		state = INIT;
		break;}
	default:
		break;
	}
}

#endif
