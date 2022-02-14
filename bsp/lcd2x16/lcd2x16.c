/**
 * @date 	10/02/2022
 * @author  Florentin LEPELTIER
 * Ce driver a été adapté au module NRF via la SDK. La version originale est conçue pour µC STM32. Il est incomplet et n'utilise qu'une seule des deux lignes du LCD en mode 4-bits. C'est du bricolage comme on en a jamais vu.
 * @INFOS IMPORTANTES :
 * Les pins de commande doivent être en pulldown de préférence (elles servent à set des configurations spécifiques à l'écran --> Cf datasheet...)
 * ATTENTION : /!\ Même non utilisées, les pins de data D0-D3 doivent être reliées au GND, sans quoi l'écran sera incapable de'interprêter certaines commandes...
 * Pour plus d'informatiins, il est nécessaire de lire et de comprendre la datasheet
 */
#include "../appli/config.h"

#if USE_SCREEN_LCD2X16

#include "lcd2x16.h"
#include "../appli/common/macro_types.h"
#include <stdarg.h>
#include <stdio.h>
#include "../appli/common/gpio.h"
#include "./appli/common/systick.h"
//Portage...
#define GPIO_SET_OUTPUT(pin)		GPIO_configure(pin, GPIO_PIN_CNF_PULL_Pulldown, true)
#define GPIO_SET_INPUT(pin)			GPIO_configure(pin, GPIO_PIN_CNF_PULL_Pulldown, false)
#define GPIO_WRITE(pin, value) 		GPIO_write(pin, value)
#define GPIO_READ(pin)				GPIO_read(pin)
#define DELAY_MS(x)					SYSTICK_delay_ms(x)

/**
 * Adresse de la deuxième ligne de l'écran LCD.
 *
 * Adresse dans la RAM interne de l'écran LCD du premier caractère de la deuxième ligne.
 */
#define LCD2X16_LINE_TWO_AD 0x40

//NEW LIBRARY FUNCTIONS
static void LCD2X16_set_command_pins_to_output(void);
static void LCD2X16_set_data_pins_to_output(void);
static void LCD2X16_send_config_command(bool_e D4_value, bool_e D5_value, bool_e D6_value, bool_e D7_value);
static void LCD2X16_write_data(bool_e D4_value, bool_e D5_value, bool_e D6_value, bool_e D7_value, bool_e D4_2nd_value, bool_e D5_2nd_value, bool_e D6_2nd_value, bool_e D7_2nd_value);
static void LCD2X16_validate_data(void);
static void LCD2X16_set_all_pins_to_zero(void);
static void LCD2X16_validate_data(void);
static void LCD2X16_config_four_bit_mode_one_line(void);
static void LCD2X16_config_display_on_no_cursor(void);
static void LCD2X16_clear_display(void);
static void LCD2X16_return_home(void);
//STM32 LIBRARY
static uint8_t LCD2X16_getByte(uint8_t rs);
static void LCD2X16_sendNibble(unsigned char n);
static void LCD2X16_sendByte( unsigned char rs, unsigned char n);

/*=================================================================================================*/

/*
 * Cette fonction blocante a pour but de vous aider à appréhender les fonctionnalités de ce module logiciel.
 */
void LCD2X16_demo(void)
{
	LCD2X16_init();
	LCD2X16_printf("LCD2x16 - demo");
	while(1)
	{

	}
}

void LCD2X16_set_command_pins_to_output(void){
	// Configuration des ports de commande et de données en écriture
	GPIO_SET_OUTPUT(PIN_RW);
	GPIO_SET_OUTPUT(PIN_RS);
	GPIO_SET_OUTPUT(PIN_E);
}

void LCD2X16_set_data_pins_to_output(void){
	GPIO_SET_OUTPUT(PIN_DATA_0);
	GPIO_SET_OUTPUT(PIN_DATA_1);
	GPIO_SET_OUTPUT(PIN_DATA_2);
	GPIO_SET_OUTPUT(PIN_DATA_3);
}
//@pre : les pins de commande doivent être config en sorties
void LCD2X16_send_config_command(bool_e D4_value, bool_e D5_value, bool_e D6_value, bool_e D7_value){
	GPIO_WRITE(PIN_DATA_0, D4_value);
	GPIO_WRITE(PIN_DATA_1, D5_value);
	GPIO_WRITE(PIN_DATA_2, D6_value);
	GPIO_WRITE(PIN_DATA_3, D7_value);
	DELAY_MS(5);
	LCD2X16_validate_data(); //ie press enable
	DELAY_MS(5);

}

void LCD2X16_write_data(bool_e D4_value, bool_e D5_value, bool_e D6_value, bool_e D7_value, bool_e D4_2nd_value, bool_e D5_2nd_value, bool_e D6_2nd_value, bool_e D7_2nd_value){
	GPIO_WRITE(PIN_RS, 1);
	DELAY_MS(5);
	LCD2X16_send_config_command(D4_value, D5_value, D6_value, D7_value);
	LCD2X16_send_config_command(D4_2nd_value, D5_2nd_value, D6_2nd_value, D7_2nd_value);
	DELAY_MS(5);
	GPIO_WRITE(PIN_RS, 0);
	DELAY_MS(5);

}

void LCD2X16_validate_data(void){
	GPIO_WRITE(PIN_E, 1);
	DELAY_MS(1);
	GPIO_WRITE(PIN_E, 0);
	DELAY_MS(1);
}

void LCD2X16_set_all_pins_to_zero(void){
	GPIO_WRITE(PIN_DATA_0, 0);
	GPIO_WRITE(PIN_DATA_1, 0);
	GPIO_WRITE(PIN_DATA_2, 0);
	GPIO_WRITE(PIN_DATA_3, 0);
	GPIO_WRITE(PIN_E, 0);
	GPIO_WRITE(PIN_RS, 0);
	GPIO_WRITE(PIN_RW, 0);
	DELAY_MS(40);
}

void LCD2X16_init(void){
		LCD2X16_set_command_pins_to_output();
		LCD2X16_set_data_pins_to_output();
		//Initialisation de toutes les broches à 0
		LCD2X16_set_all_pins_to_zero();
		DELAY_MS(40);
		//4 bit mode, 1 line
		LCD2X16_config_four_bit_mode_one_line();
		//Clear the display
		LCD2X16_clear_display();
		//Return home
		LCD2X16_return_home();
		//Display on, no cursor
		LCD2X16_config_display_on_no_cursor();
		DELAY_MS(40);
		//Print init data : H (72 hexa)
		LCD2X16_write_data(0,0,1,0,0,0,0,1);
		//Print init data : I (73 hexa)
		LCD2X16_write_data(0,0,1,0,1,0,0,1);
		DELAY_MS(500); //L'init ne semble pas fonctionner sinon...
}

/***************************CONFIGURATION FUNCTIONS**********************/
void LCD2X16_config_four_bit_mode_one_line(void){
	LCD2X16_send_config_command(0,1,0,0);
}

void LCD2X16_config_display_on_no_cursor(void){
	LCD2X16_send_config_command(0,0,0,0);
	LCD2X16_send_config_command(0,0,1,1);
}

/***************************USER ACTIVE FUNCTIONS**********************/
void LCD2X16_clear_display(void){
	LCD2X16_send_config_command(0,0,0,0);
	LCD2X16_send_config_command(1,0,0,0); //D0-D3 via D4_D7
}

void LCD2X16_return_home(void){
	LCD2X16_send_config_command(0,0,0,0);
	LCD2X16_send_config_command(0,1,0,0);
}


/*************************STM32 LIBRARY**************************/

/** Fonction d'affichage d'une chaine formatée (cf printf)
 * 	@param  premier paramètre = chaine de format
 * 	@param  paramètres suivants = selon les balises %x indiquées dans la chaîne de format
 * 	@pre 	La chaine fabriquée ne DOIT PAS excéder 20 caractères (et en pratique 16 caractères pour un écran de 16 caractères par lignes).
 *  @pre     l'usage de cette fonction doit être précédé au moins une fois de l'appel de LCD2X16_init()
 * 	@post	Si la chaîne fabriquée excède 20 caractères, un message est affiché via printf.
 */
void	LCD2X16_printf(const char *__restrict string, ...)
{
	LCD2X16_putChar('\f'); //Sans cette ligne des disfonctionnement apparaissent...
	LCD2X16_clear_display();
	LCD2X16_return_home();
	DELAY_MS(20);
	char buffer[20];
	uint8_t i;
	uint8_t size;
	va_list args_list;
	va_start(args_list, string);
	size = vsnprintf(buffer, 20, string, args_list);
	if(size>21)
		printf("ATTENTION, chaine LCD trop grande !\n");
	va_end(args_list);
	for(i=0;i<size;i++)
		LCD2X16_putChar(buffer[i]);
}

/**
 * Fonction d'envoi d'un quartet sur le LCD
 *
 *    @param n le quartet à envoyer (les 4 bits de poids faible de n)
 *    @pre     le port de données doit être configuré en écriture
 */
static void LCD2X16_sendNibble(unsigned char n)
{
	GPIO_WRITE(PIN_DATA_0, (n   )&0x01);
	GPIO_WRITE(PIN_DATA_1, (n>>1)&0x01);
	GPIO_WRITE(PIN_DATA_2, (n>>2)&0x01);
	GPIO_WRITE(PIN_DATA_3, (n>>3)&0x01);

	LCD2X16_validate_data();
}

/**
 * Fonction d'envoi d'un octet sur le LCD
 *
 *    @param rs   numéro du registre du LCD (0 ou 1)
 *    @param n    octet a envoyer
 *    @pre        le port de données doit être configuré en écriture
 */
static void LCD2X16_sendByte( unsigned char rs, unsigned char n)
{
	uint8_t nb_try = 3;
   while((LCD2X16_getByte(0) & 0b10000000) != 0 && nb_try--);

   if(nb_try)
   {
	   GPIO_WRITE(PIN_RS, (rs)?1:0);
	   GPIO_WRITE(PIN_RW, 0);
	   DELAY_MS(0.1);

	   LCD2X16_sendNibble(n >> 4);
	   LCD2X16_sendNibble(n & 0xf);
   }
}

/** Fonction d'écriture d'un caractère sur le LCD. Cette fonction affiche un caractère ascii sur l'afficheur
 *  Il existe des caractères de contrôle :
 *     - \f  efface l'écran
 *     - \n  saut au début de la seconde ligne
 *     - \b  retour arrière d'un caractère
 *
 *     @param c le caractêre à afficher
 *     @pre     l'usage de cette fonction doit être précédé au moins une fois de l'appel à LCD2X16_init().
 *     @post    les bits du port de données sont configurés en écriture.
 *  */
void LCD2X16_putChar(char c)
{
   switch (c)
   {
      case '\f':  // Effacer l'écran
         LCD2X16_sendByte(0,1);
         DELAY_MS(10);
         break;

      case '\n':  // Passer à la ligne suivante
         LCD2X16_setCursor(1,2);
         break;

     case '\b':   // Retour au caractère précédent
         LCD2X16_sendByte(0,0x10);
         break;

     default:     // Caractère affichable
         LCD2X16_sendByte(1,c);
   }
}

/** Fonction de positionnement du curseur sur le LCD
 *
 *    @param column indice de colonne (1 à 16)
 *    @param line indice de ligne (1 ou 2)
 *    @pre     l'usage de cette fonction doit être précédé au moins une fois de l'appel de LCD2X16_init()
 */
void LCD2X16_setCursor( unsigned char column, unsigned char line)
{
   unsigned char address;

   // Calcul de l'adresse correspondant aux coordonnées
   if(line!=1)
     address=LCD2X16_LINE_TWO_AD;
   else
     address=0;

   address+=column-1;

   // Envoi de la commande de positionnement au LCD
   LCD2X16_sendByte(0,0x80|address);
   DELAY_MS(0.2);	//  > 38us
}

/** Fonction de lecture d'un caractère sur le LCD.
 *
 *    @param x indice de colonne du caractère à lire (1 à 16)
 *    @param y indice de ligne du caractère à lire (1 ou 2)
 *    @return  le caractère lu en (x,y)
 *    @pre     l'usage de cette fonction doit être précédé au moins une fois de l'appel de LCD2X16_init()
 */
char LCD2X16_getChar( unsigned char x, unsigned char y)
{
   LCD2X16_setCursor(x,y);
   return LCD2X16_getByte(1);
}

/**
 * Fonction de lecture d'un octet sur le LCD
 *
 *    @param  rs   numéro du registre du LCD (0 ou 1)
 *    @return l'octet lu
 *    @post   les bits du port de données sont configurés en écriture.
 */
static uint8_t LCD2X16_getByte(uint8_t rs)
{
	uint8_t read;

	GPIO_WRITE(PIN_RS, (rs)?1:0);

	// configuration de l'accès aux données en lecture
	GPIO_SET_INPUT(PIN_DATA_0);
	GPIO_SET_INPUT(PIN_DATA_1);
	GPIO_SET_INPUT(PIN_DATA_2);
	GPIO_SET_INPUT(PIN_DATA_3);

	GPIO_WRITE(PIN_RW, 1);
	DELAY_MS(0.001);

	GPIO_WRITE(PIN_E, 1);
	DELAY_MS(0.01);;

	// Lecture du quartet de poids fort
	read  = (GPIO_READ(PIN_DATA_0)	 << 4) |
			(GPIO_READ(PIN_DATA_1)  << 5) |
			(GPIO_READ(PIN_DATA_2)  << 6) |
			(GPIO_READ(PIN_DATA_3)  << 7);

	GPIO_WRITE(PIN_E, 0);
	DELAY_MS(0.001);

	GPIO_WRITE(PIN_E, 1);
	DELAY_MS(0.01);

	// Lecture du quartet de poids faible
	read |= (GPIO_READ(PIN_DATA_0)  << 0) |
			(GPIO_READ(PIN_DATA_1)  << 1) |
			(GPIO_READ(PIN_DATA_2)  << 2) |
			(GPIO_READ(PIN_DATA_3)  << 3);

	GPIO_WRITE(PIN_E, 0);

	// Configuration de l'accès aux données en écriture
	GPIO_SET_OUTPUT(PIN_DATA_0);
	GPIO_SET_OUTPUT(PIN_DATA_1);
	GPIO_SET_OUTPUT(PIN_DATA_2);
	GPIO_SET_OUTPUT(PIN_DATA_3);

	// Concaténation des deux quartets lus et retour
	return read;
}


#endif
