#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#define PORT    	GPIOB
#define INIT_PORT 	RCC_APB2Periph_GPIOB
#define PIN_E 		GPIO_Pin_10
#define PIN_RW		GPIO_Pin_11
#define PIN_RS		GPIO_Pin_12

#define LCD_SHIFT	6
#define USE_GPIO	GPIO_Pin_9
#define PIN_D7		USE_GPIO
#define PIN_D6		USE_GPIO >> 1
#define PIN_D5		USE_GPIO >> 2
#define PIN_D4		USE_GPIO >> 3

#define FUNCTION_SET 				0b00100000
#define DISPLAY_ON_OFF_CONTROL		0b00001111
#define DISPLAY_CLEAR				0b00000001
#define ENTRY_MODE_SET				0b00000100

#define SET_RS_1	PORT->ODR |=  PIN_RS
#define SET_RS_0	PORT->ODR &=~ PIN_RS
#define SET_E_1 	PORT->ODR |=  PIN_E
#define SET_E_0		PORT->ODR &=~ PIN_E
#define SET_RW_1	PORT->ODR |=  PIN_RW
#define SET_RW_0	PORT->ODR &=~ PIN_RW

const unsigned char russian[] = { 0x41, 0xA0, 0x42, 0xA1, 0xE0, 0x45, 0xA3, 0xA4, 
                                  0xA5, 0xA6, 0x4B, 0xA7, 0x4D, 0x48, 0x4F, 0xA8, 
                                  0x50, 0x43, 0x54, 0xA9, 0xAA, 0x58, 0xE1, 0xAB, 
                                  0xAC, 0xE2, 0xAD, 0xAE, 0x62, 0xAF, 0xB0, 0xB1, 
                                  0x61, 0xB2, 0xB3, 0xB4, 0xE3, 0x65, 0xB6, 0xB7,
                                  0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0x6F, 0xBE, 
                                  0x70, 0x63, 0xBF, 0x79, 0xE4, 0x78, 0xE5, 0xC0, 
                                  0xC1, 0xE6, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7 };

void InitPinOut(void);

void InitPinIn(void);

void InitLCD(void);

void WriteDataLCD(uint8_t byte);

void WriteCmdLCD(uint8_t byte);

void ClearLCD(void);

void ReturnHome(void);

void SetLCDPosition(const uc8 x, const uc8 y);

void WriteStrLCD(uc8 *STRING);

void PrintDefinechar(const uint8_t *pc, uint8_t char_code);

void WriteADCNumberLCD(const float number);

void PrintNumber(const int number);

void RunDelay(const int tick);



