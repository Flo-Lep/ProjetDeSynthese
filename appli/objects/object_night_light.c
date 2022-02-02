// Eclairage nocturne avec des LEDS RGB WS2812

#include "../config.h"
#include "../../bsp/WS2812.h"
#include "../common/parameters.h"

#if OBJECT_ID == OBJECT_NIGHT_LIGHT

static volatile uint32_t my_color = 0;

void OBJECT_NIGHT_LIGHT_color_updated_callback(int32_t new_color)
{
	debug_printf("nouvelle couleur envoye depuis la station de base : %lx\n", new_color);
	my_color = new_color;
}

void OBJECT_NIGHT_LIGHT_chenillard(my_color){
	for (int var = 0; var <= 30; ++var) {
		WS2812_display_only_one_pixel(my_color,COLOR_BLACK,var);
		WS2812_refresh();
	}
}



void OBJECT_NIGHT_LIGHT_state_machine(void){
	typedef enum{
			INIT,
			RUN,
			SLEEP
		}state_e;

	static state_e state = INIT;
	static uint32_t previous_color = -1;
	switch(state)
	{
		case INIT:
			PARAMETERS_enable(PARAM_COLOR, 0xCAFEDECA, TRUE, &OBJECT_NIGHT_LIGHT_color_updated_callback, NULL);
			WS2812_init(WS2812_PIN, 30);
			state = RUN;
		break;

		case RUN:{
			if(my_color != previous_color)
			{
				WS2812_display_full(my_color);
				WS2812_refresh();
				previous_color = my_color;
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
