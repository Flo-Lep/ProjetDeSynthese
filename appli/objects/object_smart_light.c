/*
 * object_smart_light.c
 *
 *  Created on: 10 f�vr. 2021
 *      Author: Thomas
 */
#include "../config.h"
#include "appli/common/gpio.h"
#include "object_smart_light.h"
#include "appli/common/buttons.h"
#include "appli/common/parameters.h"
#include "appli/common/serial_dialog.h"







#if OBJECT_ID == OBJECT_SMART_LIGHT



static bool_e appui;

bool_e DetectAppui(void)
{
	 appui = TRUE;
	return appui;

}


void Configuration(void){
	GPIO_configure(MOSFET_LED_FROIDE2, NRF_GPIO_PIN_NOPULL, TRUE);//configure le pin des mosfets en sortie
	GPIO_configure(MOSFET_LED_FROIDE, NRF_GPIO_PIN_NOPULL, TRUE);
	GPIO_configure(MOSFET_LED_CHAUDE2, NRF_GPIO_PIN_NOPULL, TRUE);
	GPIO_configure(MOSFET_LED_CHAUDE, NRF_GPIO_PIN_NOPULL, TRUE);
	BUTTONS_add(BUTTON_USER0, PIN_BUTTON_LOCAL, TRUE, &DetectAppui,NULL,NULL,NULL);
	}


void OBJECT_SMART_LIGHT_UPDATED_MODE_callback(mode_e new_mode){

debug_printf("nouveau mode envoyé depuis la station de base : %lx\n", mode);
mode = new_mode;


}


void Eteindre(void){
	GPIO_write(MOSFET_LED_FROIDE2, FALSE);
	GPIO_write(MOSFET_LED_FROIDE, FALSE);
	GPIO_write(MOSFET_LED_CHAUDE2, FALSE);
	GPIO_write(MOSFET_LED_CHAUDE, FALSE);
}

void AllumageTotale(void){

	GPIO_write(MOSFET_LED_FROIDE2, TRUE);
	GPIO_write(MOSFET_LED_FROIDE, TRUE);
	GPIO_write(MOSFET_LED_CHAUDE2, TRUE);
	GPIO_write(MOSFET_LED_CHAUDE, TRUE);

}
void AllumageFroid(void){

	GPIO_write(MOSFET_LED_FROIDE2, TRUE);
	GPIO_write(MOSFET_LED_FROIDE, TRUE);

}

void AllumageChaud(void){

	GPIO_write(MOSFET_LED_CHAUDE2, TRUE);
	GPIO_write(MOSFET_LED_CHAUDE, TRUE);
}

void Smart_light_Main(void){
	{

		/*static mode_e previous_mode = DEBUT; //Sauvegarder l'état précédent.
		bool_e entrance = (mode != previous_mode)?TRUE:FALSE;*/



		switch(mode)
			{
				case DEBUT:
					Configuration();
					PARAMETERS_enable(PARAM_MODE,DEBUT, TRUE, &OBJECT_SMART_LIGHT_UPDATED_MODE_callback, NULL);

						mode = ALLUMAGE;

					break;
				case ALLUMAGE:

					AllumageTotale();
					PARAMETERS_enable(PARAM_MODE,DEBUT, TRUE, &OBJECT_SMART_LIGHT_UPDATED_MODE_callback, NULL);
							if(appui)
							{
								appui = FALSE;
								Eteindre();
								mode = FROID;

							}
					break;
				case FROID:

					AllumageFroid();

							if(appui)
							{
								appui = FALSE;
								Eteindre();
								mode = CHAUD;
							}

					break;
				case CHAUD:

					AllumageChaud();

							if(appui)
							{
								appui = FALSE;
								Eteindre();
								mode = EXTINCTION;
							}
					break;
				case EXTINCTION:

					Eteindre();

							if(appui)
								{
								appui = FALSE;
								mode = ALLUMAGE;
								}


					break;



			}



	}

}


#endif


