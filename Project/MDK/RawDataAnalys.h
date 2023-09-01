#ifndef MY_LIB_RAW_DATA_ANALYS_H_
#define MY_LIB_RAW_DATA_ANALYS_H_

#include "main.h"

extern double AverageVoltage;

uint8_t Demodulation(double*, uint16_t*, uint8_t*, uint8_t*);
void ZeroCrossAnalysis(double*, double*, int8_t, uint16_t*);

#endif /* MY_LIB_RAW_DATA_ANALYS_H_ */
