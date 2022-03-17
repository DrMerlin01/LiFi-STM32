#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_dac.h"
#include "stdlib.h"
#include "stdio.h"

int timer_activated = 0;
uint16_t data_for_dma[1] = {0};

void InitDACAndDMA(void);
