/*
 * object_tracker_gps.h
 *
 *  Created on: 1 févr. 2021
 *      Author: fabie
 */

#ifndef APPLI_OBJECTS_OBJECT_TRACKER_GPS_H_
#define APPLI_OBJECTS_OBJECT_TRACKER_GPS_H_

typedef struct
{
	uint16_t 	id;
	uint32_t 	time;		//[HHMMSS]
	uint32_t 	seconds;	//[sec since 0:00:00]
	double 		latitude_rad;		//rad
	double 		longitude_rad;		//rad
	double		latitude_deg;		//deg
	double		longitude_deg;		//deg
	int16_t		lat_degrees;	//Partie entière des degrés
	double		lat_minutes;	//Minutes d'angle, avec décimales
	int16_t		long_degrees;	//Partie entière des degrés
	double		long_minutes;	//Minutes d'angle, avec décimales
	bool_e		north;
	bool_e 		east;
	uint16_t	ground_speed;
	uint32_t 	date32;
	uint8_t 	altitude_metre;
	uint8_t checksum;
}gps_datas_t;

typedef enum
{
	NO_TRAME_RECEIVED = 0,	//Une trame est en cours de réception
	CHECKSUM_INVALID,
	TRAME_INVALID,
	TRAME_UNKNOW,			//Une trame inconnue a été reçue
	TRAME_GPRMC,			//Une trame GPRMC a été reçue
	TRAME_GPGGA,				//Une trame GPGGA a été reçue
}nmea_frame_e;


typedef enum
{
	INIT = 0,	//Une trame est en cours de réception
	CONTAINER_TRAM,
	WAIT,
	SENT_CONTAINER_TRAM,
	STOP,

}tracker_gps_state;



void GPS_main(void);

void GPS_On(void);

void GPS_Off(void);

void GPS_test(void);

void GPS_process_rx(uint8_t c);

uint8_t calcul_delta_heure(uint8_t heure_a, uint8_t heure_b);

double gps_calcul_distance(double lat_a_rad, double lon_a_rad, double lat_b_rad, double lon_b_rad);


#endif /* APPLI_OBJECTS_OBJECT_TRACKER_GPS_H_ */
