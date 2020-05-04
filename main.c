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
int flag = 0; //���� ��� �������������� ������ ������� ��������
int temp = 0; //��� �������� ����������� �������� �� ���, ��� ���������
int symbolValue = 0; //��� ���������� �������� ����, ���� ������ ���� ��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////
void TIM2_IRQHandler()//��������� ������� (�����)
{
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//������� ��� ������������ �������
	adc_value = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_2); // ��� ����� ������

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
		temp = adc_value;//��� ��������� �������� ��������� (����� ����) � ���������� temp
		flag = 1;//����� ���������� ����� ��� ��� ������ ��������
	}
}

void main(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//�������� ������������ ������� 2

	SystemInit();
	ADCInit();//������������� ���
	ADCMobileInit();
	DACMobileInit();
	DAC_DMAInit();//������������� ��� � ���

	//������ 2 ������� ������������ 5 �� ��� 1.17 ��� ��� 2 ���
	TIM2->PSC = 5;//������������ 1000, 5, 5
	TIM2->ARR = 600;//������ 4100 ���� 1200
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//�� ���������� �������� ��������
	TIM_Cmd(TIM2, ENABLE);//��������� ������
	NVIC_EnableIRQ(TIM2_IRQn);//��������� ����������

    while (1){}
}
