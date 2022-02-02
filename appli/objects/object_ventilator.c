/*
 * object_ventilator.c
 *
 *  Created on: 10 févr. 2021
 *      Author: Utilisateur
 */
#include "../config.h"


#if OBJECT_ID == OBJECT_VENTILATOR
#include "../common/gpio.h"
#include "nrf_gpio.h"
#include "nrf52.h"
#include "appli/common/buttons.h"
#include "appli/common/adc.h"
#include "appli/common/serial_dialog.h"
#include "object_ventilator.h"
#include "modules/nrfx/drivers/include/nrfx_saadc.h"
#include "appli/common/parameters.h"
#include "appli/common/rf_dialog.h"

static volatile int etat = 0;


void OBJECT_VENTILATOR_etat_updated_callback(int new_etat)
{

	etat = new_etat;

}


static ventilator_e state = VENTILATOR_INIT;
int16_t temperature;
static bool_e state_changement = FALSE;
void object_ventilator_changement_etat(void);

void object_ventilator_temperature(void)
{



	ADC_read(TEMP_OUTPUT, &temperature);



	debug_printf("Temperature est %d.\n", temperature);


	int temp_deg;

	temp_deg = (temperature)*10000 / 195000 ;

	debug_printf("La temperature en degre est %d. \n", temp_deg);

	/*(19,5mV/°C)*/
}

void object_ventilator_activation(void)
{
	switch(state) {
		case VENTILATOR_INIT:
			PARAMETERS_enable(PARAM_ACTUATOR_STATE, 0, TRUE, &OBJECT_VENTILATOR_etat_updated_callback, NULL);
			GPIO_init();
			ADC_init();
			GPIO_configure(MOSFET_PIN, NRF_GPIO_PIN_PULLUP, true);
			BUTTONS_add(BUTTON_NETWORK, PIN_BUTTON_NETWORK, TRUE, &object_ventilator_changement_etat, NULL, NULL, NULL);
			state = VENTILATOR_OFF;	//Changement d'état
			break;

		case VENTILATOR_ON:
			GPIO_write(MOSFET_PIN, true);


			if(state_changement)
			{
				state_changement = FALSE;
				state = VENTILATOR_OFF;	//Changement d'état
			}
			break;

		case VENTILATOR_OFF:
			GPIO_write(MOSFET_PIN, false);

			if(state_changement)
			{
				state_changement = FALSE;
				state = VENTILATOR_ON;	//Changement d'état
			}
			break;

		default:
			state = VENTILATOR_INIT;	//N'est jamais sensé se produire.
			break;
	}
	/*if(state == VENTILATOR_ON){
		GPIO_init();
		GPIO_configure(MOSFET_PIN, NRF_GPIO_PIN_PULLUP, true);
		GPIO_write(MOSFET_PIN, true); }
	else if (state == VENTILATOR_OFF){
		GPIO_write(MOSFET_PIN, false);
	}*/
}

void object_ventilator_changement_etat(void)
{

	state_changement = TRUE;
}


	//GPIO_write(MOSFET_PIN, false);

#endif
