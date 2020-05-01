#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_tim.h"
#include "misc.h"

extern uint16_t dataForSend[1];
int countBit=0;
void ADCMobileInit();
