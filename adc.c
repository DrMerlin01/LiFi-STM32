#include "adc.h"

void ADCInit()
{
	GPIO_InitTypeDef GPIO_InitStructure;//структура для настройки порта А
	ADC_InitTypeDef ADC_InitStructure;//структура для настройки АЦП

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//максимум 24Мгц

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE ) ;//тактироваание порта А
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE ) ;//тактирование АЦП

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//определяем порты 1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//на вход без потдтяжки
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//низкая скорость
	GPIO_Init(GPIOA, &GPIO_InitStructure);//загружаем настройки

	/*ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//непрерывная работа АЦП
	//ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//выравнивание результата вправо
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;//по внешнему событию откл.
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent ;//Независимый режим работы
	//ADC_InitStructure.ADC_NbrOfChannel = 1;//число каналов, с которыми работаем 2 было
	//ADC_InitStructure.ADC_ScanConvMode = ENABLE;//сканирование каналов
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5);
	ADC_Init(ADC1, &ADC_InitStructure);//загружаем настройки*/

	NVIC_InitTypeDef NVIC_InitStructure;//Объявляем структуру для регистрации прирывания АЦП
	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;//Выбирает тип прерывания
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//Устанавливаем приоритет прерывания
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//Устанавливаем подприоритет прерывания
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//Запускаем прерывание
	NVIC_Init(&NVIC_InitStructure);//Загружаем настройки структуры

	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//непрерывная работа АЦП
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//выравнивание результата вправо
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//Работа без триггера
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent ;//Независимый режим работы между АЦП
	ADC_InitStructure.ADC_NbrOfChannel = 1;//число каналов, с которыми работаем
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;//сканирование каналов
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);//Время преобразования для 1 канала
	ADC_Init(ADC1, &ADC_InitStructure);//загружаем настройки

	ADC_AnalogWatchdogThresholdsConfig(ADC1, 1570, 1185);//Определяем границы срабатывания для АЦП1
	ADC_AnalogWatchdogSingleChannelConfig(ADC1, ADC_Channel_1);//Канал на котором работает АЦП в сторожевом режиме
	ADC_AnalogWatchdogCmd(ADC1, ADC_AnalogWatchdog_SingleRegEnable);//Включить аналоговый сторожевой таймер на одном обычном канале

	ADC_ITConfig(ADC1, ADC_IT_AWD, ENABLE);//Разрешить прерывание по сторожевому таймеру

	//ADC_ExternalTrigConvCmd( ADC1, ENABLE );//триггер выкл

	ADC_Cmd(ADC1, ENABLE);//запускаем АЦП

	ADC_ResetCalibration(ADC1);//включить регистр калибровки сброса АЦП1
	while(ADC_GetResetCalibrationStatus(ADC1));//проверка конца калибровочного регистра сброса АЦП1
	ADC_StartCalibration(ADC1);//начать калибровку АЦП1
	while(ADC_GetCalibrationStatus(ADC1));//проверка окончания калибровки АЦП1

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//Запуск преобразований в бесконечном режиме, так как настроена непрерываная работа

	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//включаем тактирование таймера 3

	//Таймер 3 частота срабатывания 5 Гц или 1.17 кГц или 2 кГц
	/*TIM3->PSC = 1000;//предделитель 1000, 5, 5
	TIM3->ARR = 4100;//период 4100, 4100, 2400
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//разрешаем запрос ДМА при обновлении счетного регистра
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);//выбирает режим вывода триггера по переполнению
	TIM_Cmd(TIM3, ENABLE);//Запускаем таймер
	NVIC_EnableIRQ(TIM3_IRQn);//разрешаем прерывание по таймеру 3*/
}

/*void TIM3_IRQHandler()
{
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);//очищаем бит переполнения таймера
    adc_value = ADC_GetConversionValue(ADC1);//преобразование АЦП
}*/

void ADC1_IRQHandler()
{
    if(ADC_GetITStatus(ADC1, ADC_IT_AWD))//Проверяем установлен ли бит прерывания по сторожевому таймеру
    {
    	if (cnts == 0) {
    		TIM_Cmd(TIM2, ENABLE);//Запускаем таймер
    		NVIC_EnableIRQ(TIM2_IRQn);//Разрешаем прерывание
    		cnts++;
    		f--;
		}

    	if (count == -1) {
			count++;
			temp=0;
			flag=1;//устанавливаем для записи старт бита в переменную temp
		}

        ADC_IT_AWD_FLAG = 1;//Устанавливаем глобальный флаг, что были приняты данные в 1
        ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);//Очищаем бит прерывания
    }
}
