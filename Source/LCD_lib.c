/*!
 * @file        LCD_lib.c
 *
 * @brief       Функции для работы с LCD экраном
 *
 * @version     V1.0.0
 *
 * @date        02-10-2023
 */

#include "LCD_lib.h"

uint8_t MSG_Start_1[7][3] = {"Н", "А", "Ж", "М", "И", "Т", "Е"};
uint8_t MSG_Start_2[4][3] = {"К", "Е", "У", "1"};

uint8_t MSG_DQPSK_1[4][3] = {"А", "Р", "С", "Ф"};
uint8_t MSG_DQPSK_2[4][3] = {"К", "О", "Д", ":"};
uint8_t MSG_DQPSK_3[4][3] = {"К", "Р", "Ц", ":"};
uint8_t MSG_DQPSK_4[4][3] = {"С", "Р", "Ц", ":"};
uint8_t MSG_DQPSK_5[4][3] = {"А", "М", "П", ":"};

void LCD_Voltage_Result(uint16_t, uint16_t, double);

/*!
 * @brief     Запускает LCD экран
 *
 * Вызывает [LCD_GPIO_Config()](#LCD_GPIO_Config), [initLCD()](#initLCD),
 * закрашивает экран в белый, выводит на экран стартовое сообщение (MSG_Start_1, MSG_Start_2)
 */

void LCD_Start() {
	LCD_GPIO_Config();
	initLCD();
	fillRectangle(0, 0, LCD_HEIGHT, LCD_WIDTH, WHITE);
	drawString(10, 200, BLACK, WHITE, MSG_Start_1, 7);
	drawString(85, 250, BLACK, WHITE, MSG_Start_2, 4);
}

/*!
 * @brief     Переводит LCD экран в режим отображения АРСФ кодов
 *
 * Закрашивает экран в белый, выводит на экран обозначение режима работы (MSG_DQPSK_1) и поля
 * для отображения результатов АРСФ декодирования (MSG_DQPSK_2, MSG_DQPSK_3, MSG_DQPSK_4, MSG_DQPSK_5).
 * Также сбрасывает счётчик #NoResultCounter таймера [TIM4](#TMR4_EventCallback).
 */

void LCD_DQPSK_mode() {
	fillRectangle(0, 0, LCD_HEIGHT, LCD_WIDTH, WHITE);
	drawString(85, 5, RED, WHITE, MSG_DQPSK_1, 4);
	drawString(5, 100, BLACK, WHITE, MSG_DQPSK_2, 4);
	drawString(5, 200, BLACK, WHITE, MSG_DQPSK_3, 4);
	drawString(5, 300, BLACK, WHITE, MSG_DQPSK_4, 4);
	drawString(5, 400, BLACK, WHITE, MSG_DQPSK_5, 4);
	NoResultCounter = 2;
}

uint16_t prevCode = 0x999;      /*!< Содержит значение предыдущего принятого кода.
								 * 0x999 - невозможное значение, поэтому является default*/

uint8_t NoResultCounter = 3;	/*!< Зависит от количества безрезультатных (сигнал не удалось декодировать) приёмов сигнала подряд.
								 * extern переменная, уменьшается в [TMR4_EventCallback()](#TMR4_EventCallback) с периодом 500ms.
								 * Если NoResultCounter = 0 (т.е. в течении ~1 секунды не произошло удачного декодирования),
								 * то вызывается [LCD_Show_No_Result()](#LCD_Show_No_Result)*/

uint8_t NoResultState = 0;		/*!< Индикация отсутсвия отображения кода на LCD экране. 0 - код на экране примутсвует,
								 * 1 - кода нет*/

/*!
 * @brief     Отображения результата декодирования АРСФ на LCD экране
 * 
 * @param     code декодированный hex код
 * @param	  speed разрешённая на участке скорость в формате десятеричного числа[XXYY],
 *			  где XX - разрешенная скорость на КРЦ, YY - разрешенная скорость на СРЦ
 * @param	  AvgV средняя амплитуда принятого сигнала
 * 
 * Если code код равен #prevCode, то выводит только AvgV (предыдущие показания code, speed остаются на экране).
 *
 * Также при каждом вызове обновляет счетчик #NoResultCounter
 */

void LCD_DQPSK_Result(uint16_t code, uint16_t speed, double AvgV) {
	if (NoResultState) {
		prevCode = 0x999;
		TMR_Enable(TMR4);
	}
	NoResultCounter = 3;
	NoResultState = 0;
	
	if (prevCode == 0x999) fillRectangle(155, 100, LCD_WIDTH, LCD_WIDTH, WHITE);
	
	LCD_Voltage_Result(180, 400, AvgV);

	if (code == prevCode) return;
	prevCode = code;
	fillRectangle(155, 100, 350, 350, WHITE);
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

/*!
 * @brief     Переводит LCD экран в режим отображения АРС кодов
 *
 * Закрашивает экран в белый, выводит на экран обозначение режима работы (MSG_Freq_1) и поля
 * для отображения результатов АРС декодирования (MSG_Freq_2, MSG_Freq_3, MSG_Freq_6).
 * Также сбрасывает счётчик #NoResultCounter таймера [TIM4](#TMR4_EventCallback).
 */

void LCD_Freq_mode() {
	fillRectangle(0, 0, LCD_HEIGHT, LCD_WIDTH, WHITE);
	drawString(105, 5, RED, WHITE, MSG_Freq_1, 3);
	drawString(5, 100, BLACK, WHITE, MSG_Freq_2, 4);
	drawString(5, 200, BLACK, WHITE, MSG_Freq_3, 4);
	drawString(5, 300, BLACK, WHITE, MSG_Freq_6, 4);
	NoResultCounter = 2;
}

/*!
 * @brief     Отображения результата декодирования АРС на LCD экране
 * 
 * @param     freq доминирующая частота, полученная в результате спектрального анализа
 * @param	  AvgV средняя амплитуда принятого сигнала
 *
 * Если code код равен #prevCode, то выводит только AvgV (предыдущие показания freq остаются на экране).
 *
 * Также при каждом вызове обновляет счетчик #NoResultCounter
 */

void LCD_Freq_Result(uint16_t freq, double AvgV) {
	if (NoResultState) {
		prevCode = 0x999;
		TMR_Enable(TMR4);
	}
	NoResultCounter = 2;
	NoResultState = 0;
	
	if (prevCode == 0x999) fillRectangle(155, 100, LCD_WIDTH, LCD_WIDTH, WHITE);
	
	LCD_Voltage_Result(180, 300, AvgV);

	if (freq == prevCode) return;
	prevCode = freq;
	if (prevCode != 0x999) fillRectangle(155, 100, LCD_HEIGHT, LCD_HEIGHT, WHITE);
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

/*!
 * @brief     Отображение результата анализа амлитуды принятого сигнала на LCD экране
 * 
 * @param     x_pos точка начала отрисовки по оси х
 * @param	  y_pos точка начала отрисовки по оси y
 * @param	  AvgV средняя амплитуда принятого сигнала
 *
 * Выводит на на белом фоне в заданной позиции double с точностью два знака после запятой в формате X.XX
 */

void LCD_Voltage_Result(uint16_t x_pos, uint16_t y_pos, double AvgV) {
	uint8_t MSG_Voltage_1[4][3] = {"0", ".", "0", "0"};
	AvgV += 5;												//Округление по предпоследнему знаку
	int64_t V = (int) AvgV;
	
	toString(MSG_Voltage_1[0], V/1000);
	toString(MSG_Voltage_1[2], (V % 1000)/100);
	toString(MSG_Voltage_1[3], (V % 100)/10);
	
	drawFloatMonoWidth(x_pos, y_pos, BLACK, WHITE, MSG_Voltage_1, 4);
}

/*!
 * @brief     Отображение состояния 
 * 
 * @param     x_pos точка начала отрисовки по оси х
 * @param	  y_pos точка начала отрисовки по оси y
 * @param	  AvgV средняя амплитуда принятого сигнала
 *
 * Выводит на на белом фоне в заданной позиции double с точностью два знака после запятой в формате X.XX
 */

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

/*!
 * @brief     Очищает информацию о предыдущих полученных результатах
 *
 * Функция возвращает переменные #prevCode, #NoResultState и #NoResultCounter к дефолтным значениям
 */

void LCD_Result_Reset() {
	prevCode = 0x999;
	NoResultState = 0;
	NoResultCounter = 2;
}

/*!
 * @brief     Инициализация GPIO ножек для работы с LCD экраном
 *
 * Дефолтные значения пинов:
 * - PC4 (LCD_BL) - High
 * - PC5 (LCD_RST) - High
 * - PB0 (LCD_DC) - Low
 * - PB1 (LCD_CS) - Low
 */

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

uint8_t buf[LCD_WIDTH * 2]; /*!< Содержит данные, передаваемые контроллеру ILI9488 по SPI*/

/*!
 * @brief     Отрисовка прямоугольника с заливкой на LCD экране
 * 
 * @param     x0 точка начала отрисовки по оси х
 * @param	  y0 точка начала отрисовки по оси y
 * @param     x1 точка конца отрисовки по оси х
 * @param	  y1 точка конца отрисовки по оси y
 * @param	  color цвет заливки в формате RGB565
 *
 * Выводит на экран прямоугольник с заливкой в заданной позиции.
 */

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
	for(int y=0; y<LCD_HEIGHT; y++) 
		SPI_Tx_Data(SPI, buf, LCD_WIDTH * 2);
		SPI_Wait_for_termination(SPI);
	CS_SET;
}

/*!
* @brief     Отрисовка изображения на LCD экране
 * 
 * @param     x0 точка начала отрисовки по оси х
 * @param	  y0 точка начала отрисовки по оси y
 * @param	  img ссылка на массив, содержащий изображение для отрисовки.
 *
 * Выводит на экран изображение в заданной позиции.
 */

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

/*!
 * @brief     Вывод на экран строки символов
 * 
 * @param     x0 точка начала отрисовки по оси х
 * @param	  y0 точка начала отрисовки по оси y
 * @param	  color цвет заливки символов в формате RGB565
 * @param	  Background цвет заливки фона в формате RGB565
 * @param     str массив символов-строк в формате UTF-8, образующий отрисовываемую строку
 * @param	  len количество символов-строк, образующих отрисовываемую строку
 *
 * Выводит на экран строку символов с постоянным расстоянием (8 пикселей) между символами
 */

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

/*!
 * @brief     Вывод на экран дробного числа
 * 
 * @param     x0 точка начала отрисовки по оси х
 * @param	  y0 точка начала отрисовки по оси y
 * @param	  color цвет заливки символов в формате RGB565
 * @param	  Background цвет заливки фона в формате RGB565
 * @param     str массив символов-строк в формате UTF-8, образующий отрисовываемую строку
 * @param	  len количество символов-строк, образующих отрисовываемую строку
 *
 * Выводит на экран дробное число в формате X.XX, которое всегда будет одинаковой ширины
 */

void drawFloatMonoWidth(uint16_t x0, uint16_t y0, uint16_t color, uint16_t Background, uint8_t str[][3], uint8_t len) {
	uint8_t index = 0;
	uint16_t carr_shif = 0;
	struct Symbol symb;
	uint8_t space[3] = {95, 0, 0};
	while(index < len)
	{
		if ((str[index][0]*1000000 + str[index][1]*1000 + str[index][2]) == 49000000) {
			drawChar(x0 + carr_shif, y0, space, color, Background);
			carr_shif += 8;
			drawChar(x0 + carr_shif, y0, space, color, Background);
			carr_shif += 7;
		}
		symb = drawChar(x0 + carr_shif, y0, str[index], color, Background);
		carr_shif += symb.width + 8 - symb.space_pix;
		index++;
	}
	drawChar(x0 + carr_shif, y0, space, color, Background);
}

/*!
 * @brief     Вывод на экран дробного числа
 * 
 * @param     x0 точка начала отрисовки по оси х
 * @param	  y0 точка начала отрисовки по оси y
 * @param	  с символов-строка в формате UTF-8
 * @param	  color цвет заливки символов в формате RGB565
 * @param	  Background цвет заливки фона в формате RGB565
 *
 * @return    Symbol symbol структура, описывающая отрисованный символ. Подробнее описана в #getOffset
 *
 * Выводит на экран символ с заданным цветом color, закрашивает фон цветом Background
 */

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
		CS_SET;
	}
	
	return symb;
}

/*!
 * @brief     Установка окна для отрисовки на LCD экране
 * 
 * @param     x1 точка начала отрисовки по оси х
 * @param	  y1 точка начала отрисовки по оси y
 * @param     x2 точка конца отрисовки по оси х
 * @param	  y2 точка конца отрисовки по оси y
 *
 * Устанавливает прямоугольную область для отрисовки. Выход за пределы области в лучшем случае
 * повлияет только на время отрисовки, в худшем вызовет графические артефакты, но не остановит 
 * выполнение программы.
 */

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

/*!
 * @brief     Инициализация LCD экрана
 *
 * Функция отправляет команды на чип ILI9488 по шине SPI для установки необходимого
 * режима работы LCD экрана
 */

void initLCD(){
	hwResetLCD();
	
	writeCmdLCD(0xE0);  //positive gamma
	writeDataLCD(0x00);
	writeDataLCD(0x2C);
	writeDataLCD(0x2C);
	writeDataLCD(0x0B);
	writeDataLCD(0x0C);
	writeDataLCD(0x04);
	writeDataLCD(0x4C);
	writeDataLCD(0x64);
	writeDataLCD(0x36);
	writeDataLCD(0x03);
	writeDataLCD(0x0E);
	writeDataLCD(0x01);
	writeDataLCD(0x10);
	writeDataLCD(0x01);
	writeDataLCD(0x00);
	
	writeCmdLCD(0xE1);  //negative gamma
	writeDataLCD(0x0F);
	writeDataLCD(0x37);
	writeDataLCD(0x37);
	writeDataLCD(0x0C);
	writeDataLCD(0x0F);
	writeDataLCD(0x05);
	writeDataLCD(0x50);
	writeDataLCD(0x32);
	writeDataLCD(0x36);
	writeDataLCD(0x04);
	writeDataLCD(0x0B);
	writeDataLCD(0x00);
	writeDataLCD(0x19);
	writeDataLCD(0x14);
	writeDataLCD(0x0F);
	
	writeCmdLCD(0xB1);  //frame rate
	writeDataLCD(0xA0); //60Hz
	
	writeCmdLCD(0xB4);  //display inversion control
	writeDataLCD(0x02); //2-dot
	
	writeCmdLCD(0xC0);  //power control 1
	writeDataLCD(0x17); //Vreg1out
	writeDataLCD(0x15); //Vreg2out
	
	writeCmdLCD(0xC1);  //power control 2
	writeDataLCD(0x41); //VGH, VGL
	
	writeCmdLCD(0xC5);  //VCOM
	writeDataLCD(0x00);
	writeDataLCD(0x0A);
	writeDataLCD(0x80);
	
	writeCmdLCD(0xB6);  //rgb/mcu interface control
	writeDataLCD(0x02); //mcu
	
	writeCmdLCD(0x36);  //Memory access
	writeDataLCD(0x48); //album or portreit || 0xF8
	
	writeCmdLCD(0x3A);  //Interface pixel format
	writeDataLCD(0x55); //0x55-18bit
	
	writeCmdLCD(0xE9);  //set image function
	writeDataLCD(0x00); //disable 24-bit data
	
	writeCmdLCD(0xF7);  //adjust control
	writeDataLCD(0xA9);
	writeDataLCD(0x51);
	writeDataLCD(0x2C);
	writeDataLCD(0x82); //D7 stream loose
	
	writeCmdLCD(0x11);  //exit sleep
	Delay(120*168*1000);//required delay
	
	writeCmdLCD(0x29);  //display on

	onLCD();
}

/*!
 * @brief     hardware перезагрузка LCD экрана
 *
 * Функция устанавливает уровень Low на линии LCD_RST и после некоторой задержки
 * выставляет уровень High, что вызывает hardware перезагрузку LCD экрана.
 */

void hwResetLCD(void){
	RST_RESET;
	Delay(120*168*100);
	RST_SET;
	Delay(500000);
}

/*!
 * @brief     software перезагрузка LCD экрана
 *
 * Функция отправляет команду 0x01 по шине SPI, что вызывает software перезагрузку LCD экрана.
 * Также, согласно документации, после перезагрузки необходимо обеспечить задержку выполнения
 * остальных функций (установлена ~120 мс).
 */

void swResetLCD(void){
	writeCmdLCD(0x01);
	Delay(120*168*1000);
}

/*!
 * @brief     Включение LCD экрана
 *
 * Функция выставляет High уровень на линии LCD_BL
 */

void onLCD(void){
	BL_SET;
}

/*!
 * @brief     Выключение LCD экрана
 *
 * Функция выставляет Low уровень на линии LCD_BL
 */

void offLCD(void){
	BL_RESET;
}

/*!
 * @brief     Отправка команды на контроллер LCD экрана ILI9488
 *
 * Функция выставляет Low уровень на линии LCD_CS и линии LCD_DC, после чего выполняет отправку 
 * команды по SPI. Далее для соблюдения протокола функция выставляет High уровень на линии LCD_CS
 */

void writeCmdLCD(uint8_t cmd){
	DC_RESET;
	CS_RESET;
	SPI_Tx_Data(SPI, &cmd, 1);
	SPI_Wait_for_termination(SPI);
	CS_SET;
}

/*!
 * @brief     Отправка данных (параметров команд) на контроллер LCD экрана ILI9488
 *
 * Поведение аналогично (writeCmdLCD())[#writeCmdLCD], за исключением того, что при отправке
 * данных на линии LCD_DC устонавливается High уровень.
 */

void writeDataLCD(uint8_t data){
	DC_SET;
	CS_RESET;
	SPI_Tx_Data(SPI, &data, 1);
	SPI_Wait_for_termination(SPI);
	CS_SET;
}
