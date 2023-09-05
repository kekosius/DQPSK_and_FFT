#ifndef MY_LIB_TIM_LIB_H_
#define MY_LIB_TIM_LIB_H_

#include "main.h"

void TMR2_Init(void);
void TMR3_Init(void);
void TMR4_Init(void);
void TMR2_EventCallback(void);
void TMR3_EventCallback(void);
void TMR4_EventCallback(void);
void SpectrumVarReInit(void);
void VarReInit(void);

#endif /* MY_LIB_TIM_LIB_H_ */
