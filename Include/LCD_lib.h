#ifndef MY_LIB_LCD_LIB_H_
#define MY_LIB_LCD_LIB_H_

#include "main.h"

//macros
#define CS_SET		GPIOB->BSCL |= GPIO_PIN_1
#define CS_RESET	GPIOB->BSCH |= GPIO_PIN_1
#define DC_SET		GPIOB->BSCL |= GPIO_PIN_0
#define DC_RESET	GPIOB->BSCH |= GPIO_PIN_0
#define BL_SET		GPIOC->BSCL |= GPIO_PIN_4
#define BL_RESET	GPIOC->BSCH |= GPIO_PIN_4
#define RST_SET		GPIOC->BSCL |= GPIO_PIN_5
#define RST_RESET	GPIOC->BSCH |= GPIO_PIN_5
#define UPDATE		Delay(0);

//constans
#define LCD_WIDTH	480
#define LCD_HEIGHT	320

//colors
#define BLACK       0x0000
#define NAVY        0x000F
#define DARKGREEN   0x03E0
#define DARKCYAN    0x03EF
#define MAROON      0x7800
#define PURPLE      0x780F
#define OLIVE       0x7BE0
#define LIGHTGREY   0xC618
#define DARKGREY    0x7BEF
#define BLUE        0x001F
#define GREEN       0x07E0
#define CYAN        0x07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF
#define ORANGE      0xFD20
#define GREENYELLOW 0xAFE5
#define PINK        0xF81F

extern SPI_T* SPI;
extern uint8_t WorkStatus;
extern uint8_t NoResultCounter;

//tools
void LCD_Start(void);
void LCD_DQPSK_mode(void);
void LCD_Freq_mode(void);
void LCD_DQPSK_Result(uint16_t, uint16_t, double);
void LCD_Show_No_Result(void);
void LCD_Result_Reset(void);
void LCD_Freq_Result(uint16_t, double);
void LCD_GPIO_Config(void);
void initLCD(void);
void hwResetLCD(void);
void swResetLCD(void);
void onLCD(void);
void offLCD(void);
void writeCmdLCD(uint8_t);
void writeDataLCD(uint8_t);

//draw
void fillRectangle(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
void setDrawWindow(uint16_t, uint16_t, uint16_t, uint16_t);
void drawPicture(uint16_t, uint16_t, const uint16_t* img);
void drawFloatMonoWidth(uint16_t, uint16_t, uint16_t, uint16_t, uint8_t str[][3], uint8_t);
void drawString(uint16_t, uint16_t, uint16_t, uint16_t, uint8_t[][3], uint8_t len);
struct Symbol drawChar(uint16_t, uint16_t, uint8_t*, uint16_t, uint16_t);

#endif
