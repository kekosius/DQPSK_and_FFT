#ifndef MY_LIB_RAW_DATA_ANALYS_H_
#define MY_LIB_RAW_DATA_ANALYS_H_

#include "main.h"

extern double AverageVoltage;

double AmlitudeAnalysis(double*, uint16_t);
double MovingAverage(double);
void MovingAverageClear(void);
uint8_t Demodulation(double*, uint16_t*, uint8_t*, uint8_t*);
void ZeroCrossAnalysis(double*, double*, int8_t, uint16_t*);
void SpectrumAnalysis(double*, uint8_t);

#endif
