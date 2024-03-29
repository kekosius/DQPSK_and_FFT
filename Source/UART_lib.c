/*!
 * @file        UART_lib.c
 *
 * @brief       Функции для работы с USART
 *
 * @version     V1.0.0
 *
 * @date        02-10-2023
 */


#include "UART_lib.h"
#include "math.h"

uint8_t errorCounterUSART = 0;			/*!< Количество ошибок по USART каналу подряд*/

/*!
 * @brief     Инициализация COM порта
 */

void APM_MINI_COM1_Init (USART_Config_T* configStruct) {
    
    GPIO_Config_T GPIO_configStruct;
    GPIO_ConfigStructInit(&GPIO_configStruct);
    
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_USART1);
    
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_9, GPIO_AF_USART1);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_10, GPIO_AF_USART1);
    
    GPIO_configStruct.mode = GPIO_MODE_AF;
    GPIO_configStruct.pin = GPIO_PIN_9;
    GPIO_configStruct.speed = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOA, &GPIO_configStruct);
    
    USART_Config(USART1, configStruct);
    
    GPIO_configStruct.mode = GPIO_MODE_AF;
    GPIO_configStruct.pin = GPIO_PIN_10;
    GPIO_Config(GPIOA, &GPIO_configStruct);
    
    USART_Config(USART1, configStruct);
    USART_Enable(USART1);
}

/*!
 * @brief     Задержка выполнения основной программы
 *
 * @param	  Количество простейших вычислений, которые будут проведены во время задержки
 */

void Delay(uint32_t count){
    volatile uint32_t delay = count;
    while(delay--);
}

/*!
* @brief     Инициализация USART канала
 *
 * Выставленные параметры:
 * - Baud rate -		115200
 * - Parity -			None
 * - Stop Bit -			1
 * - Data Bits -		8
 * - Выбранный канал - USART1, TX - PA9, RX - PA10
 * - Включено прерывание по ошибке, при прерывании вызывается [USART_Reload()](#USART_Reload)
 */

void USART_Init(void) {
    USART_Config_T usartConfigStruct;

    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    usartConfigStruct.mode = USART_MODE_TX_RX;
    usartConfigStruct.parity = USART_PARITY_NONE;
    usartConfigStruct.stopBits = USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;

    APM_MINI_COM1_Init(&usartConfigStruct);

    Delay(0x7FFF);
    
    USART_EnableInterrupt(USART1, USART_INT_ERR);
	USART_ClearStatusFlag(USART1, USART_FLAG_FE);
	USART_ClearStatusFlag(USART1, USART_FLAG_NE);
    USART_ClearStatusFlag(USART1, USART_FLAG_OVRE);
    NVIC_EnableIRQRequest(USART1_IRQn, 0, 0);
}

/*!
 * @brief     Отправка массива char по USART
 *
 * @param     usart ссылка на USART структуру
 * @param     dat массив для отправки
 * @param     count длинна массива
 */

void USART_Write(USART_T* usart,uint8_t *dat, uint32_t count) {
    while(count--){
        while(USART_ReadStatusFlag(usart, USART_FLAG_TXBE) == RESET);
        USART_TxData(usart, *dat++);
    }
}

/*!
 * @brief     Отправка char по USART
 *
 * @param     usart ссылка на USART структуру
 * @param     data char для отправки
 */

void USART_Tx_Char(USART_T* usart, uint8_t data) {
	while(USART_ReadStatusFlag(usart, USART_FLAG_TXBE) == RESET);
	USART_TxData(usart, data);
	errorCounterUSART = 0;
}

/*!
* @brief     Отправка int по USART без пробела в конце
 *
 * @param     usart ссылка на USART структуру
 * @param     num int для отправки
 *
 * Отправляет число как совокупность char
 */

void USART_Tx_NumberButNoSpace(USART_T* usart, int64_t num) {
    uint32_t reverse = 0;
    uint8_t zero_trig = 0;
	
	if (num < 0) {
		USART_Tx_Char(usart, '-');
		num *= -1;
	}
	
	if (num == 0) USART_Tx_Char(usart, '0');

    while(num){
        reverse = reverse*10 + num%10;
        if (!(reverse || num%10)) zero_trig++; //Количество нулей на конце числа
        num /= 10;
    }

    while(reverse){
        USART_Tx_Char(usart, reverse%10 + '0');
        reverse /= 10;
    }

    while (zero_trig) {
        USART_Tx_Char(usart, '0');
        zero_trig--;
    }
}

/*!
 * @brief     Отправка int по USART
 *
 * @param     usart ссылка на USART структуру
 * @param     num int для отправки
 *
 * Отправляет число как совокупность char и пробел в конце
 */

void USART_Tx_Number(USART_T* usart, int64_t num) {
    USART_Tx_NumberButNoSpace(usart, num);
    USART_Tx_Char(usart, ' ');
}

/*!
 * @brief     Отправка double по USART
 *
 * @param     usart ссылка на USART структуру
 * @param     num double для отправки
 * @param     accuracy точность, количество знаков после запятой
 *
 * Отправляет число как совокупность char и пробел в конце
 */

void USART_Tx_Float(USART_T* usart, double num, uint16_t accuracy){
	int64_t IntegerPart = (int) num;
	int64_t FloatPart = 0;
	double trace = fabs(num - IntegerPart);
	FloatPart = (int) (pow(10,accuracy)*fabs(num - IntegerPart));
	USART_Tx_NumberButNoSpace(usart, IntegerPart);
	USART_Tx_Char(usart, '.');
	int64_t a = FloatPart;
	uint8_t counter = 0;
	if (a == 0) {
		for (int i = 0; i < accuracy; i++) {
			USART_Tx_Char(usart, '0');
		}
		USART_Tx_Char(usart, ' ');
		return;
	}
	for (int i = 0; i < accuracy; i++) {
		if (a) counter++;
		a /= 10;
	}
	for (int i = 0; i < accuracy-counter; i++) {
		USART_Tx_Char(usart, '0');
	}
	USART_Tx_NumberButNoSpace(usart, FloatPart);
	USART_Tx_Char(usart, ' ');
}

uint8_t SpectrumResultMessageFreq[6] = {'H', 'z', ' ', '-', '>', ' '};

/*!
 * @brief     Отправка результата спектрального анализа по USART
 *
 * @param     usart ссылка на USART структуру
 * @param     Fp амплитуда в процентах
 * @param     freq частота сигнала, которому соответсвует амплитуда
 */

void USART_Tx_Specrum_Result(USART_T* usart, double Fp, double freq) {
	USART_Tx_Float(usart, freq, 0);
	USART_Write(usart, SpectrumResultMessageFreq, 6);
	if (Fp < 10) {
		USART_Tx_Char(usart, ' ');
		USART_Tx_Char(usart, ' ');
	} else if (Fp < 100) {
		USART_Tx_Char(usart, ' ');
	}
	USART_Tx_Float(usart, Fp, 1);
	USART_Tx_Char(usart, '%');
	USART_Tx_Char(usart, 13);
}

/*!
 * @brief     Отправка сообщения по USART о перезагрузке модуля USART
 */

void USART_Restart_note() {
	USART_Tx_Char(USART, 13);
	uint8_t restart_message[17] = {'U', 'S', 'A', 'R', 'T', ' ', 'E', 'R', 'R', 'O', 'R', ' ','R', 'E', 'S', 'E', 'T'};
	USART_Write(USART, restart_message, 17);
	USART_Tx_Char(USART, 13);
}

/*!
* @brief     Перезагрузка USART канала, вызванная обработчиком ошибки
 *
 * Также отправляет информационное сообщение по USART о перезагрузке.
 * Если не удалось отправить char по USART после пяти перезагрузок подряд,
 * то вызывается функция принудительной остановки работы [Fatal_error()](#Fatal_error)
 */

void USART_Reload(USART_T* usart) {
	if ((USART_ReadStatusFlag(usart, USART_FLAG_FE) == SET) || 
		(USART_ReadStatusFlag(usart, USART_FLAG_NE) == SET) || 
		(USART_ReadStatusFlag(usart, USART_FLAG_OVRE) == SET))
	{
		USART_Disable(usart);
		USART_Init();
		errorCounterUSART++;
		USART_Restart_note();
		USART_ClearStatusFlag(usart, USART_FLAG_FE);
		USART_ClearStatusFlag(usart, USART_FLAG_NE);
		USART_ClearStatusFlag(usart, USART_FLAG_OVRE);
	}			
	if (errorCounterUSART >=5) {
		Fatal_error();
	}		
}
