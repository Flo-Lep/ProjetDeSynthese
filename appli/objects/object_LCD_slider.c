
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
#include "nrf_drv_gpiote.h"

typedef enum
	{
		INIT,
		RUN,
		IDLE,
	}mode_e;

/************************PRIVATE VAR DECLARATIONS****************************/
static uint8_t slider_display;
static volatile bool_e update_display;
static volatile uint8_t slider_A; //Cette variable peut changer via une extit (on ne prend pas de raccourci)
static uint8_t slider_A_last_state;

/*******************PRIVATE FUNCTIONS PROTOTYPES***************************/
static void LCD_SLIDER_complete_init(void);
static void LCD_SLIDER_lcd_init(void);
static void LCD_SLIDER_encoder_init(void);
static void LCD_SLIDER_leds_init(void);
static void LCD_SLIDER_buttons_init(void);
static void LCD_SLIDER_short_press_button_callback_event(void);
static void LCD_SLIDER_state_machine(void);
static void LCD_SLIDER_movement_callback_extit(void);
static void LCD_SLIDER_update_display(void);
static void LCD_SLIDER_compute_slider_value(void);
static bool_e slider_switch_press_event(void);

/************************INIT FUNCTIONS****************************/
/*
  * @brief Fonction initialisant l'objet via l'appel des sous fonctions d'init
  */
void LCD_SLIDER_complete_init(void){
	LCD_SLIDER_lcd_init();
	LCD_SLIDER_encoder_init();
	LCD_SLIDER_leds_init();
	LCD_SLIDER_buttons_init();
	slider_display = 0;
	update_display = false;
	slider_A_last_state = GPIO_read(LCD_A_SLIDER_PIN);
	LCD2X16_printf("LCD SLIDER - INITIALISED");
	debug_printf("Appli initialised\n");
};

/*
  * @brief Fonction initialisant le LCD sur ses broches respectives. On en profite pour activer le convertisseur Boost 5V qui alimente le LCD.
  */
void LCD_SLIDER_lcd_init(void){
	LCD2X16_init();
	//Enable 5V boost converter to power screen
	GPIO_configure(BOOST_ENABLE_PIN, NRF_GPIO_PIN_PULLUP, 1);
	GPIO_write(BOOST_ENABLE_PIN, 1);

}

/*
  * @brief Fonction initialisant l'encodeur digital (3 sorties : A, B et Switch) ainsi qu'une interruption ext pour l'incrementation du slider
  */
void LCD_SLIDER_encoder_init(void){
	GPIO_configure(LCD_A_SLIDER_PIN, NRF_GPIO_PIN_PULLUP, 0);
	GPIO_configure(LCD_B_SLIDER_PIN, NRF_GPIO_PIN_PULLUP, 0);
	GPIO_configure(LCD_SWITCH_SLIDER_PIN, NRF_GPIO_PIN_PULLUP, 0);
	//Extit config
	nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
	in_config.sense = NRF_GPIOTE_POLARITY_LOTOHI;
	in_config.pull = NRF_GPIO_PIN_PULLUP;
	nrf_drv_gpiote_in_init(LCD_A_SLIDER_PIN, &in_config, &LCD_SLIDER_movement_callback_extit);
	nrf_drv_gpiote_in_event_enable(LCD_A_SLIDER_PIN, true);
}

/*
  * @brief Fonction initialisant les LEDs de l'objet (network + battery)
  */
void LCD_SLIDER_leds_init(void){
	LED_add(LED_ID_NETWORK, PIN_LED_NETWORK);
	LED_add(LED_ID_BATTERY, PIN_LED_BATTERY);
}

/*
  * @brief Fonction initialisant le bouton network de l'objet
  */
void LCD_SLIDER_buttons_init(void){
	BUTTONS_add(BUTTON_NETWORK, PIN_BUTTON_NETWORK, TRUE, &LCD_SLIDER_short_press_button_callback_event, NULL, NULL, NULL);
}

/*
  * @brief Cette fonction est appelee lorsque qu'un appui court est détecté sur le bouton network
  * @pre   Il faut préalablement avoir initialisé le bouton et y avoir renseigné la fonction de callback (via BUTTONS_add())
  */
void LCD_SLIDER_short_press_button_callback_event(void){
	debug_printf("Network button has been shortly pressed\n");
	//RF_DIALOG_send_msg_id_to_object(OBJECT_BASE_STATION, PING, 0, NULL);

}

/************************BACKGROUND FUNCTIONS****************************/

/*
  * @brief Unique fonction publique appelée dans le main. Elle permet de lancer l'execution de la tâche de fond du module
  */
void LCD_SLIDER_process_main(void){
	LCD_SLIDER_state_machine();
};

/*
  * @brief Fonction appelée en tâche de fond. Cette machine à état gère les différents modes
  */
void LCD_SLIDER_state_machine(void){
	static mode_e state  = INIT;
	switch(state){
	case INIT :
		LCD_SLIDER_complete_init();
		LCD2X16_printf("Counter : ");
		state = RUN;
		debug_printf("Switching to 'RUN' mode\n");
		break;
	case RUN :
		//LCD_SLIDER_compute_slider_value();
		if(update_display){
			LCD_SLIDER_update_display();
			update_display = false;
		}
		if(slider_switch_press_event()){
			//Send slider value to the server
			//RF_DIALOG_send_msg_id_to_object(recipient_e obj_id,msg_id_e msg_id, uint8_t datasize, uint8_t * datas);
			//RF_DIALOG_send_msg_id_to_object(OBJECT_BASE_STATION, PING, 0, NULL);
			//RF_DIALOG_send_msg_id_to_basestation(PARAMETER_IS, 5, slider_display); ?//tab 5 donnï¿½es
			debug_printf("Switch button pressed\n");
			debug_printf("Slider value : %d\n", slider_display);
		}
		break;
	case IDLE :
		break;
	default :
		break;
	}
};

/*
  * @brief Cette fonction met à jour l'affichage du compteur incrémenté par le slider
  * @pre   L'écran lcd2x16 doit avoir été initialisé en amont
  */
void LCD_SLIDER_update_display(void){
	LCD2X16_setCursor(0, 5);
	LCD2X16_printf("%d\%\n", slider_display);

}


/*void LCD_SLIDER_compute_slider_value(void){
	slider_A = GPIO_read(LCD_A_SLIDER_PIN);
	//debug_printf("Slider A %d\n", slider_A);
	if(slider_A!=slider_A_last_state){
		if(GPIO_read(LCD_B_SLIDER_PIN)!=slider_A){ //ie clockwise
			slider_display++;
			debug_printf("Slider ++ : %d\n", slider_display);
		}
		else{//ie counterclockwise
			slider_display--;
			debug_printf("Slider -- : %d\n", slider_display);
		}
		slider_A_last_state = slider_A;
		update_display = true;
	}
}*/

/*
  * @brief Cette fonction est appelée par une interruption ext dès que le slider tourne
  */
void LCD_SLIDER_movement_callback_extit(void){
	debug_printf("It activated");
	update_display = true;
}


/**********************************USEFUL FUNCTIONS************************************/
/*
 * @brief Cette fonction détecte un appui sur le slider (switch interne)
 * @pre	  Le bouton doit etre initialise selon une broche en amont (ds le main par ex)
 * @author : Nirgal
 */
bool_e slider_switch_press_event(void)
{
	static bool_e previous_button = false; //ï¿½tat prï¿½cï¿½dent du bouton
	bool_e ret = false;
	bool_e current_button; //ï¿½tat actuel du bouton
	//bouton en logique inverse, d'oï¿½ le '!'
	current_button = GPIO_read(LCD_SWITCH_SLIDER_PIN);
	//si le bouton est appuyï¿½ et ne l'ï¿½tait pas avant, champomy !
	if(current_button && !previous_button)
	ret = true;
	//on mï¿½morise l'ï¿½tat actuel pour le prochain passage
	previous_button = current_button;
	return ret;
}

#endif

