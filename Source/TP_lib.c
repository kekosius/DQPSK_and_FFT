#include "TP_lib.h"

void TP_GPIO_Config() {
	GPIO_Config_T GPIO_InitStructure;
	
	RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB);
	
	GPIO_ConfigStructInit(&GPIO_InitStructure);
    GPIO_InitStructure.pin = GPIO_PIN_2;
    GPIO_InitStructure.speed = GPIO_SPEED_100MHz;
    GPIO_InitStructure.mode = GPIO_MODE_OUT;
    GPIO_Config(GPIOB, &GPIO_InitStructure);
	
	GPIOB->BSCH |= GPIO_PIN_2;
	
	UPDATE;
}

void initTP(void){
	CS_TP_SET;
	UPDATE;
}

uint8_t getTPPoints(uint16_t* x0, uint16_t* y0, uint16_t cnt){
	uint8_t data = 0, retVal = 0;
	uint16_t x = 0, y = 0, cntOK = 0;
	uint16_t xAcp, yAcp;

	CS_TP_RESET;
	UPDATE;
	
	for(int i=0; i<cnt; i++)
	{
		data = 0x90;
//		HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
		SPI_Tx_Data(SPI, &data, 1);
		data = 0x00;
//		HAL_SPI_TransmitReceive(&hspi1, &data, &retVal, 1, 1000);
		SPI_TxRx_Data(SPI, &data, &retVal, 1);
		x = ((retVal & 0x7F) << 4);
//		HAL_SPI_TransmitReceive(&hspi1, &data, &retVal, 1, 1000);
		SPI_TxRx_Data(SPI, &data, &retVal, 1);
		x |= ((retVal << 1) >> 4);
		
		data = 0xD0;
//		HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
		SPI_Tx_Data(SPI, &data, 1);
		data = 0x00;
//		HAL_SPI_TransmitReceive(&hspi1, &data, &retVal, 1, 1000);
		SPI_TxRx_Data(SPI, &data, &retVal, 1);
		y = ((retVal & 0x7E) << 4);
//		HAL_SPI_TransmitReceive(&hspi1, &data, &retVal, 1, 1000);
		SPI_TxRx_Data(SPI, &data, &retVal, 1);
		y |= ((retVal << 1) >> 4);		

		if(x >= X_DLT && x <= X_MAX && y >= Y_DLT && y <= Y_MAX)
		{
			cntOK++;
			if(xAcp == 0)
				xAcp = x;
			else
				xAcp = (xAcp + x) / 2;
			if(yAcp == 0)
				yAcp = y;
			else
				yAcp = (yAcp + y) / 2;  
		}
	}
	CS_TP_SET;
	UPDATE;

	//if(xAcp >= X_DLT && xAcp <= X_MAX && yAcp >= Y_DLT && yAcp <= Y_MAX) //filter
	//{
	*x0 = (xAcp - X_DLT) / X_PROP;
	*y0 = (yAcp - Y_DLT) / Y_PROP;
		
	return cntOK;
	//}
	//return 0;
}
