
/*
 * object_LCD_slider.c
 *
 *  Created on: 20 janv. 2022
 *      Author: Florentin LEPELTIER
 */

#include "../config.h"
#include "../common/gpio.h"
#include "../common/buttons.h"
#include "../common/leds.h"
#include "../bsp/lcd2x16/lcd2x16.h"

#if OBJECT_ID == OBJECT_LCD_SLIDER
/*
	Reads encoder value and updates the screen accordingly
  	Encoder : The slider is composed of three signals coming back on 3 pins of the NRF : Output_A(P0.12) / Output_B(P0.11) / Switch_output(P0.10)
  		-When slider is turned clockwise, A!=B
  		-When it turns counterclockwise, A==B
  	LCD : The LCD is wired on the following pins : Enable(P0.25) / RW(P0.26) / RS(P0.27) / D7(P0.28) / D6(P0.29) / D5(P0.30) / D4(P0.31)
*/
#include "../bsp/lcd2x16/lcd2x16.h"
#include "object_LCD_slider.h"

typedef enum
	{
		INIT,
		RUN,
		IDLE,
	}mode_e;

/************************PRIVATE VAR DECLARATIONS****************************/
static volatile uint8_t slider_display;
static bool_e update_display;
static volatile uint8_t slider_A;
static volatile uint8_t slider_A_last_state;

/*******************PRIVATE FUNCTIONS PROTOTYPES***************************/
static void LCD_SLIDER_complete_init(void);
static void LCD_SLIDER_lcd_init(void);
static void LCD_SLIDER_encoder_init(void);
static void LCD_SLIDER_leds_init(void);
static void LCD_SLIDER_buttons_init(void);
static void LCD_SLIDER_short_press_button_callback_event(void);
static void LCD_SLIDER_machine_state(void);
static void LCD_SLIDER_update_display(void);
static void LCD_SLIDER_compute_slider_value(void);
static bool_e slider_switch_press_event(void);

/************************INIT FUNCTIONS****************************/
void LCD_SLIDER_complete_init(void){
	LCD_SLIDER_lcd_init();
	LCD_SLIDER_encoder_init();
	LCD_SLIDER_leds_init();
	LCD_SLIDER_buttons_init();
	slider_display = 0;
	update_display = false;
	LCD2X16_printf("LCD SLIDER - INITIALISED");
};


void LCD_SLIDER_lcd_init(void){
	LCD2X16_init();
	//Enable 5V boost converter to power screen
	GPIO_configure(BOOST_ENABLE_PIN, GPIO_PIN_CNF_PULL_Pullup, TRUE);
	GPIO_write(BOOST_ENABLE_PIN, 1);

}

void LCD_SLIDER_encoder_init(void){
	GPIO_configure(LCD_A_SLIDER_PIN, GPIO_PIN_CNF_PULL_Pullup, FALSE);
	GPIO_configure(LCD_B_SLIDER_PIN, GPIO_PIN_CNF_PULL_Pullup, FALSE);
	GPIO_configure(LCD_SWITCH_SLIDER_PIN, GPIO_PIN_CNF_PULL_Pullup, FALSE);
	slider_A_last_state = GPIO_read(LCD_A_SLIDER_PIN);
}

void LCD_SLIDER_leds_init(void){
	LED_add(LED_ID_NETWORK, PIN_LED_NETWORK);
	LED_add(LED_ID_BATTERY, PIN_LED_BATTERY);
}

void LCD_SLIDER_buttons_init(void){
	BUTTONS_add(BUTTON_NETWORK, PIN_BUTTON_NETWORK, TRUE, &LCD_SLIDER_short_press_button_callback_event, NULL, NULL, NULL);
}

void LCD_SLIDER_short_press_button_callback_event(void){
	printf("Network button has been shortly pressed");
	//RF_DIALOG_send_msg_id_to_object(OBJECT_BASE_STATION, PING, 0, NULL);

}

/************************BACKGROUND FUNCTIONS****************************/

void LCD_SLIDER_process_main(void){
	LCD_SLIDER_machine_state();
};

void LCD_SLIDER_machine_state(void){
	static mode_e state  = INIT;
	switch(state){
	case INIT :
		LCD_SLIDER_complete_init();
		state = RUN;
		break;
	case RUN :
		LCD_SLIDER_compute_slider_value();
		if(update_display){
			LCD_SLIDER_update_display();
			update_display = false;
		}
		if(slider_switch_press_event()){
			//Send slider value to the server
			//RF_DIALOG_send_msg_id_to_object(recipient_e obj_id,msg_id_e msg_id, uint8_t datasize, uint8_t * datas);
			//RF_DIALOG_send_msg_id_to_object(OBJECT_BASE_STATION, PING, 0, NULL);
			//RF_DIALOG_send_msg_id_to_basestation(PARAMETER_IS, 5, slider_display); ?//tab 5 données
			printf("Switch button pressed\n");
			printf("Slider value : %d", slider_display);
		}
		break;
	case IDLE :
		break;
	default :
		break;
	}
};

void LCD_SLIDER_update_display(void){

}

void LCD_SLIDER_compute_slider_value(void){
	slider_A = GPIO_read(LCD_A_SLIDER_PIN);
	if(slider_A!=slider_A_last_state){
		if(GPIO_read(LCD_B_SLIDER_PIN)!=slider_A){ //ie clockwise
			slider_display++;
		}
		else{//ie counterclockwise
			slider_display--;
		}
		slider_A_last_state = slider_A;
		update_display = true;
	}
}

/*bool LCD_SLIDER_moves(void){
	if(slider_A_output_press_event() || slider_B_output_press_event()){
		return true;
	}
	else{
		return false;
	}
}*/

/**********************************USEFUL FUNCTIONS************************************/
/*
 * @brief Ces fonctions detectent l'utilisation du slider
 * @pre	  Le bouton doit etre initialise selon une broche en amont (ds le main par ex)
 * @author : Nirgal
 */
bool_e slider_switch_press_event(void)
{
	static bool_e previous_button = false; //état précédent du bouton
	bool_e ret = false;
	bool_e current_button; //état actuel du bouton
	//bouton en logique inverse, d'où le '!'
	current_button = GPIO_read(LCD_SWITCH_SLIDER_PIN);
	//si le bouton est appuyé et ne l'était pas avant, champomy !
	if(current_button && !previous_button)
	ret = true;
	//on mémorise l'état actuel pour le prochain passage
	previous_button = current_button;
	return ret;
}

/*bool_e slider_A_output_press_event(void)
{
	static bool_e previous_button = false;
	bool_e ret = false;
	bool_e current_button;
	current_button = GPIO_read(A_SLIDER_PIN);
	if(current_button && !previous_button)
	ret = true;
	previous_button = current_button;
	return ret;
}*/

/*bool_e slider_B_output_press_event(void)
{
	static bool_e previous_button = false;
	bool_e ret = false;
	bool_e current_button;
	GPIO_read(B_SLIDER_PIN);
	if(current_button && !previous_button)
	ret = true;
	previous_button = current_button;
	return ret;
}*/

#endif

