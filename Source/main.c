#include "main.h"

SPI_T* SPI = SPI1;
USART_T* USART = USART1;

void DAC_Init(void);
void ADC_Init(void);

int main(void)
{
	USART1_Init();
    DAC_Init();
    ADC_Init();
	APM_MINI_LEDInit(LED2);
	APM_MINI_LEDInit(LED3);
	SPI_Init();
	LCD_Start();
	APM_MINI_PBInit(BUTTON_KEY1, BUTTON_MODE_EINT);
    while (1) {};
}

void DAC_Init()
{
    GPIO_Config_T   gpioConfig;
    DAC_Config_T    dacConfig;

    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA);

    GPIO_ConfigStructInit(&gpioConfig);
    gpioConfig.mode    = GPIO_MODE_AN;
    gpioConfig.pupd    = GPIO_PUPD_NOPULL;
    gpioConfig.pin     = GPIO_PIN_4;
    GPIO_Config(GPIOA, &gpioConfig);

    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_DAC);

    DAC_ConfigStructInit(&dacConfig);
    dacConfig.trigger             = DAC_TRIGGER_NONE;
    dacConfig.waveGeneration      = DAC_WAVE_GENERATION_NONE;
    dacConfig.maskAmplitudeSelect = DAC_LFSR_MASK_BIT11_1;
    dacConfig.outputBuffer        = DAC_OUTPUT_BUFFER_ENBALE;
    DAC_Config(DAC_CHANNEL_1, &dacConfig);

    DAC_Enable(DAC_CHANNEL_1);
}

uint8_t WorkStatus = 0; 

void KEY1_IRQHandler()
{
    if(EINT_ReadIntFlag(EINT_LINE_1)) {
		Delay(10000000);
		FlagsClear();
		if (WorkStatus) {
			LCD_DQPSK_mode();
			WorkStatus = 0;
			TMR_Disable(TMR3);
			FilterWipe();
			VarReInit();
			TMR2_Init();
			LCD_Result_Reset();
		}
		else {
			LCD_Freq_mode();
			WorkStatus = 1;
			TMR_Disable(TMR2);
			SpectrumVarReInit();
			TMR3_Init();
			LCD_Result_Reset();
		}
		TMR4_Init();
        EINT_ClearIntFlag(EINT_LINE_1);
    }
}
