/**
 * @addtogroup lcd2x16
 *
 * Afficheur LCD 2 lignes de 16 caract�res.
 * 
 *
 * L'afficheur est utilis� en mode 4 bits. Les ports d'entr�e/sortie se d�composent par cons�quent en
 *    - trois bits de commande
 *    - quatre bits de donn�es
 *
 * Les bits de commande sont configur�es en sortie tandis que les bits de donn�es peuvent �tre acc�d�s
 * soit en lecture, soit en �criture.
 *
 * El�ments � modifier pour configurer l'�cran LCD :
 *    - pour la gestion des d�lais : #CLOCK_FREQUENCY
 *    - Configuration des ports de donn�es et de commande
 * @{
 */

/**
 * @file lcd2x16_c18.h
 *
 *  Driver d'afficheur LCD 2 lignes de 16 caract�res.
 *
 *  @version 1.3
 *  @date 24/08/2015
 */

#ifndef __LCD2X16_H
#define __LCD2X16_H

/**
 * Bits du port de donn�es.
 * Adaptez ces configurations � votre design (ports et pins utilis�s).
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
 * Cette fonction doit �tre appel�e pr�alablement � toute utilisation de l'�cran LCD.
 */
void LCD2X16_init(void);

#endif
