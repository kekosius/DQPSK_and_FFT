#include "main.h"

SPI_T* SPI = SPI1;
USART_T* USART = USART1;

void DAC_Init(void);

int main(void)
{
	USART_Init();
    DAC_Init();
    ADC_Init();
	APM_MINI_LEDInit(LED2);
	APM_MINI_LEDInit(LED3);
	SPI_Init();
	LCD_Start();
	APM_MINI_PBInit(BUTTON_KEY1, BUTTON_MODE_EINT);
	IWDT_init();
	while (1) {
		SPI->DATA = 1;
		IWDT_Update();
	}
}
