#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_dac.h"
#include "stdlib.h"
#include "stdio.h"

int firstWorkTimer=0;
uint16_t dataForSend[1] = {0};

void DAC_DMAInit(void);
