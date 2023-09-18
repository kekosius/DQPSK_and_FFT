#include "SignalReceiveHandler.h"

//#define DEBUG

uint8_t ZeroVoltageCounter = 0;
uint8_t ReceiveStarted = 0;
uint8_t ReceiverReady = 0;
uint8_t firstPeriod = 0;
uint16_t ExitCounter = 0;
int8_t StartSign = 0;

double InterferenceAmplitude = (INPUT_AMPLITUDE/2)*0.5;

//Амплитуда колебаний, которая будет считаться как паразитный сигнал (помеха)

/*! \cond */

struct MinMaxStruct { 
	double max;
	double min;
};
struct MinMaxStruct PeriodAmplitude;
/*! \endcond */

void FlagsClear() {
	TMR_ClearIntFlag(TMR2, TMR_INT_UPDATE);
	TMR_ClearIntFlag(TMR3, TMR_INT_UPDATE);
	ADC_ClearStatusFlag(ADC1, ADC_FLAG_EOC);
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

void ReceiveStop(double* VoltagesData, double* ZeroCrossTimings, uint16_t* EndOfSample){
	
	APM_MINI_LEDOff(LED2);
	APM_MINI_LEDOn(LED3);
	DAC_SetHigh();
	
	int8_t StartSign = GetStartSign();
	ZeroCrossAnalysis(VoltagesData, ZeroCrossTimings, StartSign, EndOfSample);
	
	uint8_t HighBinData[40] = {0};
	uint8_t LowBinData[40] = {0};
	
	if(Demodulation(ZeroCrossTimings, EndOfSample, HighBinData, LowBinData)){
		#ifdef DEBUG
			USART_Tx_Char(USART, 13);
			for (int i = 0; i < 43; i++) USART_Tx_Number(USART, EndOfSample[i]);
			USART_Tx_Char(USART, 13);
			for (int i = 1; i < 43; i++) {
				for (int j = EndOfSample[i-1]+1; j<=EndOfSample[i]; j++) {
					USART_Tx_Float(USART, ZeroCrossTimings[j], 5);
				}
				USART_Tx_Char(USART, 13);
			}
			for (int i = 0; i < 40; i++){
				USART_Tx_Number(USART, HighBinData[i]);
				if ((i+1)%4 == 0) USART_Tx_Char(USART, ' ');
			}
			USART_Tx_Char(USART, 13);
			for (int i = 0; i < 40; i++){
				USART_Tx_Number(USART, LowBinData[i]);
				if ((i+1)%4 == 0) USART_Tx_Char(USART, ' ');
			}
			USART_Tx_Char(USART, 13);
			while(1);
		#endif
	}
	
	VarReInit();
	FilterWipe();
	TMR_Reset(TMR2);
	ADC_Init();
	TMR2_Init();
	DAC_SetLow();
	APM_MINI_LEDOff(LED3);
}
