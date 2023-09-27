/*!
 * @file        main.c
 *
 * @brief       main фунция
 *
 * @version     V1.0.0
 *
 * @date        12-09-2023
 */

#include "main.h"

SPI_T* SPI = SPI1;			/*!< Используемый канал SPI */
USART_T* USART = USART1;	/*!< Используемый канал USART */

/*!
 * @brief     Запуск перефирии: USART, ЦАП, АПЦ, светодиоды, SPI, LCD экран,
 * 			  кнопка KEY1, сторожевой таймер IWDT.
 */

int main(void)
{
	USART_Init();
    DAC_Init();
    ADC_Init();
	APM_MINI_LEDInit(LED2);
	APM_MINI_LEDInit(LED3);
	SPI_Init();
	LCD_Start();
	APM_MINI_PBInit(BUTTON_KEY1, BUTTON_MODE_EINT);
	//IWDT_init();
	while (1) {} //IWDT_Update();
}

/*! @mainpage 
* Программа содержит слежующие компоненты:
* 
*/
