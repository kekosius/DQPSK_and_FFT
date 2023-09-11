#ifndef MY_LIB_AMP_LIB_H_
#define MY_LIB_AMP_LIB_H_

#include "main.h"

extern double AverageVoltage;

double AmlitudeAnalysis(double*, uint16_t);
double MovingAverage(double);
void MovingAverageClear(void);

#endif
