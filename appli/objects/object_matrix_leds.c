/*
 * object_matrix_leds.c
 *
 *  Created on: 2 févr. 2021
 *      Author: meven
 */
#include "../config.h"
#include "object_matrix_leds.h"
#include "bsp/matrix_leds_32x32.h"
#include "appli/common/parameters.h"


#if OBJECT_ID == OBJECT_MATRIX_LEDS

static volatile uint32_t value = 0;
static volatile param_id_e param_id = PARAM_TEMPERATURE;

void OBJECT_MATRIX_LEDS_value_updated_callback(uint32_t new_value){
	value = new_value;
}

//En commentaire une version du code affichant une donnée en brut

void MATRIX_afficheur(uint32_t colorDonnees, uint32_t colorType){
	typedef enum{
		INIT_DATA,
		DISPLAY_MATRIX
	}state_e;

	static state_e state = INIT_DATA;
	static matrix_t matrix[32][32];
	static uint32_t pre_value = -1;

	switch(state){
	case INIT_DATA:
		PARAMETERS_enable(PARAM_TEMPERATURE, 0, TRUE, &OBJECT_MATRIX_LEDS_value_updated_callback, NULL);
		MATRIX_init();
		state = DISPLAY_MATRIX;
		break;
	case DISPLAY_MATRIX:{
		if(value != pre_value){
			MATRIX_reset(matrix);
			bool_e positif = TRUE;
			if(value < 0)
				positif = FALSE;
			MATRIX_show_value(matrix, value, positif, colorDonnees, param_id, colorType);
			pre_value = value;
		}
		MATRIX_display(matrix);
		break;
	}
	default:
		break;
	}
}

#endif



