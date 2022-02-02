/*
 * object_brightness_sensor.h
 *
 *  Created on: 4 févr. 2021
 *      Author: antho_9ak3x1e
 */

#ifndef APPLI_OBJECTS_OBJECT_BRIGHTNESS_SENSOR_H_
#define APPLI_OBJECTS_OBJECT_BRIGHTNESS_SENSOR_H_
#if OBJECT_ID == OBJECT_BRIGHTNESS_SENSOR

typedef enum{
	INIT = 0,
	GET_DATA,
	SEND_DATA,
	SLEEP,
	STOP
}state;


void OBJECT_BRIGHTNESS_SENSOR_state_machine();
void BUTTON_action_sleep(state state);
#endif
#endif /* APPLI_OBJECTS_OBJECT_BRIGHTNESS_SENSOR_H_ */
