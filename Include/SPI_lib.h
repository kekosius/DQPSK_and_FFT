#ifndef MY_LIB_SPI_LIB_H_
#define MY_LIB_SPI_LIB_H_

#include "main.h"

void SPI_Init(void);
void SPI_Tx_Data (SPI_T*, uint8_t*, uint16_t);
void SPI_Rx_Data (SPI_T*, uint8_t*, uint16_t);
void SPI_TxRx_Data (SPI_T*, uint8_t*, uint8_t*, uint16_t);
void SPI_Wait_for_termination(SPI_T* SPI);

#endif
