#include "PM.h"

#define TICKS_NUM							 			(5376) 

//#define DEBUG

double TimerFreq = 26785.714285;						//Частота таймера TIM2
double TimerPeriod = 1/26785.714285;					//Период прерываний таймера
const double pi = 3.14159265359;
uint16_t PeriodCount = 0;
uint16_t ThroughPeriodCount = 0;
uint16_t PrevCrossTick = 0;
uint8_t SampleCounter = 0;
uint8_t PeriodContinues = 0;
uint8_t FirstPer = 1;


uint8_t Decoding(uint8_t*, uint8_t*);
uint16_t BauerToOrig(uint16_t, uint8_t*);
uint16_t GoodCodeFinder(uint16_t*, uint8_t*);
uint16_t SpeedLimit(uint16_t);

void AnalysisEnd(void);

void ZeroCrossAnalysis(double* VoltagesData, double* ZeroCrossTimings, int8_t StartSign, uint16_t* EndOfSample){
	for (int i = 1; i < TICKS_NUM; i++){
		if ((VoltagesData[i]*StartSign < 0) && (VoltagesData[i-1]*StartSign > 0)) {
			if((i - PrevCrossTick) == 8) {
				ZeroCrossTimings[ThroughPeriodCount] = ((double) i - 1.0 + 0.5)*TimerPeriod;
				ThroughPeriodCount++;
				if (PeriodContinues == 0) {
					if (PeriodCount > 5) {
						if (!FirstPer) {
							SampleCounter++;
						} 
						else FirstPer = 0;
						if (SampleCounter > 43) {
							AnalysisEnd();
							return;
						}
						EndOfSample[SampleCounter] = ThroughPeriodCount;	
						PeriodContinues = 1;
						PeriodCount = 0;
					}	
					else {
						ThroughPeriodCount -= PeriodCount;
						PeriodCount = 0;
						PeriodContinues = 1;
					}
				} 
				else {
					if (PeriodCount == 15) {
						PeriodContinues = 0;
					}
				}
				PeriodCount++;
			} 
			else PeriodContinues = 0;
			
			PrevCrossTick = i;
		}
		
		if (ThroughPeriodCount > 700) {
		AnalysisEnd();
		return;
		}
	}
	SampleCounter++;
	EndOfSample[SampleCounter] = ThroughPeriodCount;
	
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
	#endif

	
	AnalysisEnd();
	//
	return;
}

void AnalysisEnd() {
	PeriodContinues = 1;
	PeriodCount = 0;
	ThroughPeriodCount = 0;
	SampleCounter = 0;
	PrevCrossTick = 0;
	FirstPer = 1;
}

uint8_t Demodulation(double* ZeroCrossTimings, uint16_t* EndOfSample, uint8_t* High, uint8_t* Low) {
	double DiffPhaze[41] = {0};
	double T = 1/3348.21;
	double sum = 0; 
	double phase = 0;
	double offsetTime = 0;
	double currentPhase = 0.0;
	
	uint8_t ExitCode = 0;
	TMR_Disable(TMR2);
	uint8_t HighBinData[40] = {0};
	uint8_t LowBinData[40] = {0};
	USART_Tx_Char(USART, 13);
	
	for (uint8_t i = 0; i <= EndOfSample[1]; i++) {
		phase = (ZeroCrossTimings[i] - ((int)(ZeroCrossTimings[i]/T))*T)*(2*pi)/T - pi;
		sum+=phase;
	}
	DiffPhaze[0] = sum/28;
	offsetTime = DiffPhaze[0]*T/(2*pi);
	uint32_t pos = 1;
	
	
	for (uint16_t i = 2; i < 42; i++) {
		sum = 0;
		uint8_t Phase_0pi = 0;
		uint8_t Phase_pi_2 = 0;
		uint8_t Phase_pi = 0;
		uint8_t Phase_3pi_2 = 0;
		
		if (EndOfSample[i-1]+1 > EndOfSample[i]) break;

		for (uint16_t j = EndOfSample[i-1]+1; j<=EndOfSample[i]; j++){
			currentPhase = (ZeroCrossTimings[j] - offsetTime);
			phase = fmod(currentPhase, T)*(2*pi)/T - pi;
			
			if (phase < 0) phase+=2*pi;
			if ((phase < pi/4) || (phase >= 7*pi/4)) Phase_0pi++;
			else if ((phase >= pi/4) && (phase < 3*pi/4)) Phase_pi_2++;
			else if ((phase >= 3*pi/4) && (phase < 5*pi/4)) Phase_pi++;
			else Phase_3pi_2++;
		}
		
		if (Phase_0pi >= Phase_pi_2 && Phase_0pi > Phase_pi && Phase_0pi >= Phase_3pi_2) {
			DiffPhaze[pos] = 0;
			HighBinData[pos-1] = 0;
			LowBinData[pos-1] = 0;
		} else if (Phase_pi_2 > Phase_0pi && Phase_pi_2 >= Phase_pi && Phase_pi_2 > Phase_3pi_2) {
			DiffPhaze[pos] = pi/2;
			HighBinData[pos-1] = 1;
			LowBinData[pos-1] = 0;
		} else if (Phase_pi > Phase_0pi && Phase_pi > Phase_pi_2 && Phase_pi >= Phase_3pi_2) {
			DiffPhaze[pos] = pi;
			HighBinData[pos-1] = 1;
			LowBinData[pos-1] = 1;
		} else if (Phase_3pi_2 > Phase_0pi && Phase_3pi_2 > Phase_pi_2 && Phase_3pi_2 > Phase_pi) {
			DiffPhaze[pos] = 3*pi/2;
			HighBinData[pos-1] = 0;
			LowBinData[pos-1] = 1;
		} 
		
		offsetTime += DiffPhaze[pos]*T/(2*pi);
		pos++;
	}
	
	#ifdef DEBUG
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
	//while(1);
	#endif

	
	ExitCode = Decoding(HighBinData, LowBinData);
	
	for (uint8_t i = 0; i < 40; i++) {
		High[i] = HighBinData[i];
		Low[i] = LowBinData[i];
	}
	
	return ExitCode;
}
 
uint8_t ErrorMessage[3] = {'B', 'A', 'D'};

uint16_t CodeSearch(uint8_t* High, uint8_t* Low, uint8_t* BadState, uint8_t* Counter) {
	uint8_t counter = 0;
	uint16_t CurrCode = 0;
	uint16_t GoodCode = 0;
	uint8_t BadCode = 0;
	
	for (int i=0; i < 5; i++){
		uint16_t HighEncodedHex = 0;
		uint16_t HighDecodedHex = 0;
		uint16_t LowEncodedHex = 0;
		uint16_t LowDecodedHex = 0;
		uint16_t Power = 128;
		for (int j = 0; j < 8; j++) {
			HighEncodedHex += High[i*8 + j]*Power;
			LowEncodedHex += Low[i*8 + j]*Power;
			Power /= 2;
		}
		HighDecodedHex = BauerToOrig(HighEncodedHex, &BadCode);
		LowDecodedHex = BauerToOrig(LowEncodedHex, &BadCode);
		if (!BadCode) {
			CurrCode = HighDecodedHex * 0x10 + LowDecodedHex;
			if (!counter) {
				GoodCode = CurrCode;
				counter = 1;
			}
			else if (CurrCode == GoodCode) {
				GoodCode = CurrCode;
				counter++;
			}
		} else BadCode = 0;
	}
	if (counter < 2) *BadState = 1;
	else *BadState = 0;
	*Counter = counter;
	return GoodCode;
}

void ArrayShift(uint8_t* High, uint8_t* Low, uint8_t* NewHigh, uint8_t* NewLow, uint16_t Length, uint16_t Shift) {
	for (int16_t i = Length-1; i >= 0; i--) {
		if (i - Shift >= 0) {
			NewHigh[i] = High[i-Shift];
			NewLow[i] = Low[i-Shift];
		} else {
			NewHigh[i] = 2;
			NewLow[i] = 2;
		}
	}
}

uint8_t Decoding(uint8_t* High, uint8_t* Low){
	
	for (uint16_t i = 0; i < 8; i++) {
		uint8_t BadState = 0;
		uint8_t Counter = 0;
		uint16_t Code = 0;
		uint8_t HighData[40] = {0};
		uint8_t LowData[40] = {0};
		ArrayShift(High, Low, HighData, LowData, 40, i);
		Code = CodeSearch(HighData, LowData, &BadState, &Counter);
		
		if (!BadState) {
			
			uint16_t GoodCode = 0;
			GoodCode = Code/0x10 + (Code%0x10)*0x10;
			if ((GoodCode / 0x10) < 10) USART_Tx_Char(USART, '0' + GoodCode / 0x10);
			else USART_Tx_Char(USART, 'A' + (GoodCode / 0x10) - 0x0A);
			if ((GoodCode % 0x10) < 10) USART_Tx_Char(USART, '0' + GoodCode % 0x10);
			else USART_Tx_Char(USART, 'A' + (GoodCode % 0x10) - 0x0A);	
			USART_Tx_Char(USART, ' ');
			USART_Tx_Char(USART, Counter + '0');			
			uint16_t speedCode = SpeedLimit(GoodCode);
			
			LCD_DQPSK_Result(GoodCode, speedCode, AverageVoltage);
			
			return 0;
		}
	}
	
	USART_Write(USART, ErrorMessage, 3);
	USART_Tx_Char(USART, ' ');
	return 1;
}

uint16_t GoodCodeFinder(uint16_t* Codes, uint8_t* counter) {
	uint8_t RepitCounter = 0;
	uint16_t ProbableCode = 0;
	for (int i = 0; i < *counter; i++) {
		uint8_t CurrentCouner = 0;
		for (int j = 0; j < *counter; j++){
			if (Codes[i] == Codes[j]) CurrentCouner++;
		}
		if (CurrentCouner > RepitCounter) {
			RepitCounter = CurrentCouner;
			ProbableCode = Codes[i];
		}
	}
	*counter = RepitCounter;
	return ProbableCode;
}

uint16_t BauerToOrig(uint16_t data, uint8_t* ErrorVar) {
	switch (data) {
		case 0x01:
			return 0x00;
		case 0x1f:
			return 0x01;
		case 0x2c:
			return 0x02;
		case 0x32:
			return 0x03;
		case 0x4a:
			return 0x04;
		case 0x54:
			return 0x05;
		case 0x67:
			return 0x06;
		case 0x79:
			return 0x07;
		case 0x86:
			return 0x08;
		case 0x98:
			return 0x09;
		case 0xab:
			return 0x0a;
		case 0xb5:
			return 0x0b;
		case 0xcd:
			return 0x0c;
		case 0xd3:
			return 0x0d;
		case 0xe0:
			return 0x0e;
		case 0xfe:
			return 0x0f;
		default:
			*ErrorVar = 1;
			return 0xff;
	}
}

uint16_t SpeedLimit(uint16_t DQPSK_code) {
	
	uint16_t speedCode = DQPSK_code%0x40;
	
	switch (speedCode) {
		
		case 0x00: return 9999;
		case 0x01: return 8080;
		case 0x02: return 8070;
		case 0x03: return 8065;
		case 0x04: return 8060;
		case 0x05: return 7070;
		case 0x06: return 7065;
		case 0x07: return 7060;
		case 0x08: return 7055;
		case 0x09: return 7050;
		case 0x0A: return 6565;
		case 0x0B: return 6560;
		case 0x0C: return 6555;
		case 0x0D: return 6550;
		case 0x0E: return 6545;
		case 0x0F: return 6540;
		case 0x10: return 6500;
		case 0x11: return 6060;
		case 0x12: return 6055;
		case 0x13: return 6050;
		case 0x14: return 6045;
		case 0x15: return 6040;
		case 0x16: return 6000;
		case 0x17: return 5555;
		case 0x18: return 5550;
		case 0x19: return 5545;
		case 0x1A: return 5540;
		case 0x1B: return 5535;
		case 0x1C: return 5500;
		case 0x1D: return 5050;
		case 0x1E: return 5045;
		case 0x1F: return 5040;
		case 0x20: return 5035;
		case 0x21: return 5020;
		case 0x22: return 5000;
		case 0x23: return 4545;
		case 0x24: return 4540;
		case 0x25: return 4535;
		case 0x26: return 4520;
		case 0x27: return 4500;
		case 0x28: return 4040;
		case 0x29: return 4035;
		case 0x2A: return 4020;
		case 0x2B: return 4000;
		case 0x2C: return 3535;
		case 0x2D: return 3520;
		case 0x2E: return 3515;
		case 0x2F: return 3500;
		case 0x30: return 2020;
		case 0x31: return 2015;
		case 0x32: return 2000;
		case 0x33: return 1515;
		case 0x34: return 1500;
		case 0x35: return 0000;
		case 0x36: return 3535;
		case 0x37: return 3520;
		case 0x38: return 3515;
		case 0x39: return 3500;
		case 0x3A: return 2020;
		case 0x3B: return 2015;
		case 0x3C: return 2000;
		case 0x3D: return 1500;
		case 0x3E: return 1505;
		case 0x3F: return 0500;
		default:   return 7777;
		
	}
}
