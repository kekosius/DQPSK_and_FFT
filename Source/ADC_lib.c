#include "ADC_lib.h"

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

    NVIC_EnableIRQRequest(ADC_IRQn, 1, 1);

    ADC_Enable(ADC1);

    ADC_SoftwareStartConv(ADC1);
}

double GetRealVoltage() {
	uint16_t adcData;
	adcData = ADC_ReadConversionValue(ADC1);
	double adcVoltage = (adcData*POWER_SUPPLY)/4095;
	double RealVoltage = adcVoltage - INPUT_AMPLITUDE/2;
	return RealVoltage;
}
