#include "AMP.h"

#define MOVE_AVG_NUM									(5)

#define SEND_AVG

double AmlitudeAnalysis(double* Data, uint16_t length) {
	double max = 0;
	double min = 0;
	double minSum = 0;
	double maxSum = 0;
	double avg = 0;
	double v = 0;
	uint16_t maxCounter = 0;
	uint16_t minCounter = 0;

	for (uint16_t i = 0; i < length; i++){
		v = Data[i];
		if ((v > 400) && (min < 0)) {
			minSum += min;
			minCounter++;
			min = 0;
		}
		if ((v < -400) && (max > 0)) {
			maxSum += max;
			maxCounter++;
			max = 0;
		}
		if (v < min) min = v;
		if (v > max) max = v;
	}
	
	avg = (maxSum/maxCounter) - (minSum/minCounter);
	
	#ifdef MOVE_AVG
		double mAvg = MovingAverage(avg);
	#endif

	#ifdef SEND_AVG
		USART_Tx_Char(USART, 13);
		USART_Write(USART, (uint8_t[5]) {'A', 'V', 'G', ':', ' '}, 5 );
		USART_Tx_Float(USART, avg, 3);
		USART_Write(USART, (uint8_t[2]) {'m', 'V'}, 2 );
	#endif
		
	#ifdef MOVE_AVG
		return mAvg;
	#endif
	
	return avg;
}

double AverageVoltage = 0;

double avgStorage[MOVE_AVG_NUM] = {0};
uint16_t avgCounter = 0;

double MovingAverage(double avg) {
	avgStorage[avgCounter] = avg;
	avgCounter++;
	avgCounter %= MOVE_AVG_NUM;
	
	double avgSum = 0;
	uint16_t sumCounter = 0;
	for (uint16_t i = 0; i < MOVE_AVG_NUM; i++) {
		if (avgStorage[i] > 0) {
			avgSum += avgStorage[i];
			sumCounter++;
		}
	}
	
	return (avgSum/sumCounter);
}

void MovingAverageClear() {
	for (uint16_t i = 0; i < MOVE_AVG_NUM; i++) {
		avgStorage[i] = 0;
	}
	avgCounter = 0;
}
