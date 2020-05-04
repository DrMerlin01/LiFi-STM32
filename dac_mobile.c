#include "dac_mobile.h"
///////////////////////////////////////////////////////////////////////////////////////////////
void DACMobileInit()
{
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//������������ ����� �
	GPIO_InitTypeDef PORTA;//�������� ������� ��� ����� �

	GPIO_StructInit (&PORTA);//������������� ���������
	PORTA.GPIO_Mode = GPIO_Mode_AIN;//���������� ���� ����� ������
	PORTA.GPIO_Pin = GPIO_Pin_5;// ���� �5
	PORTA.GPIO_Speed = GPIO_Speed_2MHz;//�������� ������
	GPIO_Init (GPIOA, &PORTA);//����������� �������� ����������� ���������

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);//�������� ������������ ������� 7

	//������ 7 ������� ������������ 1.17��� ���  5 ��, 2 ��� (������������ 5, � ������ 2400)
	//(24 000 000 / (4100 * 1000)) - 5 ��
	TIM7->PSC = 5;//������������
	TIM7->ARR = 600;// ��� 8 ���
	TIM_ITConfig(TIM7, TIM_DIER_UIE , ENABLE);//��������� ������ ��� ��� ���������� �������� ��������
	TIM_SelectOutputTrigger(TIM7, TIM_TRGOSource_Update);//�������� ����� ������ �������� �� ������������
	TIM_Cmd(TIM7, ENABLE);//��������� ������

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);//������������ DAC
	DAC_InitTypeDef DAC_InitStructure;//��������� ��� ���

	DAC_StructInit(&DAC_InitStructure); //������������� ���������
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T7_TRGO;//�������� ������� ��� ��� 7
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;//�� ���������� �����
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;//�������� �������� �����
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);//����������� �������� �������� ����������
	DAC_Cmd(DAC_Channel_2, ENABLE);//��������� ���
}
