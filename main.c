#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dac.h"
#include "stm32f10x_dma.h"
#include "misc.h"
///////////////////////////////////////////////////////////////////////////////////////////////
extern int adc_value;
int flag = 0; //флаг для первоначальной записи первого значения
int temp = 0; //для хранения пердыдущего значения из АЦП, для сравнения
int symbolValue = 0; //для сохранения значения бита, если подряд буду два одинаковых
///////////////////////////////////////////////////////////////////////////////////////////////
void TIM2_IRQHandler()//обработка даннных (прием)
{
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//очищаем бит переполнения таймера
	adc_value = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_2); // ДЛЯ ИНЖЕК КАНАЛА

	if(flag == 1){
		if((temp < adc_value)){
			DAC_SetChannel2Data(DAC_Channel_2, 0);
			symbolValue = 0;
			temp = adc_value;
		} else if((adc_value < temp)){
			DAC_SetChannel2Data(DAC_Channel_2, 4095);
			symbolValue = 1;
			temp = adc_value;
		} else {
			if(symbolValue == 1)DAC_SetChannel2Data(DAC_Channel_2, 4095);
			else DAC_SetChannel2Data(DAC_Channel_2, 0);
		}
	}

	if(flag==0){
		temp = adc_value;//для получения значения сравнения (старт бита) в переменную temp
		flag = 1;//чтобы пропустить старт бит при записи значения
	}
}

void main(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//включаем тактирование таймера 2

	SystemInit();
	ADCInit();//инициализация АЦП
	ADCMobileInit();
	DACMobileInit();
	DAC_DMAInit();//инициализация ЦАП и ДМА

	//Таймер 2 частота срабатывания 5 Гц или 1.17 кГц или 2 кГц
	TIM2->PSC = 5;//предделитель 1000, 5, 5
	TIM2->ARR = 600;//период 4100 БЫЛО 1200
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//по обновлению счетного регистра
	TIM_Cmd(TIM2, ENABLE);//Запускаем таймер
	NVIC_EnableIRQ(TIM2_IRQn);//Разрешаем прерывание

    while (1){}
}
