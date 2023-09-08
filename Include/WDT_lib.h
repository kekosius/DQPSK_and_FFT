#ifndef MY_LIB_WDT_LIB_H_
#define MY_LIB_WDT_LIB_H_

#include "main.h"
#include "apm32f4xx_iwdt.h"

extern volatile uint32_t TimingDelay;

void IWDT_init(void);
void IWDT_Update(void);

#endif
