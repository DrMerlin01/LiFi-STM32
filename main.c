#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dac.h"
#include "stm32f10x_dma.h"
#include "misc.h"

int Array[32]={9};//��� �������
int resultArray=0;
int Array2[8]={1,2,4,8,16,32,64,128};//������������ ��� �������������� ��������� � ������
extern int c, j=0;
extern int adc_value;
extern volatile char ADC_IT_AWD_FLAG;
int countArray = 0, flag=0, temp=0;
int count = -1;
int symbolCode=0;
int flagLed=0, f=0 , cc=0;

void TIM3_IRQHandler()//��������� ������ 3 ��� ������� ��� ���
{
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);//������� ��� ������������ �������



			//TIM_Cmd(TIM6, ENABLE);//��������� ������ 6



}

void TIM2_IRQHandler()//��������� ������� (�����)
{
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//������� ��� ������������ �������

	GPIO_SetBits(GPIOA,GPIO_Pin_8);

	if (count==-1 && f == 0) {
						count++;
						flag=1;//������������� ��� ������ ����� ���� � ���������� temp
						f++;
				}

	//adc_value = ADC_GetConversionValue(ADC1); ��� ��� ������
	adc_value = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_2); // ��� ����� ������
	if(count == 32){
		for(int i = 0;i < 32; i++){
			//symbolCode += Array[i]*Array2[i];
			if(i%2==0){
			//	number_output(Array[i]);
				//Lcd_write_str(" ");
				if(j<9){
					if(i!=1){
					//resultArray += Array[i]*Array2[j];
					j++;
					}
				}
			}
		}

		for(int i = 0;i < 32; i++){
			Array[i] = 0;
		}

		//Lcd_write_data(resultArray);
		//Lcd_write_str(" ");


		if (cc==0) {
			cc++;
			count = -1;
					flag = 0;
					f=0;
		}
		else {
			count = 0;
			flag=3;
			f=35;
		}
	}

	if(flag == 2){

			if((temp < adc_value)){
				Array[countArray] = 0;
				//number_output(Array[countArray]);
				countArray++;
				count++;
				temp = adc_value;
			} else if((adc_value < temp)){
				Array[countArray] = 1;
				//number_output(Array[countArray]);
				countArray++;
				count++;
				temp = adc_value;
			} else {
				if(count != 0) {
				Array[countArray]=Array[countArray-1];
				//number_output(Array[countArray]);
				countArray++;
				count++;
				}
			}

		/*if(flagLed==0){//��������� ���������� ����� � ���
			NVIC_InitTypeDef NVIC_InitStructure;//��������� ��������� ��� ����������� ���������� ���
			NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;//�������� ��� ����������
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//������������� ��������� ����������
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//������������� ������������ ����������
			NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;//��������� ����������
			NVIC_Init(&NVIC_InitStructure);
			flagLed=1;
		}*/
	}

	if(flag==1){
		temp = adc_value;//��� ��������� �������� ��������� (����� ����) � ���������� temp
		flag=2;//����� ���������� ����� ��� ��� ������ ��������
	}


    GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}

void main(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//�������� ��������� ��� ������������� ����� �0 �����������
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//�������� ������������ ������� 4
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//�������� ������������ ������� 2

	SystemInit();

	ADCInit();//������������� ���

	Init_lcd();//������������� �������
	Lcd_clear();//������� �������
	Lcd_goto(0,0);//��������� ������� � ������

	ADCMobileInit();
	DAC_DMAInit();//������������� ��� � ���

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//���� 0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//��������� �� ���� � ��������� � �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//������ ��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//������������� ������ ������ �����


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//���� ��� ������������� ���� �������� 6 � 2
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//��������� �� ���� � ��������� � �������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//������ ��������
		GPIO_Init(GPIOA, &GPIO_InitStructure);//������������� ������ ������ �����

	int a=0;	//���������� ��� ��������� �������� �� ���

	//������ 3 ������� ������������ 5 �� ��� 1.17 ��� ��� 2 ���
	TIM3->PSC = 5;//������������ 1000, 5, 5
	TIM3->ARR = 600;//������ 4100, 4100, 2400 ���� 1200
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//�� ���������� �������� ��������
	TIM_Cmd(TIM3, ENABLE);//��������� ������
	NVIC_EnableIRQ(TIM3_IRQn);//��������� ����������

	//������ 2 ������� ������������ 5 �� ��� 1.17 ��� ��� 2 ���
	TIM2->PSC = 5;//������������ 1000, 5, 5
	TIM2->ARR = 600;//������ 4100 ���� 1200
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//�� ���������� �������� ��������
	TIM_Cmd(TIM2, ENABLE);//��������� ������
	NVIC_EnableIRQ(TIM2_IRQn);//��������� ����������

    while (1){
			/*Lcd_goto(0,0);
    	    number_output(adc_value/1000);
    	    number_output((adc_value/100)%10);
    	    number_output(((adc_value%100)/10));
    	    number_output(adc_value%10);*/
    }
}
