#ifndef MY_LIB_TIM_LIB_H_
#define MY_LIB_TIM_LIB_H_

#include "apm32f4xx_tmr.h"
#include "apm32f4xx_rcm.h"
#include "apm32f4xx_misc.h"
#include "UART_lib.h"
#include "math.h"
#include "apm32f4xx_dac.h"
#include "Board.h"
#include "RawDataAnalys.h"
#include "Filter_lib.h"
#include "ADC_lib.h"
#include "main.h"
#include "string.h"
#include "SignalReceiveHandler.h"

void TMR2_Init(void);
void TMR3_Init(void);
void TMR4_Init(void);
void TMR2_EventCallback(void);
void TMR3_EventCallback(void);
void TMR4_EventCallback(void);
void SpectrumVarReInit(void);
void VarReInit(void);

#endif /* MY_LIB_TIM_LIB_H_ */
