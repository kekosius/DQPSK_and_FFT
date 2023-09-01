#include "SignalReceiveHandler.h"

//#define DEBUG

uint8_t ZeroVoltageCounter = 0;
uint8_t ReceiveStarted = 0;
uint8_t ReceiverReady = 0;
uint8_t firstPeriod = 0;
uint16_t ExitCounter = 0;
int8_t StartSign = 0;

double InterferenceAmplitude = (INPUT_AMPLITUDE/2)*0.5;
double AmlitudeAnalysis(double*, uint16_t);

//Амплитуда колебаний, которая будет считаться как паразитный сигнал (помеха)

struct MinMaxStruct { 
	double max;
	double min;
};
struct MinMaxStruct PeriodAmplitude;

void FlagsClear() {
	TMR_ClearIntFlag(TMR2, TMR_INT_UPDATE);
	TMR_ClearIntFlag(TMR3, TMR_INT_UPDATE);
	ADC_ClearStatusFlag(ADC1, ADC_FLAG_EOC);
}

double TestBuff[256] = {0};

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
	 
//	 USART_Tx_Char(USART1, 13);
//	 for (int i = 0; i < 256; i++) USART_Tx_Number(USART1, FFT_Buff[i]);
	 AverageVoltage = AmlitudeAnalysis(FFT_Buff, 256);
//	 for (int i = 0; i < 256; i++) TestBuff[i] = FFT_Buff[i];
//	 AmlitudeAnalysis(TestBuff, 256);
//	 AmlitudeAnalysis(TestBuff, 256);
	 
	 fftM(FFT_Buff, freqPerc);
	 
	 USART_Tx_Char(USART1, 13);
	 USART_Tx_Char(USART1, ' ');
	 
	 uint16_t result = 0;
	 
	 for (int i = 0; i < 6; i++) {
		 if (freqPerc[i] < 0.4 || LowLevel) freqPerc[i] = 0;
		 else if (freqPerc[i] > 0.85) {
			 freqPerc[i] = 1;
			 result = 75+i*50;
		 }
		 USART_Tx_Specrum_Result(USART1, freqPerc[i]*100, 75+i*50);
	 }
	 
//	 AmlitudeAnalysis(FFT_Buff, 256);
//	 AmlitudeAnalysis(TestBuff, 256);
	 
	 if (result > 0) LCD_Freq_Result(result, AverageVoltage);
	 
	 DAC_ConfigChannel1Data(DAC_ALIGN_12BIT_R, 0x0FFF);
	 SpectrumVarReInit();
	 TMR_Reset(TMR3);
	 ADC_Init();
	 TMR3_Init();
	 APM_MINI_LEDOff(LED3);	 
	 __enable_irq();
}
 
uint8_t ReadyToStartReceive(double RealVoltage) {
	if (ReceiveStarted || ReceiverReady) return 1;
	if (fabs(RealVoltage) > InterferenceAmplitude) {
		ZeroVoltageCounter = 0;
		return 0;
	}
	if (ZeroVoltageCounter > 8*2) {
		ReceiverReady = 1;
		ZeroVoltageCounter = 0;
		PeriodAmplitude.max = 0;
		PeriodAmplitude.min = 0;
		firstPeriod = 1;
		return 1;
	}
	ZeroVoltageCounter++;
	return 0;
}

uint8_t CatchPackageStart(double RealVoltage, double PrevVoltage, uint64_t* tick) {
	if (ReceiveStarted) return 1;
	if (RealVoltage > PrevVoltage) {
		PeriodAmplitude.max = RealVoltage;
	} else PeriodAmplitude.min = RealVoltage;
	
	if ((PeriodAmplitude.max - PeriodAmplitude.min) > InterferenceAmplitude) {
		ReceiveStarted = 1;
		*tick = 0; //8
		PrevVoltage = 0;
		if (RealVoltage > 0) StartSign = 1;
		else StartSign = -1;
		APM_MINI_LEDOn(LED2);
		#ifdef DEEP_DEBUG
		  USART_Tx_Number(USART1, 228);
		#endif
		return 1;
	}
	return 0;
}

void FasleActivationCatch(double RealVoltage, double PrevVoltage, uint8_t* FalseActivation) {
	if (RealVoltage > PrevVoltage) PeriodAmplitude.max = RealVoltage;
	else PeriodAmplitude.min = RealVoltage;

	if (PeriodAmplitude.max - PeriodAmplitude.min < InterferenceAmplitude) ExitCounter++;
	else ExitCounter = 0;

	if (ExitCounter > 8*4) *FalseActivation = 1;
}
 
void FirstPeriodInit(double RealVoltage, double PrevVoltage) {
	if ((RealVoltage*StartSign < 0) && (PrevVoltage*StartSign > 0)) {
	 firstPeriod = 0;
	}
}

void HandlerVarReInit() {
	ReceiverReady = 0;
	ReceiveStarted = 0;
	ExitCounter = 0;
	PeriodAmplitude.max = 0;
	PeriodAmplitude.min = 0;
}

uint8_t GetStartSign() {
	return StartSign;
}
 
double AmlitudeAnalysis(double* Data, uint16_t length) {
	double totalMax = 0;
	double max = 0;
	double min = 0;
	double minSum = 0;
	double maxSum = 0;
	double avg = 0;
	double v = 0;
	uint16_t maxCounter = 0;
	uint16_t minCounter = 0;
	
//	for (uint16_t i = 0; i < length; i++) {
//		v = Data[i]/1000.0;
//		if (v > totalMax) totalMax = v;
//		if (v == 0) continue;
//		if (v > 0 && min < 0) {
//			sum+= max - min;
//			min = 0;
//			max = 0;
//			counter++;
//		}
//		if (v > 0.1 && v > max) max = v;
//		if (v < -0.1 && v < min && max != 0) min = v;
//	}
//	
//	if (counter) avg = sum/counter;

//	for (int i = 0; i < length; i++) {
//		if (fabs(Data[i]) > max) max = fabs(Data[i]);
//	}

	for (uint16_t i = 0; i < length; i++){
		v = Data[i];
		if ((v > 400) && (min < 0)) {
			minSum += min;
			minCounter++;
			min = 0;
		}
		if ((v < -400) && (max > 0)) {
			maxSum += max;
			maxCounter++;
			max = 0;
		}
		if (v < min) min = v;
		if (v > max) max = v;
	}
	
	avg = (maxSum/maxCounter) - (minSum/minCounter);

//	USART_Tx_Char(USART1, 13);
//	USART_Tx_Float(USART1, max, 3);
	USART_Tx_Char(USART1, 13);
	USART_Write(USART1, (uint8_t[5]) {'A', 'V', 'G', ':', ' '}, 5 );
	USART_Tx_Float(USART1, avg, 3);
	USART_Write(USART1, (uint8_t[2]) {'m', 'V'}, 2 );
//	USART_Tx_Char(USART1, 13);
//	USART_Tx_Float(USART1, avg, 3);
	return avg;
}

double AverageVoltage = 0;

void ReceiveStop(double* VoltagesData, double* ZeroCrossTimings, uint16_t* EndOfSample){
	
	#ifdef DEEP_DEBUG
		USART_Tx_Number(USART1, 123);
	#endif
	
	APM_MINI_LEDOff(LED2);
	APM_MINI_LEDOn(LED3);
	
	
	DAC_ConfigChannel1Data(DAC_ALIGN_12BIT_R, 0x0FFF);
	
	int8_t StartSign = GetStartSign();
	ZeroCrossAnalysis(VoltagesData, ZeroCrossTimings, StartSign, EndOfSample);
	
	uint8_t HighBinData[40] = {0};
	uint8_t LowBinData[40] = {0};
	
	if(Demodulation(ZeroCrossTimings, EndOfSample, HighBinData, LowBinData)){
		#ifdef DEBUG
			USART_Tx_Char(USART1, 13);
			for (int i = 0; i < 43; i++) USART_Tx_Number(USART1, EndOfSample[i]);
			USART_Tx_Char(USART1, 13);
			for (int i = 1; i < 43; i++) {
				for (int j = EndOfSample[i-1]+1; j<=EndOfSample[i]; j++) {
					USART_Tx_Float(USART1, ZeroCrossTimings[j], 5);
				}
				USART_Tx_Char(USART1, 13);
			}
			for (int i = 0; i < 40; i++){
				USART_Tx_Number(USART1, HighBinData[i]);
				if ((i+1)%4 == 0) USART_Tx_Char(USART1, ' ');
			}
			USART_Tx_Char(USART1, 13);
			for (int i = 0; i < 40; i++){
				USART_Tx_Number(USART1, LowBinData[i]);
				if ((i+1)%4 == 0) USART_Tx_Char(USART1, ' ');
			}
			USART_Tx_Char(USART1, 13);
			while(1);
		#endif
	}
	
	VarReInit();
	FilterWipe();
	TMR_Reset(TMR2);
	ADC_Init();
	TMR2_Init();
	DAC_ConfigChannel1Data(DAC_ALIGN_12BIT_R, 0x0);
	APM_MINI_LEDOff(LED3);
}
