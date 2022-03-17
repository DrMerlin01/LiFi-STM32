#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dac.h"
#include "stm32f10x_dma.h"
#include "misc.h"

extern int adc_value;
int flag = 0;
int temp = 0;
int symbol_value = 0;

void TIM2_IRQHandler() {
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	adc_value = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_2);

	if (flag == 1) {
		if (temp < adc_value) {
			DAC_SetChannel2Data(DAC_Channel_2, 0);
			symbol_value = 0;
			temp = adc_value;
		} else if (adc_value < temp) {
			DAC_SetChannel2Data(DAC_Channel_2, 4095);
			symbol_value = 1;
			temp = adc_value;
		} else {
			if (symbol_value == 1) { 
				DAC_SetChannel2Data(DAC_Channel_2, 4095);
			} else {
				DAC_SetChannel2Data(DAC_Channel_2, 0);
			}
		}
	}

	if (flag == 0) {
		temp = adc_value;
		flag = 1;      
	}
}

void main(void) {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	SystemInit();
	InitADC(); 
	InitADCMobile();
	InitDACMobile();
	InitDACAndDMA();   

	TIM2->PSC = 5;
	TIM2->ARR = 600;
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);   
	TIM_Cmd(TIM2, ENABLE); 
	NVIC_EnableIRQ(TIM2_IRQn); 

    while (1) {

	}
}
