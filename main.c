#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dac.h"
#include "stm32f10x_dma.h"
#include "misc.h"

int Array[20]={9};//для символа
int resultArray[9]={0};
int Array2[8]={1,2,4,8,16,32,64,128};//коэффициенты для преобразования кодировки в символ
extern int c, j=0;
extern int adc_value;
extern int cnts;
extern volatile char ADC_IT_AWD_FLAG;
int countArray = 0;
extern int count = -1, flag=0, temp=0;
int symbolCode=0;
int firstWorkTimer=0;
extern int f=0;//временно для теста

void TIM4_IRQHandler()//запумкаем таймер 6 для запуска цап дма
{
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);//очищаем бит переполнения таймера

    if (firstWorkTimer==1)//синхронизация ацп и цап
	{
		TIM4->ARR = 2300;
	}

    if (c == 1){
    	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1){//проверяем сигнал на ножке
    		TIM_Cmd(TIM6, ENABLE);//Запускаем таймер 6
			//GPIO_SetBits(GPIOA,GPIO_Pin_8);
			c = 0;
    	}
    }

    firstWorkTimer++;
}

void TIM2_IRQHandler()//обработка даннных (прием)
{
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//очищаем бит переполнения таймера

	adc_value = ADC_GetConversionValue(ADC1);

	if(count == 20){
		for(int i = 0;i < 20; i++){
			if(i%2!=0){
				number_output(Array[i]);
				/*if(j<9){
					resultArray[j] = Array[i];
					j++;
				}*/
			}
		}

		/*for(int i = 0; i < 9; i++){
			symbolCode += resultArray[i]*Array2[i-1];//получение кода символа
			resultArray[i]=0;
		}*/

		//Lcd_write_data(symbolCode);
		//Lcd_write_str(" ");
		//symbolCode=0;//очищаем переменную в которой хранится код символа
		count=0;
		flag=0;
		//if(f==0) f++;
		//else count=0;

		if (f == 0) {
			TIM_Cmd(TIM2, DISABLE);//Запускаем таймер
			NVIC_DisableIRQ(TIM2_IRQn);//Разрешаем прерывание
			cnts--;
			f++;
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
	}

	if(flag==1){
		temp = adc_value;//для получения значения сравнения (старт бита) в переменную temp
		flag=2;//чтобы пропустить старт бит при записи значения
	}
}

void main(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//создание структуры для инициализации порта А0 контроллера
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);//включаем тактирование таймера 4
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//включаем тактирование таймера 2

	SystemInit();

	ADCInit();//инициализация АЦП

	Init_lcd();//Инициализация дисплея
	Lcd_clear();//Очистка дисплея
	Lcd_goto(0,0);//Установка курсора в начало

	DAC_DMAInit();//инициализация ЦАП и ДМА

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//Порт 0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//Настройка на вход с подтяжкой к питанию
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//Низкая скорость
	GPIO_Init(GPIOA, &GPIO_InitStructure);//инициализация режима работы порта

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//порт для синхронизации двух таймеров 6 и 2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//Настройка на вход с подтяжкой к питанию
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//Низкая скорость
	GPIO_Init(GPIOA, &GPIO_InitStructure);//инициализация режима работы порта

	//Таймер 4 частота срабатывания 5 Гц или 1.17 кГц или 2 кГц
	TIM4->PSC = 5;//предделитель 1000, 5, 5
	TIM4->ARR = 1200;//период 4100, 4100, 2400
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);//по обновлению счетного регистра
	TIM_Cmd(TIM4, ENABLE);//Запускаем таймер
	NVIC_EnableIRQ(TIM4_IRQn);//Разрешаем прерывание

	//Таймер 2 частота срабатывания 5 Гц или 1.17 кГц или 2 кГц
	TIM2->PSC = 5;//предделитель 1000, 5, 5
	TIM2->ARR = 1200;//период 4100
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//по обновлению счетного регистра


    while (1){
			/*Lcd_goto(0,0);
    	    number_output(adc_value/1000);
    	    number_output((adc_value/100)%10);
    	    number_output(((adc_value%100)/10));
    	    number_output(adc_value%10);*/
    }
}
