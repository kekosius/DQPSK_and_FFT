
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes */
#include "apm32f4xx.h"
#include "Board.h"
#include "stdio.h"
#include "apm32f4xx_gpio.h"
#include "apm32f4xx_dac.h"
#include "apm32f4xx_adc.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_usart.h"
#include "../Project/MDK/TIM_lib.h"
#include "../Project/MDK/Filter_lib.h"
#include "../Project/MDK/UART_lib.h"
#include "../Project/MDK/SignalReceiveHandler.h"
#include "../Project/MDK/LCD_lib.h"
#include "../Project/MDK/SPI_lib.h"
#include "../Project/MDK/TP_lib.h"

void ADC_Init(void);
void DAC_Init(void);
void KEY1_IRQHandler(void);
void KEY2_IRQHandler(void);


#ifdef __cplusplus
}
#endif

#endif

