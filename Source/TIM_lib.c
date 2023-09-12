#include "TIM_lib.h"

//#define DEBUG
//#define FILTER_TEST

uint8_t FalseActivation = 0;
uint8_t LowLevel = 1;
uint16_t adcData = 0;
uint16_t fft_pos = 0;
uint16_t EndOfSample[8*5+2+1] = {0};
uint64_t tick = 0;

#ifdef DEBUG
double VoltageTestArray[6000] = {0};
uint32_t c = 0;
#endif
#ifdef FILTER_TEST
int16_t FilterOutputArray[20000] = {0};
uint32_t q = 0;
#endif

double time = 0;
double ZeroCrossTimings[700] = {0};
double VoltagesData[(32+5*8*16)*8] = {0};
double OrigVoltage[(32+5*8*16)*8] = {0};
double adcVoltage = 0;
double VirtualZero = INPUT_AMPLITUDE/2;	
double RealVoltage = 0;
double PrevVoltage = 0;
double FFT_Buff[FFT_N] = {0};

void VarReInit(void);
void SpectrumVarReInit(void);

void TMR2_Init() {
    TMR_BaseConfig_T TMR_BaseConfigStruct;

    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR2);
    
    TMR_BaseConfigStruct.clockDivision = TMR_CLOCK_DIV_1;
    TMR_BaseConfigStruct.countMode = TMR_COUNTER_MODE_UP;
    TMR_BaseConfigStruct.division = 1568-1;
    TMR_BaseConfigStruct.period = 1;
    TMR_BaseConfigStruct.repetitionCounter = 0;
    TMR_ConfigTimeBase(TMR2, &TMR_BaseConfigStruct);

    TMR_EnableInterrupt(TMR2, TMR_INT_UPDATE);
    NVIC_EnableIRQRequest(TMR2_IRQn, 1, 0);
			
    TMR_Enable(TMR2);
}

void TMR3_Init() {
	
    TMR_BaseConfig_T TMR_BaseConfigStruct;

    /** Enable TMR3 Periph Clock */
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR3);
    
    /** Config TMR3 */
    TMR_BaseConfigStruct.clockDivision = TMR_CLOCK_DIV_1;
    TMR_BaseConfigStruct.countMode = TMR_COUNTER_MODE_UP;
    TMR_BaseConfigStruct.division = 6563-1;
    TMR_BaseConfigStruct.period = 1;
    TMR_BaseConfigStruct.repetitionCounter = 0;
    TMR_ConfigTimeBase(TMR3, &TMR_BaseConfigStruct);

    /** Enable TMR3 Interrupt */
    TMR_EnableInterrupt(TMR3, TMR_INT_UPDATE);
    NVIC_EnableIRQRequest(TMR3_IRQn, 1, 0);

    TMR_Enable(TMR3);
}

uint8_t NoResultCounter = 2;

void TMR4_Init() {
	
    TMR_BaseConfig_T TMR_BaseConfigStruct;

    /** Enable TMR3 Periph Clock */
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR4);
    
    /** Config TMR3 */
    TMR_BaseConfigStruct.clockDivision = TMR_CLOCK_DIV_1;
    TMR_BaseConfigStruct.countMode = TMR_COUNTER_MODE_UP;
    TMR_BaseConfigStruct.division = 42000-1;
    TMR_BaseConfigStruct.period = 1000;
    TMR_BaseConfigStruct.repetitionCounter = 0;
    TMR_ConfigTimeBase(TMR4, &TMR_BaseConfigStruct);

    /** Enable TMR3 Interrupt */
    TMR_EnableInterrupt(TMR4, TMR_INT_UPDATE);
    NVIC_EnableIRQRequest(TMR4_IRQn, 1, 0);

	NoResultCounter = 2;
    TMR_Enable(TMR4);
}

void TMR2_EventCallback(void) {
	if(TMR_ReadIntFlag(TMR2, TMR_INT_UPDATE) == SET) {		
		if (ADC_ReadStatusFlag(ADC1, ADC_FLAG_EOC)) {
			RealVoltage = GetRealVoltage();
			OrigVoltage[tick] = RealVoltage;
			RealVoltage = HighPassFilter(RealVoltage);
			
			#ifdef FILTER_TEST
				if (tick < 20000) FilterOutputArray[tick] = (int) RealVoltage;
				else {
					for (int i = 0; i < 20000; i++) USART_Tx_Number(USART, FilterOutputArray[i]);
					while(1);
				}
				tick++;
				FlagsClear();
				return;
			#endif
			
			if (!ReadyToStartReceive(RealVoltage))
			{
				FlagsClear();
				return;
			}
			
			if (!CatchPackageStart(RealVoltage, PrevVoltage, &tick))
			{
				FlagsClear();
				return;
			}
						
			#ifdef DEBUG			
				if (c < 6000) {
					VoltageTestArray[c] = RealVoltage;
					c++;
				}
			#endif
			
			VoltagesData[tick] = RealVoltage;
			FasleActivationCatch(RealVoltage, PrevVoltage, &FalseActivation);
			PrevVoltage = RealVoltage;
			tick++;
		}
		
		FlagsClear();
		
		if (FalseActivation) {
			
			#ifdef DEBUG
				for(int i = 0; i < c; i++) {
					USART_Tx_Number(USART, (int) VoltageTestArray[i]);
				}
				USART_Tx_Char(USART, 13);
			#endif
			
			VarReInit();
			return;
		}
		
		if (tick == TICKS_NUM) {
			__disable_irq();
			AverageVoltage = AmlitudeAnalysis(OrigVoltage, 5000);
			ReceiveStop(VoltagesData, ZeroCrossTimings, EndOfSample);
			__enable_irq();
		}
   }
 }

 void TMR3_EventCallback(void) {
	 if(TMR_ReadIntFlag(TMR3, TMR_INT_UPDATE) == SET) {
		 if (ADC_ReadStatusFlag(ADC1, ADC_FLAG_EOC)) {
			 RealVoltage = GetRealVoltage();
			 if (LowLevel && (RealVoltage > 500)) LowLevel = 0;
			 FFT_Buff[fft_pos] = RealVoltage;
			 fft_pos++;
			 if (fft_pos == FFT_N) SpectrumAnalysis(FFT_Buff, LowLevel);
		 }
	 }
	 FlagsClear();
 }
 
 
 
 void TMR4_EventCallback(void) {
	 if(TMR_ReadIntFlag(TMR4, TMR_INT_UPDATE) == SET) {
		 if (NoResultCounter == 0) {
			 MovingAverageClear();
			 LCD_Result_Reset();
			 LCD_Show_No_Result();
		 } else NoResultCounter--;
		 TMR_ClearIntFlag(TMR4, TMR_INT_UPDATE);
	 }
 }
 
 void SpectrumVarReInit() {
	 for (uint16_t i = 0; i < FFT_N; i++) FFT_Buff[i] = 0;
	 fft_pos = 0;
	 LowLevel = 1;
 }

void VarReInit() {
	tick = 0;
	PrevVoltage = 0;
	FalseActivation = 0;
	for (int i = 0; i < 600; i++){
		ZeroCrossTimings[i] = 0;
	}
	for (int i = 0; i < 42; i++){
		EndOfSample[i] = 0;
	}
	for (int i = 0; i < 5376; i++) {
		VoltagesData[i] = 0;
		OrigVoltage[i] = 0;
	}
	HandlerVarReInit();
	#ifdef DEBUG
		USART_Tx_Char(USART, 13);
		for(int i = 0; i < c; i++) {
			USART_Tx_Number(USART, (int) VoltageTestArray[i]);
		}
		USART_Tx_Char(USART, 13);
		for(int i = 0; i < c; i++) {
			VoltageTestArray[i] = 0;
		}
		c = 0;
	#endif
}

