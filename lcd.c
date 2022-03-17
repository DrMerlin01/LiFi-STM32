#include "lcd.h"

void WriteStrLCD(uc8 *STRING) {
	char c;   

	while (c = *STRING++) {   
		if (c >= 192) {
			WriteDataLCD(russian[c - 192]);    
		} else {
			WriteDataLCD(c); 
		}
	}
}

void SetLCDPosition(const uc8 x, const uc8 y) {
	switch (x) {
		case 0:  
			WriteCmdLCD(y | 0x80);
			break;
		case 1:
			WriteCmdLCD(0x40 + y | 0x80);
			break;
		case 2:
			WriteCmdLCD(0x10 + y | 0x80);
			break;
		case 3:  
			WriteCmdLCD(0x50 + y | 0x80);
			break;
	}
}

void InitPinOut(void) {
	RCC_APB2PeriphClockCmd(INIT_PORT, ENABLE);  
	GPIO_InitTypeDef init_pin; 
	init_pin.GPIO_Pin = PIN_E | PIN_RS | PIN_RW | PIN_D7 | PIN_D6 | PIN_D5 | PIN_D4; 
	init_pin.GPIO_Mode = GPIO_Mode_Out_PP; 
	init_pin.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(PORT, &init_pin);   
}

void InitPinIn(void) {
	RCC_APB2PeriphClockCmd(INIT_PORT, ENABLE);  
	GPIO_InitTypeDef init_pin; 
	init_pin.GPIO_Pin = PIN_D7 | PIN_D6 | PIN_D5 | PIN_D4 ; 
	init_pin.GPIO_Mode = GPIO_Mode_IPD; 
	init_pin.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(PORT, &init_pin);   
}

void WriteCmdLCD(uc8 cmd) {
	InitPinOut();   
	RunDelay(24000);
	SET_RS_0;    
	GPIO_Write(PORT, (cmd >> 4) << LCD_SHIFT);  
	SET_E_1; 
	RunDelay(10);
	SET_E_0; 
	GPIO_Write(PORT, 0x00 << LCD_SHIFT);
	GPIO_Write(PORT, (cmd & 0x0F) << LCD_SHIFT);  
	RunDelay(10);
	SET_E_1; 
	RunDelay(10);
	SET_E_0;
	SET_RS_0;
	SET_RW_0; 
}

void WriteDataLCD(uint8_t data) {
	InitPinOut();   
	GPIO_Write(PORT, (data >> 4) << LCD_SHIFT);  
	SET_E_1;
	SET_RS_1;  
	RunDelay(10);
	SET_E_0; 
	GPIO_Write(PORT, 0x00 << LCD_SHIFT);
	GPIO_Write(PORT, (data & 0x0F) << LCD_SHIFT);  
	RunDelay(10);
	SET_E_1;
	SET_RS_1; 
	RunDelay(10);
	SET_E_0;
	SET_RS_0;
	SET_RW_0;
	GPIO_Write(PORT, 0x00 << LCD_SHIFT);
}

void InitLCD(void) {
	InitPinOut();   
	RunDelay(72000);
	WriteCmdLCD(FUNCTION_SET);
	RunDelay(72000);
	WriteCmdLCD(DISPLAY_ON_OFF_CONTROL);
	RunDelay(72000);
	WriteCmdLCD(DISPLAY_CLEAR); 
	RunDelay(72000);
    WriteCmdLCD(ENTRY_MODE_SET);  
}

void ClearLCD(void) {
	WriteCmdLCD(DISPLAY_CLEAR);   
}

void ReturnHome(void) {
	WriteCmdLCD(0b0000001);    
}

void PrintDefinechar(const uint8_t *pc, uint8_t char_code) {
	uint8_t code = (char_code << 3) | 0x40;
	 
	for (uint16_t i = 0; i < 8; ++i) {
		WriteCmdLCD(code++);   
		WriteDataLCD(pc[i]); 
	}
}

void WriteADCNumberLCD(const float number) {
	int integer_part, fractional_part, int_from_frac, frac_from_frac;
	float edit_adc_value;

	edit_adc_value = (3.3 / 4096.0) * number * 1000;   

	integer_part = edit_adc_value / 1000;    
	PrintNumber(integer_part);  
	WriteStrLCD(".");

	if (integer_part != 0) {
		fractional_part = (int)edit_adc_value % 1000;  
	}

	int_from_frac = fractional_part / 100;     
	PrintNumber(int_from_frac);

	frac_from_frac = fractional_part % 100;
	PrintNumber(frac_from_frac / 10);
	PrintNumber(frac_from_frac % 10);
	WriteStrLCD(" ");
}

void PrintNumber(const int number) {
	switch(number) {
		case 0:
			WriteStrLCD("0");
			break;
		case 1:
			WriteStrLCD("1");
			break;
		case 2:
			WriteStrLCD("2");
			break;
		case 3:
			WriteStrLCD("3");
			break;
		case 4:
			WriteStrLCD("4");
			break;
		case 5:
			WriteStrLCD("5");
			break;
		case 6:
			WriteStrLCD("6");
			break;
		case 7:
			WriteStrLCD("7");
			break;
		case 8:
			WriteStrLCD("8");
			break;
		case 9:
			WriteStrLCD("9");
			break;
		default:
			WriteStrLCD("error");
			break;
	}
}

void RunDelay(const int tick) {
	int delay = tick;
	while (--delay);
}
