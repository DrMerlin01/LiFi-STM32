#include "adc.h"

void InitADC() {
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	NVIC_InitTypeDef NVIC_InitStructure;     
	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  

	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigInjecConv_None;    
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent ;    
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; 
	ADC_Init(ADC1, &ADC_InitStructure); 
 
	ADC_InjectedSequencerLengthConfig(ADC1, 1);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);

	ADC_AnalogWatchdogThresholdsConfig(ADC1, 1570, 1185);
	ADC_AnalogWatchdogSingleChannelConfig(ADC1, ADC_Channel_1);       
	ADC_AnalogWatchdogCmd(ADC1, ADC_AnalogWatchdog_SingleRegEnable);       

	ADC_ITConfig(ADC1, ADC_IT_AWD, ENABLE);    
	ADC_Cmd(ADC1, ENABLE); 
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_AutoInjectedConvCmd(ADC1, ENABLE );
	ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE);
}

void ADC1_IRQHandler() {
    if(ADC_GetITStatus(ADC1, ADC_IT_AWD)) {
        ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);  
    }
}
