#include "LCD_lib.h"

#define DELAY 0

uint8_t MSG_Start_1[7][3] = {"Н", "А", "Ж", "М", "И", "Т", "Е"};
uint8_t MSG_Start_2[4][3] = {"К", "Е", "У", "1"};

uint8_t MSG_DQPSK_1[4][3] = {"А", "Р", "С", "Ф"};
uint8_t MSG_DQPSK_2[4][3] = {"К", "О", "Д", ":"};
uint8_t MSG_DQPSK_3[4][3] = {"К", "Р", "Ц", ":"};
uint8_t MSG_DQPSK_4[4][3] = {"С", "Р", "Ц", ":"};
uint8_t MSG_DQPSK_5[4][3] = {"А", "М", "П", ":"};

void LCD_Start() {
	LCD_GPIO_Config();
	initLCD();
	fillRectangle(0, 0, LCD_HEIGHT, LCD_WIDTH, WHITE);
	drawString(10, 200, BLACK, WHITE, MSG_Start_1, 7);
	drawString(85, 250, BLACK, WHITE, MSG_Start_2, 4);
}

void LCD_DQPSK_mode() {
	fillRectangle(0, 0, LCD_HEIGHT, LCD_WIDTH, WHITE);
	drawString(85, 5, RED, WHITE, MSG_DQPSK_1, 4);
	drawString(5, 100, BLACK, WHITE, MSG_DQPSK_2, 4);
	drawString(5, 200, BLACK, WHITE, MSG_DQPSK_3, 4);
	drawString(5, 300, BLACK, WHITE, MSG_DQPSK_4, 4);
	drawString(5, 400, BLACK, WHITE, MSG_DQPSK_5, 4);
}

uint16_t prevCode = 0x999;
uint8_t NoResultState = 0;

void LCD_DQPSK_Result(uint16_t code, uint16_t speed, double AvgV) {
	if (NoResultState) {
		prevCode = 0x999;
		TMR_Enable(TMR4);
	}
	NoResultCounter = 2;
	NoResultState = 0;
	
	uint8_t MSG_DQPSK_6[4][3] = {"0", ".", "0", "0"};
	fillRectangle(155, 350, 500, 500, WHITE);
	AvgV += 5;												//Округление по предпоследнему знаку
	int64_t V = (int) AvgV;
	toString(MSG_DQPSK_6[0], V/1000);
	toString(MSG_DQPSK_6[2], (V % 1000)/100);
	toString(MSG_DQPSK_6[3], (V % 100)/10);
	drawString(180, 400, BLACK, WHITE, MSG_DQPSK_6, 4);
	
	if (code == prevCode) return;
	prevCode = code;
	fillRectangle(155, 100, 380, 400, WHITE);
	uint8_t MSG_DQPSK_4[4][3] = {"0", "x", "?", "?"};
	toString(MSG_DQPSK_4[2], code / 0x10);
	toString(MSG_DQPSK_4[3], code % 0x10);
	drawString(155, 100, BLACK, WHITE, MSG_DQPSK_4, 4);
	if (speed == 9999) {
		uint8_t MSG_DQPSK_6[2][3] = {"Н", "К"};
		drawString(220, 200, BLACK, WHITE, MSG_DQPSK_6, 2);
		drawString(220, 300, BLACK, WHITE, MSG_DQPSK_6, 2);
		return;
	}
	uint8_t MSG_DQPSK_5[2][3] = {"?", "?"};
	toString(MSG_DQPSK_5[0], speed / 1000);
	toString(MSG_DQPSK_5[1], (speed % 1000)/100);
	drawString(220, 200, BLACK, WHITE, MSG_DQPSK_5, 2);
	toString(MSG_DQPSK_5[0], (speed % 100) / 10);
	toString(MSG_DQPSK_5[1], speed % 10);
	drawString(220, 300, BLACK, WHITE, MSG_DQPSK_5, 2);
	
	
}

uint8_t MSG_Freq_1[3][3] = {"А", "Р", "С"};
uint8_t MSG_Freq_2[4][3] = {"К", "О", "Д", ":"};
uint8_t MSG_Freq_3[4][3] = {"К", "Р", "Ц", ":"};
uint8_t MSG_Freq_6[4][3] = {"А", "М", "П", ":"};

void LCD_Freq_mode() {
	fillRectangle(0, 0, LCD_HEIGHT, LCD_WIDTH, WHITE);
	drawString(105, 5, RED, WHITE, MSG_Freq_1, 3);
	drawString(5, 100, BLACK, WHITE, MSG_Freq_2, 4);
	drawString(5, 200, BLACK, WHITE, MSG_Freq_3, 4);
	drawString(5, 300, BLACK, WHITE, MSG_Freq_6, 4);
}

void LCD_Freq_Result(uint16_t freq, double AvgV) {
	if (NoResultState) {
		prevCode = 0x999;
		TMR_Enable(TMR4);
	}
	NoResultCounter = 2;
	NoResultState = 0;
	
	uint8_t MSG_Freq_7[4][3] = {"0", ".", "0", "0"};
	fillRectangle(155, 300, LCD_HEIGHT, 350, WHITE);
	AvgV += 5;												//Округление по предпоследнему знаку
	int64_t V = (int) AvgV;
	toString(MSG_Freq_7[0], V/1000);
	toString(MSG_Freq_7[2], (V % 1000)/100);
	toString(MSG_Freq_7[3], (V % 100)/10);
	drawString(180, 300, BLACK, WHITE, MSG_Freq_7, 4);
	
	if (freq == prevCode) {
		Delay(10000000);
		return;
	}
	prevCode = freq;
	fillRectangle(155, 100, 380, 400, WHITE);
	uint8_t MSG_Freq_4[3][3] = {" ", " ", " "};
	uint8_t MSG_Freq_5[3][3] = {" ", " ", " "};
	switch (freq) {
		case 75:
			toString(MSG_Freq_4[1], 7);
			toString(MSG_Freq_4[2], 5);
			toString(MSG_Freq_5[1], 8);
			toString(MSG_Freq_5[2], 0);
			break;
		case 125:
			toString(MSG_Freq_4[0], 1);
			toString(MSG_Freq_4[1], 2);
			toString(MSG_Freq_4[2], 5);
			toString(MSG_Freq_5[1], 7);
			toString(MSG_Freq_5[2], 0);
			break;
		case 175:
			toString(MSG_Freq_4[0], 1);
			toString(MSG_Freq_4[1], 7);
			toString(MSG_Freq_4[2], 5);
			toString(MSG_Freq_5[1], 6);
			toString(MSG_Freq_5[2], 0);
			break;
		case 225:
			toString(MSG_Freq_4[0], 2);
			toString(MSG_Freq_4[1], 2);
			toString(MSG_Freq_4[2], 5);
			toString(MSG_Freq_5[1], 4);
			toString(MSG_Freq_5[2], 0);
			break;
		case 275:
			toString(MSG_Freq_4[0], 2);
			toString(MSG_Freq_4[1], 7);
			toString(MSG_Freq_4[2], 5);
			toString(MSG_Freq_5[2], 0);
			break;
		case 325:
			toString(MSG_Freq_4[0], 3);
			toString(MSG_Freq_4[1], 2);
			toString(MSG_Freq_4[2], 5);
			/* В */
			MSG_Freq_5[0][0] = 208;
			MSG_Freq_5[0][1] = 146;
			MSG_Freq_5[0][2] = 0;
			/* Р */
			MSG_Freq_5[1][0] = 208;
			MSG_Freq_5[1][1] = 160;
			MSG_Freq_5[1][2] = 0;
			/* Д */
			MSG_Freq_5[2][0] = 208;
			MSG_Freq_5[2][1] = 148;
			MSG_Freq_5[2][2] = 0;
			break;
	}
	drawString(180, 100, BLACK, WHITE, MSG_Freq_4, 3);
	drawString(180, 200, BLACK, WHITE, MSG_Freq_5, 3);
	Delay(100000);
}

void LCD_Show_No_Result() {
	if (WorkStatus == 0) {
		if (NoResultState) return;
		fillRectangle(155, 100, 500, 500, WHITE);
		uint8_t MSG_DQPSK_5[2][3] = {"?", "?"};
		drawString(220, 100, BLACK, WHITE, MSG_DQPSK_5, 2);
		drawString(220, 200, BLACK, WHITE, MSG_DQPSK_5, 2);
		drawString(220, 300, BLACK, WHITE, MSG_DQPSK_5, 2);
		drawString(220, 400, BLACK, WHITE, MSG_DQPSK_5, 2);
		NoResultState = 1;
		TMR_Disable(TMR4);
	} else {
		if (NoResultState) return;
		fillRectangle(155, 100, 500, 500, WHITE);
		uint8_t MSG_Freq_6[2][3] = {"?", "?"};
		drawString(180, 100, BLACK, WHITE, MSG_Freq_6, 2);
		drawString(180, 200, BLACK, WHITE, MSG_Freq_6, 2);
		drawString(180, 300, BLACK, WHITE, MSG_Freq_6, 2);
		NoResultState = 1;
		TMR_Disable(TMR4);
	}
}

void LCD_Result_Reset() {
	prevCode = 0x999;
	NoResultState = 0;
	NoResultCounter = 2;
}

void LCD_GPIO_Config() {
	GPIO_Config_T GPIO_InitStructure;
	
	RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOC);
	
	GPIO_ConfigStructInit(&GPIO_InitStructure);
    GPIO_InitStructure.pin = GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStructure.speed = GPIO_SPEED_100MHz;
    GPIO_InitStructure.mode = GPIO_MODE_OUT;
	GPIO_InitStructure.pupd = GPIO_PUPD_NOPULL;
    GPIO_Config(GPIOC, &GPIO_InitStructure);
	
	GPIOC->BSCL |= GPIO_PIN_4 | GPIO_PIN_5;
	
	RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB);
	
	GPIO_ConfigStructInit(&GPIO_InitStructure);
    GPIO_InitStructure.pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStructure.speed = GPIO_SPEED_100MHz;
    GPIO_InitStructure.mode = GPIO_MODE_OUT;
	GPIO_InitStructure.pupd = GPIO_PUPD_NOPULL;
    GPIO_Config(GPIOB, &GPIO_InitStructure);
	
	GPIOB->BSCH |= GPIO_PIN_0|GPIO_PIN_1;
	
	Delay(10000);
}

uint8_t buf[LCD_WIDTH * 2];

void fillRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color){
	for(int i=0; i<(LCD_WIDTH * 2); i+=2)
	{
		buf[i] = color >> 8;
		buf[i + 1] = color & 0xFF;
	}
	
	//set draw region
	setDrawWindow(x0, y0, x1, y1);
	
	DC_SET;
	CS_RESET;
	Delay(DELAY);
	for(int y=0; y<LCD_HEIGHT; y++) 
		SPI_Tx_Data(SPI, buf, LCD_WIDTH * 2);
		SPI_Wait_for_termination(SPI);
	CS_SET;
}

void drawPicture(uint16_t x0, uint16_t y0, const uint16_t* img){
	uint16_t w = img[0];
	uint16_t h = img[1];
	uint32_t bytes = 2;
	
	setDrawWindow(x0, y0, x0 + w - 1, y0 + h - 1);
	
	DC_SET;
	UPDATE;
	CS_RESET;
	UPDATE;
	for(int y=0; y<h; y++)
	{
		for(int i=0; i<(w * 2); i+=2, bytes++)
		{
			buf[i] = img[bytes] >> 8;
			buf[i + 1] = img[bytes] & 0xFF;
		}
		SPI_Tx_Data(SPI, buf, (w * 2));
		SPI_Wait_for_termination(SPI);
	}
	CS_SET;
}

void drawString(uint16_t x0, uint16_t y0, uint16_t color, uint16_t Background, uint8_t str[][3], uint8_t len){
	uint8_t index = 0;
	uint16_t carr_shif = 0;
	struct Symbol symb;
	while(index < len)
	{
		symb = drawChar(x0 + carr_shif, y0, str[index], color, Background);
		carr_shif += symb.width + 8 - symb.space_pix;
		index++;
	}
}

struct Symbol drawChar(uint16_t x, uint16_t y, uint8_t* c, uint16_t color, uint16_t Background){
	struct Symbol symb = getOffset(c);
	uint8_t byte;	
	uint16_t i = 0;
	uint16_t bytes = 44 * symb.width/8;
	
	setDrawWindow(x, y, x + symb.width - 1, y + 43);
	
	
	while(i < bytes) //all bytes
	{
		for(uint8_t j=0, n=0; j < symb.width/8; j++, i++) //one string
		{
			byte = gameplay_glyph_bitmap[symb.offset + i];
			for(uint8_t m=0; m<8; m++, n += 2) //8-bit (pixels)
			{
				if((byte & (1 << (7 - m))) != 0)
				{
					buf[n] = color >> 8;
					buf[n + 1] = color & 0xFF;
				}
				else
				{
					buf[n] = Background >> 8; //background color
					buf[n + 1] = Background & 0xFF;	//background color
				}
			}
		}
		DC_SET;
		CS_RESET;
		SPI_Tx_Data(SPI, buf, 16*symb.width/8);
		SPI_Wait_for_termination(SPI);
		Delay(DELAY);
		CS_SET;
	}
	
	return symb;
}

void setDrawWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
	//column
	writeCmdLCD(0x2A);
	writeDataLCD(x1 >> 8);
	writeDataLCD(x1 & 0xFF);
	writeDataLCD(x2 >> 8);
	writeDataLCD(x2 & 0xFF);
	//row
	writeCmdLCD(0x2B);
	writeDataLCD(y1 >> 8);
	writeDataLCD(y1 & 0xFF);
	writeDataLCD(y2 >> 8);
	writeDataLCD(y2 & 0xFF);
	//GRAM
	writeCmdLCD(0x2C);
}

void initLCD(void){
	hwResetLCD();
	//positive gamma
	writeCmdLCD(0xE0);
	writeDataLCD(0x00);
	writeDataLCD(0x07);
	writeDataLCD(0x10);
	writeDataLCD(0x09);
	writeDataLCD(0x17);
	writeDataLCD(0x0B);
	writeDataLCD(0x40);
	writeDataLCD(0x8A);
	writeDataLCD(0x4B);
	writeDataLCD(0x0A);
	writeDataLCD(0x0D);
	writeDataLCD(0x0F);
	writeDataLCD(0x15);
	writeDataLCD(0x16);
	writeDataLCD(0x0F);
	//negative gamma
	writeCmdLCD(0xE1);
	writeDataLCD(0x00);
	writeDataLCD(0x1A);
	writeDataLCD(0x1B);
	writeDataLCD(0x02);
	writeDataLCD(0x0D);
	writeDataLCD(0x05);
	writeDataLCD(0x30);
	writeDataLCD(0x35);
	writeDataLCD(0x43);
	writeDataLCD(0x02);
	writeDataLCD(0x0A);
	writeDataLCD(0x09);
	writeDataLCD(0x32);
	writeDataLCD(0x36);
	writeDataLCD(0x0F);
	//frame rate
	writeCmdLCD(0xB1);
	writeDataLCD(0xA0); //60Hz
	//display inversion control
	writeCmdLCD(0xB4);
	writeDataLCD(0x02); //2-dot
	//power control 1
	writeCmdLCD(0xC0);
	writeDataLCD(0x17); //Vreg1out
	writeDataLCD(0x15); //Vreg2out
	//power control 2
	writeCmdLCD(0xC1);
	writeDataLCD(0x41); //VGH, VGL
	//VCOM
	writeCmdLCD(0xC5);
	writeDataLCD(0x00);
	writeDataLCD(0x0A);
	writeDataLCD(0x80);
	//rgb/mcu interface control
	writeCmdLCD(0xB6);
	writeDataLCD(0x02); //mcu
	//Memory access
	writeCmdLCD(0x36);
	writeDataLCD(0x48); //album or portreit || 0xF8
	//Interface pixel format
	writeCmdLCD(0x3A);
	writeDataLCD(0x55); //0x55-18bit //16-bit serial mode
	//set image function
	writeCmdLCD(0xE9);
	writeDataLCD(0x00); //disable 24-bit data
	//adjust control
	writeCmdLCD(0xF7);
	writeDataLCD(0xA9);
	writeDataLCD(0x51);
	writeDataLCD(0x2C);
	writeDataLCD(0x82); //D7 stream loose
	//exit sleep
	writeCmdLCD(0x11);
	Delay(120*168*1000);
	//display on
	writeCmdLCD(0x29);

	onLCD();
}

void hwResetLCD(void){
	RST_RESET;
	Delay(120*168*100);
	RST_SET;
	Delay(500000);
}

void swResetLCD(void){
	writeCmdLCD(0x01);
	Delay(120*168*1000);
}

void onLCD(void){
	BL_SET;
}

void offLCD(void){
	BL_RESET;
}

void writeCmdLCD(uint8_t cmd){
	DC_RESET;
	CS_RESET;
	Delay(DELAY);
	SPI_Tx_Data(SPI, &cmd, 1);
	SPI_Wait_for_termination(SPI);
	Delay(DELAY);
	CS_SET;
	Delay(DELAY);
}

void writeDataLCD(uint8_t data){
	DC_SET;
	CS_RESET;
	Delay(DELAY);
	SPI_Tx_Data(SPI, &data, 1);
	SPI_Wait_for_termination(SPI);
	Delay(DELAY);
	CS_SET;
	Delay(DELAY);
}
