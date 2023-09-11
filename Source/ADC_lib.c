#include "ADC_lib.h"

uint8_t errorCounterADC = 0;

void ADC_Init()
{
    GPIO_Config_T   gpioConfig;
    ADC_Config_T    adcConfig;

    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA);
	
    GPIO_ConfigStructInit(&gpioConfig);
    gpioConfig.mode    = GPIO_MODE_AN;
    gpioConfig.pupd    = GPIO_PUPD_NOPULL;
    gpioConfig.pin     = GPIO_PIN_0;
    GPIO_Config(GPIOA, &gpioConfig);

    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_ADC1);

    ADC_Reset();
    ADC_ConfigStructInit(&adcConfig);
    adcConfig.resolution          = ADC_RESOLUTION_12BIT;
    adcConfig.continuousConvMode  = ENABLE;
    adcConfig.dataAlign           = ADC_DATA_ALIGN_RIGHT;
    adcConfig.extTrigEdge         = ADC_EXT_TRIG_EDGE_NONE;
    adcConfig.scanConvMode        = DISABLE;
    ADC_Config(ADC1, &adcConfig);

    ADC_ConfigRegularChannel(ADC1, ADC_CHANNEL_0, 1, ADC_SAMPLETIME_84CYCLES);
	
	ADC_EnableInterrupt(ADC1, ADC_INT_OVR);					//Прерывание по перегрузке

    NVIC_EnableIRQRequest(ADC_IRQn, 1, 1);

    ADC_Enable(ADC1);

    ADC_SoftwareStartConv(ADC1);
}

double GetRealVoltage() {
	uint16_t adcData;
	adcData = ADC_ReadConversionValue(ADC1);
	double adcVoltage = (adcData*POWER_SUPPLY)/4095;
	double RealVoltage = adcVoltage - INPUT_AMPLITUDE/2;
	errorCounterADC = 0;
	return RealVoltage;
}

void ADC_Restart_note() {
	if (RCM_ReadStatusFlag(RCM_FLAG_IWDTRST) == SET) {
		USART_Tx_Char(USART, 13);
		uint8_t restart_message[15] = {'A', 'D', 'C', ' ', 'E', 'R', 'R', 'O', 'R', ' ','R', 'E', 'S', 'E', 'T'};
		USART_Write(USART, restart_message, 15);
		USART_Tx_Char(USART, 13);
		RCM_ClearStatusFlag();	
	}
}

void ADC_Reload() {
	if (ADC_ReadStatusFlag(ADC1,  ADC_FLAG_OVR) == SET) {
		ADC_Disable(ADC1);
		ADC_Init();
		errorCounterADC++;
		ADC_Restart_note();
		ADC_ClearStatusFlag(ADC1, ADC_FLAG_OVR);
	}
	
	if (errorCounterADC >=5) {
		while (1) {};
	}
}
