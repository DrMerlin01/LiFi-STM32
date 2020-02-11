#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_tim.h"
#include "misc.h"


float adc_value = 0;
volatile char ADC_IT_AWD_FLAG = 0;
int cnts = 0;
///////////////////////////////////////////////////////////////////////////////////////////////
void ADCInit();
