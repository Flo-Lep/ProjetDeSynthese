#include "../config.h"
#include "appli/common/gpio.h"
#include "objet_volet_roulant.h"
#include "appli/common/buttons.h"
#include "appli/common/parameters.h"
#include "bsp/pwm.h"
#include "serial_dialog.h"
#include "secretary.h"

#if OBJECT_ID == OBJECT_VOLET_ROULANT


void VOLET_ROULANT_MAIN(void){
	if (SERIAL_DIALOG_process_msg(8, 1)){
		PWM_init(Pin_BP_UP, 1)
				PWM_set_duty(1, 75);
	}
	else if(SERIAL_DIALOG_process_msg(8, 2)){
		PWM_init(Pin_BP_DOWN, 1)
				PWM_set_duty(2, 125);
	}else{
		if ((HAL_GPIO_ReadPin(Pin_BP_UP,2) == true) &&(HAL_GPIO_ReadPin(Pin_BP_DOWN,3) == false)){
			PWM_init(Pin_BP_UP, 1)
				PWM_set_duty(1, 75);
		}
		if((HAL_GPIO_ReadPin(Pin_BP_DOWN,3) == true) && (HAL_GPIO_ReadPin(Pin_BP_UP,2) == false)){
			PWM_init(Pin_BP_DOWN, 1)
				PWM_set_duty(2, 125);
		}
	}
}
