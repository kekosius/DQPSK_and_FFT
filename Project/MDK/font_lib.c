#include "font_lib.h"

struct Symbol getOffset(uint8_t* sym) {
	uint32_t code = sym[0]*1000000 + sym[1]*1000 + sym[2];
	switch (code) {
		case  32000000:
			return char_map[0];
		case  33000000:
			return char_map[1];
		case  39000000:
			return char_map[2];
		case  40000000:
			return char_map[3];
		case  41000000:
			return char_map[4];
		case  45000000:
			return char_map[5];
		case  46000000:
			return char_map[6];
		case  47000000:
			return char_map[7];
		case  48000000:
			return char_map[8];
		case  49000000:
			return char_map[9];
		case  50000000:
			return char_map[10];
		case  51000000:
			return char_map[11];
		case  52000000:
			return char_map[12];
		case  53000000:
			return char_map[13];
		case  54000000:
			return char_map[14];
		case  55000000:
			return char_map[15];
		case  56000000:
			return char_map[16];
		case  57000000:
			return char_map[17];
		case  58000000:
			return char_map[18];
		case  59000000:
			return char_map[19];
		case  63000000:
			return char_map[20];
		case 208144000:
			return char_map[21];
		case 208145000:
			return char_map[22];
		case 208146000:
			return char_map[23];
		case 208147000:
			return char_map[24];
		case 208148000:
			return char_map[25];
		case 208149000:
			return char_map[26];
		case 208150000:
			return char_map[27];
		case 208151000:
			return char_map[28];
		case 208152000:
			return char_map[29];
		case 208154000:
			return char_map[30];
		case 208155000:
			return char_map[30];
		case 208156000:
			return char_map[32];
		case 208157000:
			return char_map[33];
		case 208158000:
			return char_map[34];
		case 208159000:
			return char_map[35];
		case 208160000:
			return char_map[36];
		case 208161000:
			return char_map[37];
		case 208162000:
			return char_map[38];
		case 208163000:
			return char_map[39];
		case 208164000:
			return char_map[40];
		case 208165000:
			return char_map[41];
		case 208166000:
			return char_map[42];
		case 208167000:
			return char_map[43];
		case 208168000:
			return char_map[44];
		case 208171000:
			return char_map[45];
		case 208173000:
			return char_map[46];
		case 208174000:
			return char_map[47];
		case 208175000:
			return char_map[48];
		case  68000000:
			return char_map[49];
		case  70000000:
			return char_map[50];
		case 120000000:
			return char_map[51];
		default:
			return char_map[20];
	}
}

void CharToUChar(char* str, uint8_t* anw) {
	for (uint8_t i = 0; i < 3; i++) anw[i] = abs(str[i]);
	return;
}

void toString (uint8_t* str, uint16_t sym) {
	switch (sym) {
		case 0x00:
			CharToUChar("0", str);
			return;
		case 0x01:
			CharToUChar("1", str);
			return;
		case 0x02:
			CharToUChar("2", str);
			return;
		case 0x03:
			CharToUChar("3", str);
			return;
		case 0x04:
			CharToUChar("4", str);
			return;
		case 0x05:
			CharToUChar("5", str);
			return;
		case 0x06:
			CharToUChar("6", str);
			return;
		case 0x07:
			CharToUChar("7", str);
			return;
		case 0x08:
			CharToUChar("8", str);
			return;
		case 0x09:
			CharToUChar("9", str);
			return;
		case 0x0A:
			CharToUChar("А", str);
			return;
		case 0x0B:
			CharToUChar("В", str);
			return;
		case 0x0C:
			CharToUChar("С", str);
			return;
		case 0x0D:
			CharToUChar("D", str);
			return;
		case 0x0E:
			CharToUChar("Е", str);
			return;
		case 0x0F:
			CharToUChar("F", str);
			return;
		default:
			CharToUChar("?", str);
			return;
	}
}

