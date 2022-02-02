/*
 * object_touch_screen.c
 *
 *  Created on: 6 fevr. 2021
 *      Author: verhasya
 */

#include "../config.h"
#if OBJECT_ID == OBJECT_TOUCH_SCREEN

#include "object_touch_screen.h"
#include "../bsp/ili9341/ili9341_fonts.h"
#include "../bsp/ili9341/nrf_lcd.h"
#include "bsp/ili9341/ili9341.h"
#include "nrfx_spi.h"
#include "bsp/ili9341/ili9341_xpt2046.h"
#include "../common/parameters.h"

#define COLORMESSAGE 		PARAM_SCREEN_COLOR //ici on peut penser à recuperer la couleur depuis un parametre envoye de la station de base

extern const nrf_lcd_t nrf_lcd_ili9341;



void object_touch_screen_process_main(void)
{
    typedef enum{
    	INIT,
        COLOR,
        DEFAULT
    }state_e;

    static state_e state = INIT;
    switch(state){
    case INIT:
        nrf_lcd_ili9341.lcd_init();
        if (COLORMESSAGE != 0){
        	state = COLOR;
        }
        else{
            state = DEFAULT;
        }
        break;
    case COLOR:
    	nrf_lcd_ili9341.lcd_int_fill(0, 0, 240, 320, COLORMESSAGE);
    	break;
    case DEFAULT:
        nrf_lcd_ili9341.lcd_int_fill(0, 0, 240, 320, ILI9341_COLOR_BLACK);
        break;
    default:
    	break;
    }
}

#endif
