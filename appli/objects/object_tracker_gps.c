/*
 * object_tracker_gps.c
 */
#include "../config.h"



#if OBJECT_ID == OBJECT_TRACKER_GPS

#include "appli/common/gpio.h"
#include "object_tracker_gps.h"

#include <math.h>
#include <stdlib.h>
//Constantes privï¿½es
#define BUFFER_SIZE	256

/** \brief NMEA message start-of-message (SOM) character */
#define NMEA_MESSAGE_SOM '$'

/** \brief NMEA message end-of-message (EOM) character */
#define NMEA_MESSAGE_EOM '*'

/** \brief NMEA message field separator */
#define NMEA_MESSAGE_FIELD_SEPARATOR ','

#define PI                     (3.141592653589793)        /**< PI value */
#define PI180                  (PI / 180)                 /**< PI division by 180 */
#define EARTHRADIUS_KM         (6378)                     /**< Earth's mean radius in km */
#define EARTHRADIUS_M          (EARTHRADIUS_KM * 1000)    /**< Earth's mean radius in m */

#define WAIT_TIME              10 //temps d'attente avant d'enregistrer un nouveau point
#define DISTANCE               1 // distance a atteindre avant d'enregistrer un nouveau point

//Fonctions privï¿½es
static nmea_frame_e GPS_parse(uint8_t * buffer, gps_datas_t * gps_datas);
static bool_e GPS_parse_gprmc(uint8_t * string, gps_datas_t * coordinates);
static uint8_t hextoint(char c);
	/*
 * Cette fonction rï¿½cupï¿½re le nouveau caractï¿½re fourni (c) et le range dans son propre buffer
 * Si l'on atteint le dï¿½but de la trame ($) -> on reset l'index de lecture
 * Lorsqu'on atteint la fin de la trame (dï¿½tection d'un caractï¿½re '\n') -> on sous-traite le dï¿½coupage de la trame au parser.
 * Une trame correctement lue donne lieu au remplissage de la structure gps_datas et au renvoi d'une valeur de retour diffï¿½rente de NO_TRAME_RECEIVED (0)
 */

void GPS_main(void)
{
	static tracker_gps_state state = INIT;
		switch(state)
				{
					case INIT:
						// initialisation du gps

						debug_printf("init");
						LED_add(LED_ID_BATTERY, PIN_LED_BATTERY);
						LED_add(LED_ID_NETWORK, PIN_LED_NETWORK);
						LED_set(LED_ID_BATTERY, LED_MODE_ON);
						LED_set(LED_ID_NETWORK, LED_MODE_ON);

						GPS_On();
						Systick_init();
						SECRETARY_init();


						BUTTONS_init();
						BUTTONS_add(BUTTON_NETWORK, PIN_BUTTON_NETWORK, TRUE, &BUTTONS_network_process);


						state = CONTAINER_TRAM;
						break;
					case CONTAINER_TRAM: //recup곡tion des marker gps et stockage
						LED_set_flash_limited_nb(LED_ID_NETWORK, 3, 500);
						SERIAL_DIALOG_set_rx_callback(&GPS_process_rx);

						if(BUTTONS_read(BUTTON_NETWORK) == TRUE)
							state = SENT_CONTAINER_TRAM;

						break;
					case SENT_CONTAINER_TRAM: // envoie des donnꥠvers la base
						LED_set(LED_ID_NETWORK, LED_MODE_FLASH);
						// test d'envoie message en uart

						break;
					case STOP: //arret de l'utilisation du module gps
						GPS_Off();
						break;
					default:
						break;
				}
}
void GPS_On(void)
{
	GPIO_configure(MOSFET_GND_GPS, NRF_GPIO_PIN_NOPULL, true);//configure la pin de du gps concernée en sortie
	GPIO_write(MOSFET_GND_GPS, true);
}
void GPS_Off(void)
{
	GPIO_configure(MOSFET_GND_GPS, NRF_GPIO_PIN_NOPULL, true);//configure la pin de du gps concern�e en sortie
	GPIO_write(MOSFET_GND_GPS, false);
}

static gps_datas_t gps_datas;
static double gps_lat[32];
static double gps_long[32];
static double gps_date[24];
static double gps_heure[24];

static uint8_t y = 0;
static double lat_a_rad = 0;
static double lon_a_rad = 0;

static double lat_b_rad = 0;
static double lon_b_rad = 0;
static uint8_t compteur = 0;

static uint8_t heure_a = 0;
static uint8_t heure_b = 0;

void GPS_process_rx(uint8_t c)
{
	static uint8_t buffer[BUFFER_SIZE];
	static uint16_t index = 0;
	if(c == '$')
		index = 0;

	buffer[index] = c;

	if(index<BUFFER_SIZE-1)
		index++;
	if(c=='\n')
	{
	buffer[index] = '\0';

		index = 0;
		//trame termin�e, on l'envoie !
		if(GPS_parse(buffer, &gps_datas) == TRAME_GPRMC)
		{
		//lorsqu'une trame compl鵥 et valide a 굩 re趥, on peut traiter les donn꦳ interpret꦳.

		if(compteur %2 == 0) // tout les paire on stock la coordonnꥠdans a
			{
				lat_a_rad = gps_datas.latitude_rad;
				lon_a_rad = gps_datas.longitude_rad;
				heure_a = gps_datas.time;
			}

		if(compteur %2 == 1) // tout les impaire on stock la coordonnꥠdans b
			{
				lat_b_rad = gps_datas.latitude_rad;
				lon_b_rad = gps_datas.longitude_rad;
				heure_b = gps_datas.time;
			}

		compteur++;

		double distance = 0;
		distance = gps_calcul_distance(lat_a_rad, lon_a_rad, lat_b_rad, lon_b_rad);

		uint8_t delta_heure = 0;
		delta_heure = calcul_delta_heure(heure_a, heure_b);

		if(distance > DISTANCE && delta_heure < WAIT_TIME)
		{

			gps_lat[y] = gps_datas.latitude_rad;      // voir s'il faut mettre en degre pour le site
			gps_long[y] = gps_datas.longitude_rad;
			gps_date[y] =  gps_datas.date32;
			gps_heure[y] =  gps_datas.time;
			//debug_printf("%lf\n",gps_lat[y]);
			y++;

			}else // cas ou la distance n'est pas assez grande, on viens redonner l'ancienne valeur pour garder un bon referenciel
			{
				if(compteur %2 == 0)
					{
						lat_a_rad = lat_b_rad;
						lon_a_rad = lon_b_rad;
						heure_a = heure_b;
					}
				if(compteur %2 == 1)
					{
						lat_b_rad = lat_a_rad;
						lon_b_rad = lon_a_rad;
						heure_a = heure_b;
					}

			}

		}

	}

}

uint8_t calcul_delta_heure(uint8_t heure_a, uint8_t heure_b)
{
	if (heure_a > heure_b)
		return heure_a - heure_b;
	if(heure_b > heure_a)
		return heure_b - heure_a;
	if(heure_a == heure_b)
		return 0;
}

double gps_calcul_distance(double old_lat, double old_lon, double new_lat, double new_lon)
{
	double latRad, lonRad;
	double tlatRad, tlonRad;
	double midLat, midLon;
	double dist = 0.0;


  //convertion des valeures du degree vers le radian
  latRad = old_lat* 0.017453293;
  lonRad = old_lon* 0.017453293;
  tlatRad = new_lat * 0.017453293;
  tlonRad = new_lon * 0.017453293;

  midLat = tlatRad - latRad;
  midLon = tlonRad - lonRad;

  //Calcule de la distance en Km
  double latSin = sin((latRad - tlatRad)/2);
  double lonSin = sin((lonRad - tlonRad)/2);

  dist = 2 * asin(sqrt((latSin*latSin) + cos(latRad) * cos(tlatRad) * (lonSin * lonSin)));

  dist = dist * EARTHRADIUS_KM; // pour la distance en Km il faut multiplier la valeure trouv�e par le rayon de la terre

  return dist;

}



/*
 * Fonction permettant de savoir si la chaine string commence par le contenu de la chaine begin
 * Renvoi vrai si c'est le cas, faux sinon.
 */
static bool_e string_begins_with(uint8_t * string, uint8_t * begin)
{
	bool_e b;
	uint16_t i;
	b = TRUE;	//On fait l'hypothï¿½se que tout se passe bien...
	for(i=0;begin[i];i++)
	{
		if(string[i] == '\0' || (string[i] != begin[i]))
		{
			b = FALSE;
			break;
		}
	}
	return b;
}

/*
 * Cette fonction a pour but de :
 * - vï¿½rifier si la trame founie dans "buffer" commence par un identifiant connu (GPRMC, ...)
 * - vï¿½rifier la valeur du checksum
 * - sous-traiter la lecture de la trame reconnue ï¿½ la bonne fonction
 * Si le checksum est faux -> renvoit CHECKSUM_INVALID
 * Si la trame n'est pas reconnue -> renvoit NO_TRAME_RECEIVED
 * Si la trame est jugï¿½e invalide par les fonctions sous-traitantes -> renvoit TRAME_INVALID
 */
static nmea_frame_e GPS_parse(uint8_t * buffer, gps_datas_t * gps_datas)
{
	const char string_gprmc[] = "$GPRMC";
	const char string_gpgga[] = "$GPGGA";
	nmea_frame_e ret;
	Uint8 i, checksum;
	ret = NO_TRAME_RECEIVED;	//On fait l'hypothï¿½se qu'aucune trame correcte est reï¿½ue

	if(string_begins_with(buffer, (uint8_t *)string_gprmc))
		ret = TRAME_GPRMC;
	if(string_begins_with(buffer, (uint8_t *)string_gpgga))
			ret = TRAME_GPGGA;

	//TODO ajouter d'autres trames si besoin..

	if(ret != NO_TRAME_RECEIVED)	//un entï¿½te connu a ï¿½tï¿½ trouvï¿½... calcul du checksum
	{
		checksum = 0;
		for(i=1; buffer[i]!='*' && buffer[i] != '\0'; i++)
		{
			checksum ^= buffer[i];
		}
		if(! (buffer[i] == '*' && (16*hextoint(buffer[i+1]) + hextoint(buffer[i+2]) == checksum)))
			ret = CHECKSUM_INVALID;	//Trame invalide : pas de checksum *XX ou checksum incorrect
	}

	switch(ret)
	{
		case TRAME_GPRMC:
			if(!GPS_parse_gprmc(buffer, gps_datas))
				ret = TRAME_INVALID;
			break;
		case TRAME_GPGGA:
			if(!GPS_parse_gpgga(buffer, gps_datas))
				ret = TRAME_INVALID;
			break;
		default:
			break;
	}

	return ret;
}



/*
 * Cette fonction dï¿½coupe une trame GPRMC fournie dans string et remplit la structure coordinates
 * - Si la trame est invalide (notamment lorsque le GPS ne capte pas !) -> renvoit FALSE
 * - Si la trame est valide -> renvoit TRUE
 * Attention, cette fonction ï¿½crase la chaine passï¿½e en remplacant notamment certains caractï¿½res ',' par des '\0' !!!
 */
bool_e GPS_parse_gprmc(uint8_t * string, gps_datas_t * coordinates)
{
	uint8_t i;
	uint8_t *message_field[14] = {'\0'};	//tableau des pointeur sur champ
	//tableau des pointeurs sur champ dï¿½cimal
	//Header : $GPRMC
	//Data : 											,hhmmss.sss,A,ddmm.mmmm,N,ddmm.mmmm,W,X,X,ddmmyy,X,X,X*<CR><LF>
	//Chaine final :							\0		 hhmmss . sss \0 A \0 ddmm . mmmm \0 N \0 ddmm . mmmm \0 W \0 X \0 X \0 ddmmyy \0 X \0 X \0 X \0 <CR><LF>
	//indice de tableau dans la chaine : F0S0 = \0 ensuite F1	       \0 F2 \0 F3          \0 F4 \ F5           \0 F6\0 F7\0 F8\0 F9	 \0F10\0F11\0F12\0 F13

	i = 0;
	message_field[i] = string;
	while (*string != '\0' && *string != '\r' && *string != '\n')
	{
		if ((*string == NMEA_MESSAGE_FIELD_SEPARATOR) || (*string == NMEA_MESSAGE_EOM) )
		{
			// save position of the next token
			if(i<13)
				message_field[++i] = string + 1;
			*string = '\0';	// terminate string after field separator or end-of-message characters
		}
		string++;
	}

	if(i>2 && *message_field[2] == 'A')
	{
		coordinates->time =  (uint32_t)atoi((char*)message_field[1]); //conversion 32 bits
		coordinates->seconds = 	  ((uint32_t)(message_field[1][0] - '0')) * 36000
								+ ((uint32_t)(message_field[1][1] - '0')) * 3600
								+ ((uint32_t)(message_field[1][2] - '0')) * 600
								+ ((uint32_t)(message_field[1][3] - '0')) * 60
								+ ((uint32_t)(message_field[1][4] - '0')) * 10
								+ ((uint32_t)(message_field[1][5] - '0'));

		coordinates->north	= (message_field[4][0] == 'S')?FALSE:TRUE;
		coordinates->east	= (message_field[6][0] == 'W')?FALSE:TRUE;

		coordinates->lat_minutes = atof((char*)message_field[3]);
		coordinates->lat_degrees = (int16_t)(trunc(coordinates->lat_minutes));
		coordinates->lat_minutes -= (float)((coordinates->lat_degrees/100)*100);
		coordinates->lat_degrees = coordinates->lat_degrees/100;
		coordinates->latitude_deg = (double)coordinates->lat_degrees + coordinates->lat_minutes/60;
		if(coordinates->north==0)
			coordinates->latitude_deg*=-1;
		coordinates->latitude_rad = coordinates->latitude_deg * PI180;	//--> radians !

		coordinates->long_degrees = (int16_t)atoi((char*)message_field[5]);
		coordinates->long_minutes = atof((char*)message_field[5]);
		coordinates->long_minutes -= (float)((coordinates->long_degrees/100)*100);
		coordinates->long_degrees = coordinates->long_degrees/100;
		coordinates->longitude_deg = (double)coordinates->long_degrees + coordinates->long_minutes/60;
		if(coordinates->east==0)
			coordinates->longitude_deg*=-1;
		coordinates->longitude_rad = coordinates->longitude_deg * PI180;	//--> radians !

		coordinates->ground_speed =  (uint16_t)atoi((char*)message_field[7]);

		uint32_t current_date;
		current_date =  (uint32_t)atoi((char*)message_field[9]);
		coordinates->date32 = 	(	((Uint32)(current_date%100) + 20) << 25 ) 	//20 est la diffï¿½rence entre 2000 et 1980.
				| 	((Uint32)((current_date/100)%100) << 21 )
				| 	((Uint32)(current_date/10000) << 16 )
				| 	((Uint32)(coordinates->time/10000) << 11 )
				| 	((Uint32)((coordinates->time/100)%100) << 5 )
				| 	((Uint32)(coordinates->time%100) >> 1 ) ;

		return TRUE;
	}
	return FALSE;
}

bool_e GPS_parse_gpgga(uint8_t * string, gps_datas_t * coordinates)
{
	uint8_t i;
	uint8_t *message_field[14] = {'\0'};	//tableau des pointeur sur champ
	//tableau des pointeurs sur champ d�cimal
	//Header : $GPGGA
	/*
	 * $GPGGA,064036.289,4836.5375,N,00740.9373,E,1,04,3.2,200.2,M,,,,0000*0E
	$GPGGA       : Type de trame
	064036.289   : Trame envoy�e � 06 h 40 min 36 s 289 (heure UTC)
	4836.5375,N  : Latitude 48,608958� Nord = 48� 36' 32.25" Nord
	00740.9373,E : Longitude 7,682288� Est = 7� 40' 56.238" Est
	1            : Type de positionnement (le 1 est un positionnement GPS)
	04           : Nombre de satellites utilis�s pour calculer les coordonn�es
	3.2          : Pr�cision horizontale ou HDOP (Horizontal dilution of precision)
	200.2,M      : Altitude 200,2, en m�tres
	,,,,,0000    : D'autres informations peuvent �tre inscrites dans ces champs
	*0E          : Somme de contr�le de parit�, un simple XOR sur les caract�res entre $ et *3
	*/

	//Data : 											,hhmmss.sss,ddmm.mmmm,N,ddmm.mmmm,E,X,X,X,ddmm.mmmm,M,X,X,X*<CR><LF>
	//Chaine final :							\0		 hhmmss . sss \0 ddmm . mmmm \0 N \0 ddmm . mmmm \0 E \0 X \0 X \0 X \0 ddmm . mmmm \0 M \0 X \0 X \0 X \0 <CR><LF>
	//indice de tableau dans la chaine : F0S0 = \0 ensuite F1	       \0 F2 \0 F3          \0 F4 \ F5           \0 F6\0 F7\0 F8\0 F9	 \0F10\0F11\0F12\0 F13

	i = 0;
	message_field[i] = string;
	while (*string != '\0' && *string != '\r' && *string != '\n')
	{
		if ((*string == NMEA_MESSAGE_FIELD_SEPARATOR) || (*string == NMEA_MESSAGE_EOM) )
		{
			// save position of the next token
			if(i<14)
				message_field[++i] = string + 1;
			*string = '\0';	// terminate string after field separator or end-of-message characters
		}
		string++;
	}

	if(i>2 && *message_field[2] == 'A')
	{
		coordinates->time =  (uint32_t)atoi((char*)message_field[1]); //conversion 32 bits
		coordinates->seconds = 	  ((uint32_t)(message_field[1][0] - '0')) * 36000
								+ ((uint32_t)(message_field[1][1] - '0')) * 3600
								+ ((uint32_t)(message_field[1][2] - '0')) * 600
								+ ((uint32_t)(message_field[1][3] - '0')) * 60
								+ ((uint32_t)(message_field[1][4] - '0')) * 10
								+ ((uint32_t)(message_field[1][5] - '0'));

		coordinates->north	= (message_field[3][0] == 'S')?FALSE:TRUE;
		coordinates->east	= (message_field[5][0] == 'W')?FALSE:TRUE;

		coordinates->lat_minutes = atof((char*)message_field[2]);
		coordinates->lat_degrees = (int16_t)(trunc(coordinates->lat_minutes));
		coordinates->lat_minutes -= (float)((coordinates->lat_degrees/100)*100);
		coordinates->lat_degrees = coordinates->lat_degrees/100;
		coordinates->latitude_deg = (double)coordinates->lat_degrees + coordinates->lat_minutes/60;
		if(coordinates->north==0)
			coordinates->latitude_deg*=-1;
		coordinates->latitude_rad = coordinates->latitude_deg * PI180;	//--> radians !

		coordinates->long_degrees = (int16_t)atoi((char*)message_field[4]);
		coordinates->long_minutes = atof((char*)message_field[4]);
		coordinates->long_minutes -= (float)((coordinates->long_degrees/100)*100);
		coordinates->long_degrees = coordinates->long_degrees/100;
		coordinates->longitude_deg = (double)coordinates->long_degrees + coordinates->long_minutes/60;
		if(coordinates->east==0)
			coordinates->longitude_deg*=-1;
		coordinates->longitude_rad = coordinates->longitude_deg * PI180;	//--> radians !

		coordinates->altitude_metre =  (double)atoi((char*)message_field[9]);


		return TRUE;
	}
	return FALSE;
}

//converti un caractï¿½re hexa (par exemple '4', ou 'B') en un nombre (dans cet exemple : 4, 11)
static uint8_t hextoint(char c)
{
	if(c >= 'A' && c <= 'F')
		return (uint8_t)(c - 'A' + 10);
	if(c >= 'a' && c <= 'f')
		return (uint8_t)(c - 'a' + 10);
	if(c >= '0' && c <= '9')
		return (uint8_t)(c - '0');
	return 0;
}


#endif
