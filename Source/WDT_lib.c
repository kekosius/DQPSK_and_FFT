/*!
 * @file        WDT_lib.c
 *
 * @brief       Функции для работы со сторожевым таймером IWDT
 *
 * @version     V1.0.0
 *
 * @date        12-09-2023
 */

#include "WDT_lib.h"

volatile uint32_t TimingDelay = 0;		 /*!< extern переменная, используется для задержки по сторожевому таймеру.
										  *
										  * TimingDelay уменьшается системным таймером в [SysTick_Handler()](#SysTick_Handler)
										  */

void IWDT_Restart_note(void);

/*! Инициализация сторожевого таймера IWDT
*
* При отсутствии обновления в течении ~1000 ms инициируется перезагрузка системы
*/

void IWDT_init() {
	SysTick_Config(16000000/1000);			//1 тик ~= 1 ms
	IWDT_Restart_note();
	IWDT_EnableWriteAccess();
	IWDT_ConfigDivider(IWDT_DIVIDER_32);
	IWDT_ConfigReload(1000);
	IWDT_Refresh();
	IWDT_Enable();
}

/*! Задержка между обновлениями сторожевого таймера*/

void IWDT_Delay(uint32_t ms) {
	TimingDelay = 0;
    while(TimingDelay < ms);
}

/*! Обновление сторожевого таймера через каждые 300 ms*/

void IWDT_Update() {
	IWDT_Delay(300);
	IWDT_Refresh();
}

/*!
 * @brief     Отправка сообщения по USART о перезагрузке системы, вызванной IWDT таймером
 */

void IWDT_Restart_note() {
	if (RCM_ReadStatusFlag(RCM_FLAG_IWDTRST) == SET) {
		uint8_t restart_message[16] = {'W', 'D', 'T', ' ', 'S', 'Y', 'S', 'T', 'E', 'M', ' ', 'R', 'E', 'S', 'E', 'T'};
		USART_Write(USART, restart_message, 16);
		USART_Tx_Char(USART, 13);
		RCM_ClearStatusFlag();	
	}
}

/*!
 * @brief     Принудительная остановка работы программы.
 *
 * Устройство сигнализирует о принудительной остановке следующим образом:
 * - Включение светодиодов LED2, LED3
 * - Заполнение LCD экрана красным цветом
 * - Отправка сообщения SYSTEM FATAL ERROR по USART
 * - Установка High уровня на ЦАП
 */

void Fatal_error() {
	APM_MINI_LEDOn(LED2);
	APM_MINI_LEDOn(LED3);
	fillRectangle(0, 0, LCD_HEIGHT, LCD_WIDTH, RED);
	USART_Tx_Char(USART, 13);
	uint8_t fatal_message[18] = {'S', 'Y', 'S', 'T', 'E', 'M', ' ', 'F', 'A', 'T', 'A', 'L', ' ', 'E', 'R', 'R', 'O', 'R'};
	USART_Write(USART, fatal_message, 18);
	USART_Tx_Char(USART, 13);
	DAC_SetHigh();
	while(1) {};
}
