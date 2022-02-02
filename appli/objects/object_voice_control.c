/*
 * object_touch_screen.c
 *
 *  Created on: 6 févr. 2021
 *      Author: malaryth
 */

#include "../config.h"

#if OBJECT_ID == OBJECT_VOICE_CONTROL
	#include "../common/leds.h"
	#include "../common/gpio.h"
	#include "object_voice_control.h"

	typedef struct
	{
		bool_e initialized;
		bool_e changed;
		bool_e state;
		bool_e old_state;
		uint8_t pin;
	}voice_control_t;

	static voice_control_t voice_control_var[VOICE_CONTROL_NB];
	static voice_control_state_e state = INIT_VOICE_CONTROL;

	bool_e update = FALSE;

	void VOICE_CONTROL_init(void)
	{
		for(voice_control_e b = 0; b < VOICE_CONTROL_NB; b++)
		{
			voice_control_var[b].initialized = FALSE;
		}
	}

	void VOICE_CONTROL_process_main(void)
	{
		switch(state) {
			case INIT_VOICE_CONTROL:

				VOICE_CONTROL_init();

				VOICE_CONTROL_add(VOICE_CONTROL_COMMAND_1, LED_PIN, TRUE);
				VOICE_CONTROL_add(VOICE_CONTROL_COMMAND_2, TOUCH_SCREEN_PIN, TRUE);

				state = WAIT_UPDATE;	//Changement d'état
				break;

			case WAIT_UPDATE:
				VOICE_CONTROL_read();

				if(update) {
					update = FALSE;
					state = UPDATE;	//Changement d'état
				}
				break;

			case UPDATE:
				for(voice_control_e b = 0; b < VOICE_CONTROL_NB; b++)
				{
					if(voice_control_var[b].changed)
					{
						//TODO Envoyer à la station de base..

						voice_control_var[b].changed = FALSE;
						debug_printf("Commande %d : %d\n", b, voice_control_var[b].state);
					}
				}
				state = WAIT_UPDATE;	//Changement d'état
				break;

			default:
				state = INIT_VOICE_CONTROL;	//N'est jamais sensé se produire.
				break;
		}
	}

	void VOICE_CONTROL_add(voice_control_e id, uint8_t pin, bool_e pullup)
	{
		//configure la pin du voice controller concernée en entrée
		//enregistre le pin voice controller comme "initialisée"

		voice_control_var[id].pin = pin;

		GPIO_init();
		//on part du principe que tout les boutons sont no pullup
		GPIO_configure(voice_control_var[id].pin, (pullup)?NRF_GPIO_PIN_PULLUP:NRF_GPIO_PIN_NOPULL, 0);
		voice_control_var[id].initialized = TRUE;

		voice_control_var[id].changed = FALSE;
		voice_control_var[id].state = FALSE;
		voice_control_var[id].old_state = FALSE;
	}

	void VOICE_CONTROL_read(void)
	{
		for (voice_control_e b = 0; b < VOICE_CONTROL_NB; b++)
		{
			if(voice_control_var[b].initialized && !update)
			{
				voice_control_var[b].state = GPIO_read(voice_control_var[b].pin);

				if(voice_control_var[b].old_state != voice_control_var[b].state)
				{
					voice_control_var[b].old_state = voice_control_var[b].state;
					voice_control_var[b].changed = TRUE;
					update = TRUE;
				}
			}
		}
	}

#endif
