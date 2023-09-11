#ifndef MY_LIB_PM_H_
#define MY_LIB_PM_H_

#include "main.h"

extern USART_T* USART;

uint8_t Demodulation(double*, uint16_t*, uint8_t*, uint8_t*);
void ZeroCrossAnalysis(double*, double*, int8_t, uint16_t*);

#endif
