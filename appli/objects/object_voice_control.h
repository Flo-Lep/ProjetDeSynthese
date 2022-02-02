/*
 * object_touch_screen.c
 *
 *  Created on: 6 févr. 2021
 *      Author: malaryth
 */

typedef enum
{
	VOICE_CONTROL_COMMAND_1,
	VOICE_CONTROL_COMMAND_2,
	VOICE_CONTROL_COMMAND_3,
	VOICE_CONTROL_COMMAND_4,
	VOICE_CONTROL_COMMAND_5,
	VOICE_CONTROL_COMMAND_6,
	VOICE_CONTROL_COMMAND_7,
	VOICE_CONTROL_COMMAND_8,
	VOICE_CONTROL_NB	//Nombre max de variable
}voice_control_e;

typedef enum
{
	INIT_VOICE_CONTROL,
    WAIT_UPDATE,
	UPDATE,
}voice_control_state_e;

void VOICE_CONTROL_init(void);

void VOICE_CONTROL_process_main(void);

void VOICE_CONTROL_add(voice_control_e id, uint8_t pin, bool_e pullup);

void VOICE_CONTROL_read(void);
