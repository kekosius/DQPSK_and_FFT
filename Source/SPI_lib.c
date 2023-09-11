#include "SPI_lib.h"

uint8_t errorCounterSPI = 0;

void SPI_ClearInpitBuffer(SPI_T*);

void SPI_Init()
{
    GPIO_Config_T GPIO_InitStructure;
    SPI_Config_T  SPI1_InitStructure;

    RCM_EnableAHB1PeriphClock (RCM_AHB1_PERIPH_GPIOA| RCM_AHB1_PERIPH_GPIOB);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SPI1);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);

    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_5, GPIO_AF_SPI1);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_6, GPIO_AF_SPI1);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_7, GPIO_AF_SPI1);

    /* Config SPI1, SCK=PA5, MISO=PA6, MOSI=PA7 */
    GPIO_ConfigStructInit(&GPIO_InitStructure);
    GPIO_InitStructure.pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStructure.speed = GPIO_SPEED_100MHz;
    GPIO_InitStructure.mode = GPIO_MODE_AF;
    GPIO_InitStructure.otype = GPIO_OTYPE_PP;
    GPIO_InitStructure.pupd = GPIO_PUPD_NOPULL;
    GPIO_Config(GPIOA, &GPIO_InitStructure);

    SPI_ConfigStructInit(&SPI1_InitStructure);
    SPI1_InitStructure.direction = SPI_DIRECTION_2LINES_FULLDUPLEX;
    SPI1_InitStructure.mode = SPI_MODE_MASTER;
    SPI1_InitStructure.length = SPI_DATA_LENGTH_8B;
    SPI1_InitStructure.polarity = SPI_CLKPOL_LOW;
    SPI1_InitStructure.phase = SPI_CLKPHA_1EDGE;
    SPI1_InitStructure.nss = SPI_NSS_SOFT;
    SPI1_InitStructure.baudrateDiv = SPI_BAUDRATE_DIV_2;
    SPI1_InitStructure.firstBit = SPI_FIRSTBIT_MSB;
    SPI1_InitStructure.crcPolynomial = 7;
    SPI_Config(SPI, &SPI1_InitStructure);
    SPI_DisableCRC(SPI);
    SPI_DisableSSOutput(SPI);
	
	SPI_I2S_EnableInterrupt(SPI, SPI_I2S_INT_ERR);			//SPI Error interrupt enable
	
	NVIC_EnableIRQRequest(SPI1_IRQn, 1, 1);

    SPI_Enable(SPI1);
}

void SPI_Tx_Data (SPI_T* SPI, uint8_t* buff, uint16_t len) {
	for (uint16_t i = 0; i < len; i++) {
		while (SPI_I2S_ReadStatusFlag(SPI, SPI_FLAG_TXBE) == RESET) {};
		SPI->DATA = buff[i];
		SPI_ClearInpitBuffer(SPI);
	}
	errorCounterSPI = 0;
}

void SPI_Rx_Data (SPI_T* SPI, uint8_t* buff, uint16_t len) {
	for (uint16_t i = 0; i < len; i++) {
		while (SPI_I2S_ReadStatusFlag(SPI, SPI_FLAG_RXBNE) == RESET) {};
		buff[i] = SPI->DATA;
	}
	errorCounterSPI = 0;
}

void SPI_TxRx_Data (SPI_T* SPI, uint8_t* Tx_buff, uint8_t* Rx_buff, uint16_t len) {
	for (uint16_t i = 0; i < len; i++) {
		SPI_Tx_Data(SPI, &Tx_buff[i], 1);
		SPI_Rx_Data(SPI, &Rx_buff[i], 1);
	}
}

void SPI_Wait_for_termination (SPI_T* SPI) {
	while ( SPI_I2S_ReadStatusFlag(SPI, SPI_FLAG_BSY) == SET) {};
}

void SPI_Restart_note() {
	USART_Tx_Char(USART, 13);
	uint8_t restart_message[15] = {'S', 'P', 'I', ' ', 'E', 'R', 'R', 'O', 'R', ' ','R', 'E', 'S', 'E', 'T'};
	USART_Write(USART, restart_message, 15);
	USART_Tx_Char(USART, 13);
}

void SPI_Reload(SPI_T* SPI) {
	if (SPI_I2S_ReadStatusFlag(SPI, SPI_FLAG_ME) == SET ||
		SPI_I2S_ReadStatusFlag(SPI, SPI_FLAG_OVR) == SET)
	{
		SPI_Disable(SPI);
		SPI_Init();
		errorCounterSPI++;
		SPI_Restart_note();
		if (SPI_I2S_ReadStatusFlag(SPI, SPI_FLAG_ME) == SET) USART_Tx_Char(USART, '2');
		if (SPI_I2S_ReadStatusFlag(SPI, SPI_FLAG_OVR) == SET) USART_Tx_Char(USART, '3');
		SPI_I2S_ClearStatusFlag(SPI, SPI_FLAG_ME);
		SPI_I2S_ClearStatusFlag(SPI, SPI_FLAG_OVR);
	}
		
	if (errorCounterSPI >=5) {
		while (1) {};
	}
}

void SPI_ClearInpitBuffer(SPI_T* SPI) {
	uint16_t data = SPI->DATA;
	return;
}
