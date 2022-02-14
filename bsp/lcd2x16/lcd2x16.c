/**
 * @addtogroup lcd2x16
 *
 * @{
 */

/**
 * @file lcd2x16_c18.c
 *
 *  Driver d'afficheur LCD 2 lignes de 16 caractères.
 *
 *  @version 1.3
 *  @date 24/08/2015
 */

/**
 * @date 	10/02/2022
 * @author  Florentin LEPELTIER
 * Ce driver a été adapté au module nrf via la SDK. La version originale est conçue pour µC STM32.
 * INFOS IMPORTANTES :
 * Les pins de commande doivent être en pulldown de préférence (elles servent à set des configurations spécifiques à l'écran --> Cf datasheet...)
 * ATTENTION : /!\ Même non utilisées, les pins de data D0-D3 doivent être reliées au GND, sans quoi l'écran sera incapable de'interprêter certaines commandes...
 */
#include "../appli/config.h"

#if USE_SCREEN_LCD2X16

#include "lcd2x16.h"
#include "../appli/common/macro_types.h"
#include <stdarg.h>
#include <stdio.h>
#include "../appli/common/gpio.h"
#include "./appli/common/systick.h"
//Portage...
#define GPIO_SET_OUTPUT(pin)		GPIO_configure(pin, GPIO_PIN_CNF_PULL_Pulldown, true)
#define GPIO_SET_INPUT(pin)			GPIO_configure(pin, GPIO_PIN_CNF_PULL_Pulldown, false)
#define GPIO_WRITE(pin, value) 		GPIO_write(pin, value)
#define GPIO_READ(pin)				GPIO_read(pin)
#define DELAY_MS(x)					SYSTICK_delay_ms(x)


static void LCD2X16_set_command_pins_to_output(void);
static void LCD2X16_set_data_pins_to_output(void);
static void LCD2X16_send_config_command(bool_e D4_value, bool_e D5_value, bool_e D6_value, bool_e D7_value);
static void LCD2X16_write_data(bool_e D4_value, bool_e D5_value, bool_e D6_value, bool_e D7_value, bool_e D4_2nd_value, bool_e D5_2nd_value, bool_e D6_2nd_value, bool_e D7_2nd_value);
static void LCD2X16_validate_data(void);
static void LCD2X16_set_all_pins_to_zero(void);
static void LCD2X16_validate_data(void);

/*=================================================================================================*/
//

void LCD2X16_set_command_pins_to_output(void){
	// Configuration des ports de commande et de données en écriture
	GPIO_SET_OUTPUT(PIN_RW);
	GPIO_SET_OUTPUT(PIN_RS);
	GPIO_SET_OUTPUT(PIN_E);
}

void LCD2X16_set_data_pins_to_output(void){
	GPIO_SET_OUTPUT(PIN_DATA_0);
	GPIO_SET_OUTPUT(PIN_DATA_1);
	GPIO_SET_OUTPUT(PIN_DATA_2);
	GPIO_SET_OUTPUT(PIN_DATA_3);
}
//@pre : les pins de commande doivent être config en sorties
void LCD2X16_send_config_command(bool_e D4_value, bool_e D5_value, bool_e D6_value, bool_e D7_value){
	GPIO_WRITE(PIN_DATA_0, D4_value);
	GPIO_WRITE(PIN_DATA_1, D5_value);
	GPIO_WRITE(PIN_DATA_2, D6_value);
	GPIO_WRITE(PIN_DATA_3, D7_value);
	DELAY_MS(5);
	LCD2X16_validate_data(); //ie press enable
	DELAY_MS(5);

}

void LCD2X16_write_data(bool_e D4_value, bool_e D5_value, bool_e D6_value, bool_e D7_value, bool_e D4_2nd_value, bool_e D5_2nd_value, bool_e D6_2nd_value, bool_e D7_2nd_value){
	GPIO_WRITE(PIN_RS, 1);
	DELAY_MS(5);
	LCD2X16_send_config_command(D4_value, D5_value, D6_value, D7_value);
	LCD2X16_send_config_command(D4_2nd_value, D5_2nd_value, D6_2nd_value, D7_2nd_value);
	DELAY_MS(5);
	GPIO_WRITE(PIN_RS, 0);
	DELAY_MS(5);

}

void LCD2X16_validate_data(void){
	GPIO_WRITE(PIN_E, 1);
	DELAY_MS(1);
	GPIO_WRITE(PIN_E, 0);
	DELAY_MS(1);
}

void LCD2X16_set_all_pins_to_zero(void){
	GPIO_WRITE(PIN_DATA_0, 0);
	GPIO_WRITE(PIN_DATA_1, 0);
	GPIO_WRITE(PIN_DATA_2, 0);
	GPIO_WRITE(PIN_DATA_3, 0);
	GPIO_WRITE(PIN_E, 0);
	GPIO_WRITE(PIN_RS, 0);
	GPIO_WRITE(PIN_RW, 0);
	DELAY_MS(40);
}

void LCD2X16_init(void){
		LCD2X16_set_command_pins_to_output();
		LCD2X16_set_data_pins_to_output();

		//Initialisation de toutes les broches à 0
		LCD2X16_set_all_pins_to_zero();

		DELAY_MS(40);

		//4 bit mode, 1 line
		LCD2X16_send_config_command(0,1,0,0);

		//Clear the display
		LCD2X16_send_config_command(0,0,0,0);
		LCD2X16_send_config_command(1,0,0,0); //D0-D3 via D4_D7

		//Return home
		LCD2X16_send_config_command(0,0,0,0);
		LCD2X16_send_config_command(0,1,0,0);

		//Display on, no cursor
		LCD2X16_send_config_command(0,0,0,0);
		LCD2X16_send_config_command(0,0,1,1);

		DELAY_MS(40);

		//Print init data : H (72 hexa)
		LCD2X16_write_data(0,0,1,0,0,0,0,1);

		//Print init data : I (73 hexa)
		LCD2X16_write_data(0,0,1,0,1,0,0,1);

		DELAY_MS(2);
}

void LCD2X16_demo(void){
	LCD2X16_init();
}

#endif
