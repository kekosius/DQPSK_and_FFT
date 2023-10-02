/*!
 * @file        KEY_lib.c
 *
 * @brief       Смена режимов работы (АРС/АРСФ)
 *
 * @version     V1.0.0
 *
 * @date        02-10-2023
 */

#include "KEY_lib.h"

uint8_t WorkStatus = 0;    		/*!< Содержит информацию о текущем режиме работе. Extern переменная,
							     *   используется в [KEY1_IRQHandler()](#KEY1_IRQHandler) для смены режима
								 *   режима работы и в [LCD_Show_No_Result()](#LCD_Show_No_Result) для
								 *   отображения отсутсвия приёма АРС/АРСФ кодов*/

/*!
 * @brief     Смена режимов работы по нажатию на KEY1
 *
 * При нажатии на кнопку KEY1 сменяет текущий режим работы. Отключает управляющий
 * таймер для текущего режима работы, выполняет возврат к дефолтным значенияем всех
 * используемых переменных и запускает новый режим работы. Переменная WorkStatus
 * определяет текущий режим работы. Значение 1 соответсвует  режиму АРСФ, значение
 * 0 соответсвует режиму АРС
 */

void KEY1_IRQHandler()
{
    if(EINT_ReadIntFlag(EINT_LINE_1)) {
		Delay(10000000);
		FlagsClear();
		if (WorkStatus) {
			LCD_DQPSK_mode();
			WorkStatus = 0;
			TMR_Disable(TMR3);
			TMR_Disable(TMR4);
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
			TMR_Disable(TMR4);
			SpectrumVarReInit();
			MovingAverageClear();
			TMR3_Init();
			LCD_Result_Reset();
		}
		TMR4_Init();
        EINT_ClearIntFlag(EINT_LINE_1);
    }
}
