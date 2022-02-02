/*
 * object_out_weather_station.c
 *
 *  Created on: 2 févr. 2021
 *      Author: terri
 */

#include "../config.h"
#include "object_out_weather_station.h"

#if OBJECT_ID == OBJECT_OUT_WEATHER_STATION


#include "../../bsp/dht11.h"
#include "../../bsp/bmp180.h"
#include "../../bsp/nmos_gnd.h"
#include "../common/gpio.h"

void OUT_WEATHER_STATION_MAIN(void){
	typedef enum{
		INIT,
		RAIN_WAITING,
		RAIN_MEASUREMENT,
		OTHERS_MEASUREMENT,
		SEND_DATAS
	}state_e;

	static state_e state = INIT;
	switch(state){
	case INIT:
		RJ12_WindInit();
		RJ12_RainInit();
//		BMP180_Init();
		DHT11_init(DHT11_PIN);
		state = RAIN_WAITING;
		break;
	case RAIN_WAITING:{
		if(!GPIO_read(PIN_ANEMO_MOINS)){
			state = RAIN_MEASUREMENT;
		}
		//TODO si on demande les autres données --> state = OTHERS_MEASUREMENT
		state = OTHERS_MEASUREMENT;
		break;}
	case RAIN_MEASUREMENT:{
		//TODO on mesure la quantité de pluie
		//TODO si on demande les autres données --> state = OTHERS_MEASUREMENT
		state = OTHERS_MEASUREMENT;
		break;}
	case OTHERS_MEASUREMENT:{
		NMOS_On();
		//BMP180_StartTemperature();
		//BMP180_ReadTemperature();
		DHT11_main();
		NMOS_Off();
		RJ12_ReadWindTest();
		state = SEND_DATAS;
		break;}
	case SEND_DATAS:{
		//Communication avec la station de base
		state = INIT;
		break;}
	default:
		break;
	}
}


void RJ12_WindInit(void){
	GPIO_configure(PIN_ANEMO_MOINS, NRF_GPIO_PIN_PULLUP, FALSE);
	GPIO_configure(PIN_ANEMO_PLUS, NRF_GPIO_PIN_NOPULL, TRUE);
}


float RJ12_ReadWindTest(void){
	RJ12_WindInit();
	uint32_t time = SYSTICK_get_time_us();
	uint32_t time2 = 0;
	GPIO_write(PIN_ANEMO_PLUS, TRUE);
	bool_e read = 0;
	volatile uint8_t tour_in_10_seconds = 0;
	while(time2 < time + 10000000 ){
		read = GPIO_read(PIN_ANEMO_MOINS);
		time2 = SYSTICK_get_time_us();
		if(read == 1){
			tour_in_10_seconds++;
		}
	}
	volatile float vitesse_en_ms = 2*3.14*0.07*(tour_in_10_seconds)/10; // Vitesse en m/s
	volatile float vitesse_en_kmh = 3.6*vitesse_en_ms; //Vitesse en km/h
	return vitesse_en_kmh;
}


void RJ12_RainInit(void){
	GPIO_configure(PIN_PLUVIO_MOINS, NRF_GPIO_PIN_PULLUP, FALSE);
	GPIO_configure(PIN_PLUVIO_PLUS, NRF_GPIO_PIN_NOPULL, TRUE);
}


uint8_t RJ12_ReadRainTest(void){
	RJ12_RainInit();
	uint32_t time = SYSTICK_get_time_us();
	uint32_t time2 = 0;
	GPIO_write(PIN_PLUVIO_PLUS, TRUE);
	volatile bool_e read = 0;
	volatile uint8_t dose_pluie = 0;
	while(time2 < time + 10000000 ){
		read = GPIO_read(PIN_PLUVIO_MOINS);
		time2 = SYSTICK_get_time_us();
		if(read == 1){
			dose_pluie++;
		}
	}
	return dose_pluie;
}





#endif
