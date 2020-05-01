#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dac.h"
#include "stm32f10x_dma.h"
#include "misc.h"

int Array[32]={9};//для символа
int resultArray=0;
int Array2[8]={1,2,4,8,16,32,64,128};//коэффициенты для преобразования кодировки в символ
extern int c, j=0;
extern int adc_value;
extern volatile char ADC_IT_AWD_FLAG;
int countArray = 0, flag=0, temp=0;
int count = -1;
int symbolCode=0;
int flagLed=0, f=0 , cc=0;

void TIM3_IRQHandler()//запумкаем таймер 3 для запуска цап дма
{
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);//очищаем бит переполнения таймера



			//TIM_Cmd(TIM6, ENABLE);//Запускаем таймер 6



}

void TIM2_IRQHandler()//обработка даннных (прием)
{
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//очищаем бит переполнения таймера

	GPIO_SetBits(GPIOA,GPIO_Pin_8);

	if (count==-1 && f == 0) {
						count++;
						flag=1;//устанавливаем для записи старт бита в переменную temp
						f++;
				}

	//adc_value = ADC_GetConversionValue(ADC1); ДЛЯ РЕГ КАНАЛА
	adc_value = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_2); // ДЛЯ ИНЖЕК КАНАЛА
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

		/*if(flagLed==0){//отключаем сторожевой режим у АЦП
			NVIC_InitTypeDef NVIC_InitStructure;//Объявляем структуру для регистрации прирывания АЦП
			NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;//Выбирает тип прерывания
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//Устанавливаем приоритет прерывания
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//Устанавливаем подприоритет прерывания
			NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;//Запускаем прерывание
			NVIC_Init(&NVIC_InitStructure);
			flagLed=1;
		}*/
	}

	if(flag==1){
		temp = adc_value;//для получения значения сравнения (старт бита) в переменную temp
		flag=2;//чтобы пропустить старт бит при записи значения
	}


    GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}

void main(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//создание структуры для инициализации порта А0 контроллера
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//включаем тактирование таймера 4
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//включаем тактирование таймера 2

	SystemInit();

	ADCInit();//инициализация АЦП

	Init_lcd();//Инициализация дисплея
	Lcd_clear();//Очистка дисплея
	Lcd_goto(0,0);//Установка курсора в начало

	ADCMobileInit();
	DAC_DMAInit();//инициализация ЦАП и ДМА

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//Порт 0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//Настройка на вход с подтяжкой к питанию
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//Низкая скорость
	GPIO_Init(GPIOA, &GPIO_InitStructure);//инициализация режима работы порта


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//порт для синхронизации двух таймеров 6 и 2
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//Настройка на вход с подтяжкой к питанию
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//Низкая скорость
		GPIO_Init(GPIOA, &GPIO_InitStructure);//инициализация режима работы порта

	int a=0;	//переменная для получения значения из АЦП

	//Таймер 3 частота срабатывания 5 Гц или 1.17 кГц или 2 кГц
	TIM3->PSC = 5;//предделитель 1000, 5, 5
	TIM3->ARR = 600;//период 4100, 4100, 2400 БЫЛО 1200
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//по обновлению счетного регистра
	TIM_Cmd(TIM3, ENABLE);//Запускаем таймер
	NVIC_EnableIRQ(TIM3_IRQn);//Разрешаем прерывание

	//Таймер 2 частота срабатывания 5 Гц или 1.17 кГц или 2 кГц
	TIM2->PSC = 5;//предделитель 1000, 5, 5
	TIM2->ARR = 600;//период 4100 БЫЛО 1200
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//по обновлению счетного регистра
	TIM_Cmd(TIM2, ENABLE);//Запускаем таймер
	NVIC_EnableIRQ(TIM2_IRQn);//Разрешаем прерывание

    while (1){
			/*Lcd_goto(0,0);
    	    number_output(adc_value/1000);
    	    number_output((adc_value/100)%10);
    	    number_output(((adc_value%100)/10));
    	    number_output(adc_value%10);*/
    }
}
