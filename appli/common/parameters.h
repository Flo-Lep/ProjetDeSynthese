/*
 * parameters.h
 *
 *  Created on: 15 févr. 2021
 *      Author: meven
 */

#ifndef BSP_PARAMETERS_H_
#define BSP_PARAMETERS_H_
#include "macro_types.h"
#include "../config.h"

//Liste des paramètres, tout objets confondus.
typedef enum
{
	PARAM_UNKNOW = 0,
	PARAM_MY_BASE_STATION_ID,
	PARAM_ACTUATOR_STATE,
	PARAM_SENSOR_VALUE,
	PARAM_ALARM_TRESHOLD,
	PARAM_ALARM_WAY,
	PARAM_TEMPERATURE,
	PARAM_HYGROMETRY,
	PARAM_COLOR,
	PARAM_REFRESH_PERIOD,
	PARAM_PRESSURE,
	PARAM_BRIGHTNESS,
	PARAM_WINDSPEED,
	PARAM_PLUVIOMETRY,
	PARAM_SCREEN_COLOR,
	PARAM_MODE,

	PARAM_32_BITS_NB,	//avant ce define, tout les paramètres tiennent sur 32 bits.

	//Paramètres dont la taille dépasse 32 bits... et dont le traitement est spécifiquement confié à l'objet par des fonctions de callback.
	PARAM_TEXT_PART0,
	PARAM_TEXT_PART1,
	PARAM_TEXT_PART2,
	PARAM_TEXT_PART3,
	PARAM_TEXT_PART4,
	PARAM_TEXT_PART5,
	PARAM_TEXT_PART6,
	PARAM_TEXT_PART7,
	PARAM_NB
}param_id_e;

typedef enum
{
	ALARM_WAY_MAX,	//le seuil est un maximum à ne pas dépasser
	ALARM_WAY_MIN,	//le seuil est un minimum à ne pas dépasser
	ALARM_WAY_DIFF, //le seuil est une valeur à ne pas quitter (pour un capteur tout ou rien ... !)
}alarm_way_e;



void PARAMETERS_init(void);

//chaque objet doit appeler cette fonction pour chacun de ses paramètres
void PARAMETERS_enable(param_id_e param_id, int32_t default_value, bool_e value_saved_in_flash, callback_fun_i32_t callback_after_set_from_RF, callback_i32_fun_t callback_if_get_from_RF);

//cette fonction est appelée soit par l'objet qui met à jour une valeur (accessible sur demande par la station)
//  soit suite à la station qui met à jour une valeur (accessible sur demande de l'objet)
void PARAMETERS_update(param_id_e param_id, int32_t new_value);

void PARAMETERS_update_custom(param_id_e param_id, uint8_t * datas);

void PARAMETERS_restore_from_flash(void);

void PARAMETERS_read_from_flash(param_id_e param_id);

//permet de récupérer la valeur d'un paramètre
int32_t PARAMETERS_get(param_id_e param_id);




#endif /* BSP_PARAMETERS_H_ */
