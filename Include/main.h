
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes */

#include "stdio.h"
#include "math.h"

#include "Board.h"
#include "apm32f4xx.h"
#include "apm32f4xx_gpio.h"
#include "apm32f4xx_dac.h"
#include "apm32f4xx_adc.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_usart.h"
#include "apm32f4xx_tmr.h"
#include "apm32f4xx_rcm.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_spi.h"
#include "apm32f4xx_dac.h"

#include "ADC_lib.h"
#include "LCD_lib.h"
#include "SPI_lib.h"
#include "TIM_lib.h"
#include "WDT_lib.h"
#include "UART_lib.h"
#include "Filter_lib.h"
#include "RawDataAnalys.h"
#include "SignalReceiveHandler.h"
#include "font_lib.h"

void ADC_Init(void);
void DAC_Init(void);
void KEY1_IRQHandler(void);
void KEY2_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif

