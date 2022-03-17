#include "dac_dma.h"

void InitDACAndDMA(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  
	GPIO_InitTypeDef PORTA;    

	GPIO_StructInit(&PORTA); 
	PORTA.GPIO_Mode = GPIO_Mode_AIN;   
	PORTA.GPIO_Pin = GPIO_Pin_4;
	PORTA.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_Init(GPIOA, &PORTA);   

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	TIM6->PSC = 5;
	TIM6->ARR = 600;
	TIM_ITConfig(TIM6, TIM_DIER_UIE, ENABLE);      
	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);     
	TIM_Cmd(TIM6, ENABLE); 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	DAC_InitTypeDef DAC_InitStructure;  

	DAC_StructInit(&DAC_InitStructure);  
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;  
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;  
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);   
	DAC_Cmd(DAC_Channel_1, ENABLE); 

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_InitTypeDef DMA_InitStructure;  

	DMA_DeInit(DMA1_Channel3);  
	DMA_StructInit(&DMA_InitStructure); 
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(DAC->DHR12R1);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&data_for_dma;  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;   
	DMA_InitStructure.DMA_BufferSize = 1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;      
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;     
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;     
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;     
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;   
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;     
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);   

	DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
	NVIC_EnableIRQ(DMA1_Channel3_IRQn);  

	DMA_Cmd(DMA1_Channel3, ENABLE); 
	DAC_DMACmd(DAC_Channel_1, ENABLE);    
}

void DMA1_Channel3_IRQHandler(void) {
	if (timer_activated == 1) {
		TIM3->ARR = 300;
	} else {
		++timer_activated;
	}
	DMA1->IFCR |= DMA_ISR_TCIF3;   
}
