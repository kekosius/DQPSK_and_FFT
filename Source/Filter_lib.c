/*!
 * @file        Filter_lib.c
 *
 * @brief       Функции для фильтрации паразитных частот, содержащихся во входном сигнале
 *
 * @version     V1.0.0
 *
 * @date        12-09-2023
 */

#include "Filter_lib.h"

#define FILTER_ORDER 10		/*!< Порядок частотного фильтра*/

/*! Коэффициенты частотного фильтра
 *
 * Частотный фильтр ослабляет амплитуду низких частот <800 Гц и усиливает амплитуду частот в полосе пропускания 3200-4000 Гц
*/
double filter_cff[FILTER_ORDER+1] = {0.610756766442843047, -0.589964143152525988, -0.273905886988929204,
									-0.0697084231096051082,  0.0624288681984259547, 0.12607631896943805,
									 0.144535691178322656, 0.0996446362115575301, 0.0751365980279702222,
									-0.043088989279420821, -0.141911422747041521};
									
double prevStates[FILTER_ORDER] = {0};	/*!< Кольцевой буфер, хранит предыдущие значения выхода фильтра*/	

int16_t Buff = FILTER_ORDER - 1;		/*!< Позиция для записи текущего значения в кольцевом буфере*/

/*! Реализация программного частотного фильтра
 *
 * @param InputVoltage входной сигнал в милливольтах
 * @retval Напряжение Отфильтрованный входной сигнал в милливольтах
*/
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

/*! Обнуление кольцевого буфера [prevStates[]](#prevStates) и присвоение #Buff дефолтного значения 
 *
*/
void FilterWipe() {
	for (int i = 0; i < FILTER_ORDER; i++) prevStates[i] = 0;
	Buff = FILTER_ORDER - 1;
}
