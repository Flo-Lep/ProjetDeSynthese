/*
 * object_station_meteo_int.h
 *
 *  Created on: 02 fevr. 2021
 *      Author: Thibault.R
 */

#ifndef OBJECT_STATION_METEO_INT_H_
#define OBJECT_STATION_METEO_INT_H_

#include "../config.h"
#include "../../bsp/nmos_gnd.h"
#include "../../bsp/dht11.h"
#include "../../bsp/bmp180.h"


#if OBJECT_ID == OBJECT_STATION_METEO_INT

extern uint8_t humidity_int;
extern uint8_t humidity_dec;
extern uint8_t temperature_int;
extern uint8_t temperature_dec;

void STATION_METEO_INT_MAIN(void);

#endif /* OBJECT_STATION_METEO_INT_H_ */
#endif

