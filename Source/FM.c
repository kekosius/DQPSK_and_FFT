/*!
 * @file        FM.c
 *
 * @brief       Обработка АРС сигнала
 *
 * @version     V1.0.0
 *
 * @date        02-10-2023
 */

#include "FM.h"

//#define DEBUG			//Раскомментировать для отправки по USART информации о принятом сигнале в виде значений напряжения

/*!
 * @brief     Спектральный и амплитудный анализ АРС сигнала
 *
 * Функция обеспечивает анализ АРС сигнала и корректное завершение приёма
 *
 * Принцип работы функции следующий: 
 * - Переключение светодиодов (LED2 - off, LED3 - on), выставляется High напряжение
 *   на ЦАП (для визуального и сигнального информирования о состоянии системы)
 * - Амплитудный анализ полученного сигнала (см. #AmlitudeAnalysis)
 * - Быстрое преобразование Фурье (спектральный анализ). БПФ реализована при помощи 
 *   MATLAB Coder, исходные файлы содержит библиотека FFT_lib
 * - Отправка результатов спектрального анализа по USART и на LCD (см. #LCD_Freq_Result)
 * - Обнуление (возврат к значениям при инициализации) всех переменных и 
 *   переинициализация используемой перефирии
 * - Отключение LED3, выставляется Low напряжение на ЦАП
 */

void SpectrumAnalysis(double* FFT_Buff, uint8_t LowLevel, double* voltageBuff) {
	double freqPerc[6] = {0};
	APM_MINI_LEDOff(LED2);
	APM_MINI_LEDOn(LED3);
	TMR_Disable(TMR3);
	DAC_SetHigh();
	 
	/*
	Параметры, использующийеся при выполнении Быстрого Преобразования Фурье (FFT)
	uint16_t F = 6400;							частота дискретизации
	uint16_t N = 256;							количество измерений, использующихся при анализе
	double T = 0.04; T = N/F = 256/6400			время сбора данных для анализа
	double dt = 0.00015625; dt = 1/F = 1/6400	время одного измерения				
	double Nyq = 3200; Nyq = N/(2*T)			частота найквиста
	double df = 25; df = 1/T					шаг шкалы выходных значений в Гц
											    (т.е выходные амплитуды будут соответсвовать
												значениям частот 0-df-2df-...-Nyq)
	*/	 
	 
	 AverageVoltage = AmlitudeAnalysis(voltageBuff, VOL_BUF_LEN);
	 
	 fftM(FFT_Buff, freqPerc);
	 
	 USART_Tx_Char(USART, 13);
	 USART_Tx_Char(USART, ' ');
	 
	 uint16_t result = 0;
	 
	 for (int i = 0; i < 6; i++) {
		 if (freqPerc[i] < 0.4 || LowLevel) freqPerc[i] = 0;
		 else if (freqPerc[i] > 0.85) {
			 freqPerc[i] = 1;
			 result = 75+i*50;
		 }
		 USART_Tx_Specrum_Result(USART, freqPerc[i]*100, 75+i*50);
	 }
	 
	 
	 if (result > 0) LCD_Freq_Result(result, AverageVoltage);
	 
	 #ifdef DEBUG
	 for (int i = 0; i < VOL_BUF_LEN; i++) USART_Tx_Float(USART, voltageBuff[i], 3);
	 USART_Tx_Char(USART, 13);
	 #endif
	 
	 SpectrumVarReInit();
	 TMR_Reset(TMR3);
	 ADC_Init();
	 TMR3_Init();
	 APM_MINI_LEDOn(LED2);
	 APM_MINI_LEDOff(LED3);
	 DAC_SetLow();	 
}
