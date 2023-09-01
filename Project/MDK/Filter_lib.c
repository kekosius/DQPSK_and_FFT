#include "Filter_lib.h"

#define FILTER_ORDER 10

int16_t Buff = FILTER_ORDER - 1;

double filter_cff[FILTER_ORDER+1] = {0.610756766442843047, -0.589964143152525988, -0.273905886988929204,
									-0.0697084231096051082,  0.0624288681984259547, 0.12607631896943805,
									 0.144535691178322656, 0.0996446362115575301, 0.0751365980279702222,
									-0.043088989279420821, -0.141911422747041521};

double prevStates[FILTER_ORDER] = {0};												

double HighPassFilter(double InputVoltage) {
	double Output = InputVoltage*filter_cff[0];
	uint16_t cffCounter = 1;
	int16_t j;
	
	for (j = Buff; j < FILTER_ORDER; j++) {
		Output += prevStates[j]*filter_cff[cffCounter];
		cffCounter++;
	}
	
	for (j = 0; j < Buff; j++) {
		Output += prevStates[j]*filter_cff[cffCounter];
		cffCounter++;
	}
	
	Buff--;
	if (Buff < 0) Buff = FILTER_ORDER-1;
	prevStates[Buff] = InputVoltage;
	
	return Output;
}

void FilterWipe() {
	for (int i = 0; i < FILTER_ORDER; i++) prevStates[i] = 0;
}
