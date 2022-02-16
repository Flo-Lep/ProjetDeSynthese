/*
 * object_LCD_slider.h
 *
 *  Created on: 20 janv. 2022
 *      Author: Florentin LEPELTIER
 */

#ifndef APPLI_OBJECTS_OBJECT_LCD_SLIDER_H_
#define APPLI_OBJECTS_OBJECT_LCD_SLIDER_H_

#define TIMER_BETWEEN_VALUE_SENDING		5001 //ms (1 to display the first second on the screen (in this case 5))

typedef enum{
	SWITCH,
	A
}pin_type_e;

void LCD_SLIDER_process_main(void);

#endif /* APPLI_OBJECTS_OBJECT_LCD_SLIDER_H_ */
