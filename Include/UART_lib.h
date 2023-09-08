#ifndef MY_LIB_UART_LIB_H_
#define MY_LIB_UART_LIB_H_

#include "main.h"

void APM_MINI_COM1_Init(USART_Config_T*);
void Delay(uint32_t);
void USART_Init(void);
void USART_Write(USART_T*,uint8_t*, uint32_t);
void UART_Tx_Number(uint32_t);
void USART_Tx_Char(USART_T*, uint8_t);
void USART_Tx_Number(USART_T*, int64_t);
void USART_Tx_Float(USART_T*, double, uint16_t);
void USART_Tx_Specrum_Result(USART_T*, double, double);
void USART_Reload(USART_T*);

#endif
