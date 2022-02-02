/*
 * object_ventilator.h
 *
 *  Created on: 12 févr. 2021
 *      Author: Utilisateur
 */

#ifndef APPLI_OBJECTS_OBJECT_VENTILATOR_H_
#define APPLI_OBJECTS_OBJECT_VENTILATOR_H_

void object_ventilator_activation(void);
void object_ventilator_temperature(void);

typedef enum{
	 VENTILATOR_INIT,
	 VENTILATOR_ON,
	 VENTILATOR_OFF,
	 VENTILATOR_NB		//nombre max d'etat
}ventilator_e;

#endif /* APPLI_OBJECTS_OBJECT_VENTILATOR_H_ */
