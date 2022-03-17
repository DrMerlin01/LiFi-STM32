#include "adc_mobile.h"

void InitADCMobile(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructureMobile;

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_InitStructureMobile.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructureMobile.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructureMobile.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructureMobile.ADC_NbrOfChannel = 1;
	ADC_InitStructureMobile.ADC_ScanConvMode = ENABLE;
	ADC_InitStructureMobile.ADC_ExternalTrigConv = ADC_ExternalTrigInjecConv_T4_TRGO; 
	ADC_Init(ADC1, &ADC_InitStructureMobile);
	ADC_InjectedSequencerLengthConfig(ADC1, 1);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_55Cycles5);

	NVIC_InitTypeDef NVIC_InitStructureADC;
	NVIC_InitStructureADC.NVIC_IRQChannel = ADC1_IRQn;
	NVIC_InitStructureADC.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructureADC.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructureADC.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructureADC);

	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_AutoInjectedConvCmd(ADC1, ENABLE );
	ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE);

	TIM4->PSC = 5;
	TIM4->ARR = 600;
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	TIM_SelectOutputTrigger(TIM4, TIM_TRGOSource_Update);
	TIM_Cmd(TIM4, ENABLE);
	NVIC_EnableIRQ(TIM4_IRQn);
}

void TIM4_IRQHandler() {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    int adc_value = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
    if (adc_value == 1) {
		data_for_dma[0] = 4000;
	} else {
		data_for_dma[0] = 2000;
	}
}
