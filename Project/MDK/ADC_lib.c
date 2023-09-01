#include "ADC_lib.h"

void ADC_Init()
{
    GPIO_Config_T   gpioConfig;
    ADC_Config_T    adcConfig;

    /* Enable GPIOA clock */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA);

    /* ADC channel 0 configuration */
    GPIO_ConfigStructInit(&gpioConfig);
    gpioConfig.mode    = GPIO_MODE_AN;
    gpioConfig.pupd    = GPIO_PUPD_NOPULL;
    gpioConfig.pin     = GPIO_PIN_0;
    GPIO_Config(GPIOA, &gpioConfig);

    /* Enable ADC clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_ADC1);

    /* ADC configuration */
    ADC_Reset();
    ADC_ConfigStructInit(&adcConfig);
    adcConfig.resolution          = ADC_RESOLUTION_12BIT;
    adcConfig.continuousConvMode  = ENABLE;
    adcConfig.dataAlign           = ADC_DATA_ALIGN_RIGHT;
    adcConfig.extTrigEdge         = ADC_EXT_TRIG_EDGE_NONE;
    adcConfig.scanConvMode        = DISABLE;
    ADC_Config(ADC1, &adcConfig);

    /* ADC channel 10 Convert configuration */
    ADC_ConfigRegularChannel(ADC1, ADC_CHANNEL_0, 1, ADC_SAMPLETIME_84CYCLES);

    /* Enable complete conversion interupt */
    //ADC_EnableInterrupt(ADC1, ADC_INT_EOC);

    /* NVIC configuration */
    NVIC_EnableIRQRequest(ADC_IRQn, 1, 1);

    /* Enable ADC */
    ADC_Enable(ADC1);

    /* ADC start conversion */
    ADC_SoftwareStartConv(ADC1);
}

double GetRealVoltage() {
	uint16_t adcData;
	adcData = ADC_ReadConversionValue(ADC1);
	double adcVoltage = (adcData*POWER_SUPPLY)/4095;
	double RealVoltage = adcVoltage - INPUT_AMPLITUDE/2;
	return RealVoltage;
}
