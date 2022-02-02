/*
 * object_e_paper.c
 *
 *  Created on: 5 févr. 2021
 *      Author: Emilien
 */


#include "../config.h"
#if OBJECT_ID == OBJECT_E_PAPER
//#include "stm32f1xx_hal.h"
//#include "stm32f1xx_nucleo.h"
//#include "stm32f1_uart.h"
//#include "stm32f1_sys.h"
#include "../common/macro_types.h"
//#include "stm32f1_gpio.h"
#include "../bsp/epaper/epd4in2.h"
#include "../bsp/epaper/epdif.h"
#include "../bsp/epaper/epdpaint.h"
#include "../bsp/epaper/imagedata.h"
#include "../common/systick.h"
#define COLORED      1
#define UNCOLORED    0

int EPAPER_demo(void)
{
	static unsigned char frame_buffer[(EPD_WIDTH * EPD_HEIGHT / 8)];

	EPD epd;
	if (EPD_Init(&epd) != 0)
	{
		printf("e-Paper init failed\n");
		while(1);
	}

	Paint paint;
	Paint_Init(&paint, frame_buffer, epd.width, epd.height);
	Paint_Clear(&paint, UNCOLORED);

	/* Draw something to the frame_buffer */
	/* For simplicity, the arguments are explicit numerical coordinates */
	Paint_DrawRectangle(&paint, 20, 80, 180, 280, COLORED);
	Paint_DrawLine(&paint, 20, 80, 180, 280, COLORED);
	Paint_DrawLine(&paint, 180, 80, 20, 280, COLORED);
	Paint_DrawFilledRectangle(&paint, 200, 80, 360, 280, COLORED);
	Paint_DrawCircle(&paint, 300, 160, 60, UNCOLORED);
	Paint_DrawFilledCircle(&paint, 90, 210, 30, COLORED);

	/*Write strings to the buffer */
	Paint_DrawFilledRectangle(&paint, 0, 6, 400, 30, COLORED);
	Paint_DrawStringAt(&paint, 100, 10, "Hello world!", &Font24, UNCOLORED);
	Paint_DrawStringAt(&paint, 100, 40, "e-Paper Demo", &Font24, COLORED);

	while(1)
	{
		/* Display the frame_buffer */
		EPD_DisplayFrame(&epd, frame_buffer);

//		HAL_Delay(5000);
		SYSTICK_delay_ms(5000);
		/* Display the image buffer */
		EPD_DisplayFrame(&epd, IMAGE_BUTTERFLY);


		SYSTICK_delay_ms(5000);
	}
}

#endif


