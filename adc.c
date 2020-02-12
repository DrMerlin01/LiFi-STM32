#include "adc.h"

void ADCInit()
{
	GPIO_InitTypeDef GPIO_InitStructure;//��������� ��� ��������� ����� �
	ADC_InitTypeDef ADC_InitStructure;//��������� ��� ��������� ���

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//�������� 24���

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE ) ;//������������� ����� �
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE ) ;//������������ ���

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//���������� ����� 1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//�� ���� ��� ���������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//������ ��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��������� ���������

	/*ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//����������� ������ ���
	//ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//������������ ���������� ������
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;//�� �������� ������� ����.
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent ;//����������� ����� ������
	//ADC_InitStructure.ADC_NbrOfChannel = 1;//����� �������, � �������� �������� 2 ����
	//ADC_InitStructure.ADC_ScanConvMode = ENABLE;//������������ �������
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5);
	ADC_Init(ADC1, &ADC_InitStructure);//��������� ���������*/

	NVIC_InitTypeDef NVIC_InitStructure;//��������� ��������� ��� ����������� ���������� ���
	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;//�������� ��� ����������
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//������������� ��������� ����������
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//������������� ������������ ����������
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//��������� ����������
	NVIC_Init(&NVIC_InitStructure);//��������� ��������� ���������

	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//����������� ������ ���
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//������������ ���������� ������
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//������ ��� ��������
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent ;//����������� ����� ������ ����� ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;//����� �������, � �������� ��������
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;//������������ �������
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);//����� �������������� ��� 1 ������
	ADC_Init(ADC1, &ADC_InitStructure);//��������� ���������

	ADC_AnalogWatchdogThresholdsConfig(ADC1, 1570, 1185);//���������� ������� ������������ ��� ���1
	ADC_AnalogWatchdogSingleChannelConfig(ADC1, ADC_Channel_1);//����� �� ������� �������� ��� � ���������� ������
	ADC_AnalogWatchdogCmd(ADC1, ADC_AnalogWatchdog_SingleRegEnable);//�������� ���������� ���������� ������ �� ����� ������� ������

	ADC_ITConfig(ADC1, ADC_IT_AWD, ENABLE);//��������� ���������� �� ����������� �������

	//ADC_ExternalTrigConvCmd( ADC1, ENABLE );//������� ����

	ADC_Cmd(ADC1, ENABLE);//��������� ���

	ADC_ResetCalibration(ADC1);//�������� ������� ���������� ������ ���1
	while(ADC_GetResetCalibrationStatus(ADC1));//�������� ����� �������������� �������� ������ ���1
	ADC_StartCalibration(ADC1);//������ ���������� ���1
	while(ADC_GetCalibrationStatus(ADC1));//�������� ��������� ���������� ���1

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//������ �������������� � ����������� ������, ��� ��� ��������� ������������ ������

	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//�������� ������������ ������� 3

	//������ 3 ������� ������������ 5 �� ��� 1.17 ��� ��� 2 ���
	/*TIM3->PSC = 1000;//������������ 1000, 5, 5
	TIM3->ARR = 4100;//������ 4100, 4100, 2400
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//��������� ������ ��� ��� ���������� �������� ��������
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);//�������� ����� ������ �������� �� ������������
	TIM_Cmd(TIM3, ENABLE);//��������� ������
	NVIC_EnableIRQ(TIM3_IRQn);//��������� ���������� �� ������� 3*/
}

/*void TIM3_IRQHandler()
{
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);//������� ��� ������������ �������
    adc_value = ADC_GetConversionValue(ADC1);//�������������� ���
}*/

void ADC1_IRQHandler()
{
    if(ADC_GetITStatus(ADC1, ADC_IT_AWD))//��������� ���������� �� ��� ���������� �� ����������� �������
    {
    	if (cnts == 0) {
    		TIM_Cmd(TIM2, ENABLE);//��������� ������
    		NVIC_EnableIRQ(TIM2_IRQn);//��������� ����������
    		cnts++;
    		f--;
		}

    	if (count == -1) {
			count++;
			temp=0;
			flag=1;//������������� ��� ������ ����� ���� � ���������� temp
		}

        ADC_IT_AWD_FLAG = 1;//������������� ���������� ����, ��� ���� ������� ������ � 1
        ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);//������� ��� ����������
    }
}
