/*
 * object_matrix_leds.h
 *
 *  Created on: 3 févr. 2021
 *      Author: meven
 */

#include "bsp/matrix_leds_32x32.h"

#ifndef APPLI_OBJECTS_OBJECT_MATRIX_LEDS_H_
#define APPLI_OBJECTS_OBJECT_MATRIX_LEDS_H_

void MATRIX_afficheur(uint32_t colorDonnees, uint32_t colorType);

void MATRIX_reset(matrix_t matrix[32][32]);

#endif /* APPLI_OBJECTS_OBJECT_MATRIX_LEDS_H_ */
