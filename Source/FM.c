#include "FM.h"

void SpectrumAnalysis(double* FFT_Buff, uint8_t LowLevel) {
	double freqPerc[6] = {0};
	APM_MINI_LEDOn(LED3);
	TMR_Disable(TMR3);
	__disable_irq();
	DAC_ConfigChannel1Data(DAC_ALIGN_12BIT_R, 0x0);
	 
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
	 
	 AverageVoltage = AmlitudeAnalysis(FFT_Buff, 256);
	 
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
	 
	 DAC_ConfigChannel1Data(DAC_ALIGN_12BIT_R, 0x0FFF);
	 SpectrumVarReInit();
	 TMR_Reset(TMR3);
	 ADC_Init();
	 TMR3_Init();
	 APM_MINI_LEDOff(LED3);	 
	 __enable_irq();
}
