#include "KEY_lib.h"

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
			MovingAverageClear();
			TMR2_Init();
			LCD_Result_Reset();
		}
		else {
			LCD_Freq_mode();
			WorkStatus = 1;
			TMR_Disable(TMR2);
			SpectrumVarReInit();
			MovingAverageClear();
			TMR3_Init();
			LCD_Result_Reset();
		}
		TMR4_Init();
        EINT_ClearIntFlag(EINT_LINE_1);
    }
}
