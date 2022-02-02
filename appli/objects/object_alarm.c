/*
 * object_alarm.c
 *
 *  Created on: 18 févr. 2021
 *      Author: briyd
 */

#include "../config.h"

#if OBJECT_ID == OBJECT_ALARM
#include "object_alarm.h"
#include "buttons.h"
#include <ctime>
#include <iostream>

static bool_e state_changement = FALSE;
static volatile uint32_t my_status = 0;

void object_alarm_main(void){

	if incendie{
		while(incendie = true) {
			alarm(200, 2000, 20);
		}
		nosound();
	}

}

void alarm(int depart, int arrive,int pas){
	int a;
	for (a = depart; a <= arrive; a = a + pas)
	{
	     sound(a);
	     delay(25);
	}

}

void alarm_stop(){
	BUTTONS_set_long_press_callback(BUTTON_NETWORK, &etat_ampli);

}


void message_lu(){
	// reception d'un message du serveur

	switch(){
	case //incendie// :

		break;
	case //reveil//:
		break;
	case //infraction// :
		break;

	}
}

void etat_ampli(){
	state_changement = TRUE;
}



//------------------------------------------------------------------------------------------------

void OBJECT_ALARM_hurleur(){
	  if(my_status ==1)
	  {
	    time_t t1=time(0);
	    mciSendString("play mp3 from 0 to 30000 wait", NULL, 0, NULL);
	    time_t t2=time(0);
	  }
	  else

	  mciSendString("close mp3", NULL, 0, NULL);

	  return 0;
	}
}

void OBJECT_ALARM_color_updated_callback(int32_t new_color)
{
	debug_printf("nouveau status envoyée depuis la station de base : %lx\n", new_status);
	my_status = new_status;
}


void OBJECT_ALARM_state_machine(void){
	typedef enum{
			INIT,
			RUN,
			SLEEP
		}state_e;

	static state_e state = INIT;
	static uint32_t previous_status = -1;
	switch(state)
	{
		case INIT:
			PARAMETERS_enable(PARAM_ALARM_WAY, 0xCAFEDECA, TRUE, &OBJECT_ALARM_updated_callback, NULL);
			WS2812_init(PIN_UART_RX, 30);
			state = RUN;
		break;

		case RUN:{
			if(my_status != previous_status)
			{
				WS2812_display_full(my_status);
				WS2812_refresh();
				previous_status = my_status;
			}
			break;
		}

		case SLEEP:{
			break;
		}

		default:
			break;
	}
}
#endif

