/*!
 * @file        main.c
 *
 * @brief       Main program body
 *
 * @version     V1.0.0
 *
 * @date        09-08-2023
 */

#include "main.h"

SPI_T* SPI = SPI1;

void DAC_Init(void);
void ADC_Init(void);

int main(void)
{
	USART1_Init();
	char k[3];
	sprintf(k, "%u", 'D');
	uint8_t e[3] = "x";
//	USART_Tx_Number(USART1, e[0]);
//	USART_Tx_Number(USART1, e[1]);
//	USART_Tx_Number(USART1, e[2]);
    DAC_Init();
    ADC_Init();
	APM_MINI_LEDInit(LED2);
	APM_MINI_LEDInit(LED3);
	SPI_Init();
	LCD_Start();
	
//	LCD_DQPSK_mode();
//	uint32_t counter = 0;
//	while (1) {
//		LCD_DQPSK_Result(0x66, 8080, 0.01 * counter);
//		Delay(1000000);
//		counter++;
//	};
	
	APM_MINI_PBInit(BUTTON_KEY1, BUTTON_MODE_EINT);
    while (1) {};
}

void DAC_Init()
{
    GPIO_Config_T   gpioConfig;
    DAC_Config_T    dacConfig;

    /* Enable GPIOA clock */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA);

    /* DAC out PA4 pin configuration */
    GPIO_ConfigStructInit(&gpioConfig);
    gpioConfig.mode    = GPIO_MODE_AN;
    gpioConfig.pupd    = GPIO_PUPD_NOPULL;
    gpioConfig.pin     = GPIO_PIN_4;
    GPIO_Config(GPIOA, &gpioConfig);

    /* Enable DAC clock */
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_DAC);

    /* DAC channel 1 configuration */
    DAC_ConfigStructInit(&dacConfig);
    dacConfig.trigger             = DAC_TRIGGER_NONE;
    dacConfig.waveGeneration      = DAC_WAVE_GENERATION_NONE;
    dacConfig.maskAmplitudeSelect = DAC_LFSR_MASK_BIT11_1;
    dacConfig.outputBuffer        = DAC_OUTPUT_BUFFER_ENBALE;
    DAC_Config(DAC_CHANNEL_1, &dacConfig);

    /* Enable DAC channel 1 */
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
