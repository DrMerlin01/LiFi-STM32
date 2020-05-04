#include "adc_mobile.h"
///////////////////////////////////////////////////////////////////////////////////////////////
void ADCMobileInit()
{
	GPIO_InitTypeDef GPIO_InitStructure;//структура для настройки порта А
	ADC_InitTypeDef ADC_InitStructureMobile;//структура для настройки АЦП

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//максимум 24Мгц

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE ) ;//тактироваание порта А
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE ) ;//тактирование АЦП
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);//включаем тактирование таймера 4

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//определяем порты 2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//на вход без подтяжки
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//низкая скорость
	GPIO_Init(GPIOA, &GPIO_InitStructure);//загружаем настройки

	ADC_InitStructureMobile.ADC_ContinuousConvMode = ENABLE;//непрерывная работа АЦП
	ADC_InitStructureMobile.ADC_DataAlign = ADC_DataAlign_Right;//выравнивание результата вправо
	ADC_InitStructureMobile.ADC_Mode = ADC_Mode_Independent ;//Независимый режим работы
	ADC_InitStructureMobile.ADC_NbrOfChannel = 1;//число каналов, с которыми работаем
	ADC_InitStructureMobile.ADC_ScanConvMode = ENABLE;//сканирование каналов
	ADC_InitStructureMobile.ADC_ExternalTrigConv = ADC_ExternalTrigInjecConv_T4_TRGO; //для инжект
	ADC_Init(ADC1, &ADC_InitStructureMobile);//загружаем настройки
	ADC_InjectedSequencerLengthConfig(ADC1, 1);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_55Cycles5);

	NVIC_InitTypeDef NVIC_InitStructureADC;//Объявляем структуру для регистрации прирывания АЦП
	NVIC_InitStructureADC.NVIC_IRQChannel = ADC1_IRQn;//Выбирает тип прерывания
	NVIC_InitStructureADC.NVIC_IRQChannelPreemptionPriority = 0;//Устанавливаем приоритет прерывания
	NVIC_InitStructureADC.NVIC_IRQChannelSubPriority = 0;//Устанавливаем подприоритет прерывания
	NVIC_InitStructureADC.NVIC_IRQChannelCmd = ENABLE;//Запускаем прерывание
	NVIC_Init(&NVIC_InitStructureADC);//Загружаем настройки структуры

	ADC_Cmd(ADC1, ENABLE);//запускаем АЦП
	ADC_ResetCalibration(ADC1);//включить регистр калибровки сброса АЦП1
	while(ADC_GetResetCalibrationStatus(ADC1));//проверка конца калибровочного регистра сброса АЦП1
	ADC_StartCalibration(ADC1);//начать калибровку АЦП1
	while(ADC_GetCalibrationStatus(ADC1));//проверка окончания калибровки АЦП1
	ADC_AutoInjectedConvCmd(ADC1, ENABLE ); //Запуск преобразований в бесконечном режиме, так как настроена непрерываная работа ИНЖЕК КАНАЛЫ
	ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE);

	TIM4->PSC = 5;//предделитель
	TIM4->ARR = 600;//период для 8 Гц
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);//разрешаем запрос ДМА при обновлении счетного регистра
	TIM_SelectOutputTrigger(TIM4, TIM_TRGOSource_Update);//выбирает режим вывода триггера по переполнению
	TIM_Cmd(TIM4, ENABLE);//Запускаем таймер
	NVIC_EnableIRQ(TIM4_IRQn); //по таймеру 4
}

void TIM4_IRQHandler()
{
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);//очищаем бит переполнения таймера
    int adc_value=ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_1);
    if (adc_value == 1) dataForSend[countBit] = 4000;
    else dataForSend[countBit] = 2000;
}
