/**
 * @addtogroup lcd2x16
 *
 * Afficheur LCD 2 lignes de 16 caractères.
 * 
 *
 * L'afficheur est utilisé en mode 4 bits. Les ports d'entrée/sortie se décomposent par conséquent en
 *    - trois bits de commande
 *    - quatre bits de données
 *
 * Les bits de commande sont configurées en sortie tandis que les bits de données peuvent être accédés
 * soit en lecture, soit en écriture.
 *
 * Eléments à modifier pour configurer l'écran LCD :
 *    - pour la gestion des délais : #CLOCK_FREQUENCY
 *    - Configuration des ports de données et de commande
 * @{
 */

/**
 * @file lcd2x16_c18.h
 *
 *  Driver d'afficheur LCD 2 lignes de 16 caractères.
 *
 *  @version 1.3
 *  @date 24/08/2015
 */

#ifndef __LCD2X16_H
#define __LCD2X16_H

/**
 * Bits du port de données.
 * Adaptez ces configurations à votre design (ports et pins utilisés).
 */

	#define	PIN_DATA_0	LCD_DATA_1_PIN
	#define	PIN_DATA_1	LCD_DATA_2_PIN
	#define	PIN_DATA_2	LCD_DATA_3_PIN
	#define	PIN_DATA_3	LCD_DATA_4_PIN
	#define PIN_RS		LCD_RS_PIN
	#define PIN_RW		LCD_RW_PIN
	#define PIN_E		LCD_E_PIN

void LCD2X16_demo(void);

/**
 * Fonction d'initialisation du LCD.
 *
 * Cette fonction doit être appelée préalablement à toute utilisation de l'écran LCD.
 */
void LCD2X16_init(void);

#endif
