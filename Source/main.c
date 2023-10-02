/*!
 * @file        main.c
 *
 * @brief       main фунция
 *
 * @version     V1.0.0
 *
 * @date        02-10-2023
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
* Устройство по приёму АРС/АРСФ кодов работает следующим образом:
* 1. Инифиализация используемой перефирии:
*		- #USART_Init
*		- #DAC_Init
*		- #ADC_Init
*		- #SPI_Init
*		- #LCD_Start
* 2. По кнопке KEY1 выбирается текущий режим работы (см. #KEY1_IRQHandler)
* 3. Запускается соответсвующий таймер (#TMR2_Init для АРСФ, #TMR3_Init для АРС),
*    в прерывании таймера выполняется приём пакетов/сигнала
* 4. Полученные данные обрабатываются (#Demodulation для АРСФ, #SpectrumAnalysis для АРС)
* 5. Результат выводится на LCD экран (#LCD_DQPSK_Result для АРСФ, #LCD_Freq_Result для АРС)
* 
*/
