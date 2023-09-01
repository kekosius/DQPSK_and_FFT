#ifndef MY_LIB_TP_LIB_H_
#define MY_LIB_TP_LIB_H_

#include "main.h"

#define CS_TP_SET		GPIOB->BSCL |= GPIO_PIN_2;
#define CS_TP_RESET		GPIOB->BSCH |= GPIO_PIN_2;
//#define TP_DELAY		50
#define X_DLT			170
#define X_MAX			2000
#define Y_DLT			95
#define Y_MAX			2000
#define X_PROP			3.6 //~483 pixels //5.7
#define Y_PROP			5.3 //~320 pixels //7.75

extern SPI_T* SPI;

void TP_GPIO_Config(void);
void initTP(void);
uint8_t getTPPoints(uint16_t*, uint16_t*, uint16_t);

#endif
