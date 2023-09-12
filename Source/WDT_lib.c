#include "WDT_lib.h"

volatile uint32_t TimingDelay = 0;

void IWDT_Restart_note(void);

void IWDT_init() {
	SysTick_Config(16000000/1000);			//1 тик = 1 ms
	IWDT_Restart_note();
	IWDT_EnableWriteAccess();
	IWDT_ConfigDivider(IWDT_DIVIDER_32);
	IWDT_ConfigReload(1000);
	IWDT_Refresh();
	IWDT_Enable();
}

void IWDT_Delay(uint32_t ms) {
	TimingDelay = 0;
    while(TimingDelay < ms);
}

void IWDT_Restart_note() {
	if (RCM_ReadStatusFlag(RCM_FLAG_IWDTRST) == SET) {
		uint8_t restart_message[16] = {'W', 'D', 'T', ' ', 'S', 'Y', 'S', 'T', 'E', 'M', ' ', 'R', 'E', 'S', 'E', 'T'};
		USART_Write(USART, restart_message, 16);
		USART_Tx_Char(USART, 13);
		RCM_ClearStatusFlag();	
	}
}

void IWDT_Update() {
	IWDT_Delay(300);
	IWDT_Refresh();
}

void Fatal_error() {
	APM_MINI_LEDOn(LED2);
	APM_MINI_LEDOn(LED3);
	fillRectangle(0, 0, LCD_HEIGHT, LCD_WIDTH, RED);
	uint8_t fatal_message[18] = {'S', 'Y', 'S', 'T', 'E', 'M', ' ', 'F', 'A', 'T', 'A', 'L', ' ', 'E', 'R', 'R', 'O', 'R'};
	USART_Write(USART, fatal_message, 18);
	DAC_SetHigh();
	while(1) {};
}
