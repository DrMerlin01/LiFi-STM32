#include "dac_mobile.h"
///////////////////////////////////////////////////////////////////////////////////////////////
void DACMobileInit()
{
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//тактирование порта А
	GPIO_InitTypeDef PORTA;//создание структр для порта А

	GPIO_StructInit (&PORTA);//инициализация структуры
	PORTA.GPIO_Mode = GPIO_Mode_AIN;//аналоговый вход режим работы
	PORTA.GPIO_Pin = GPIO_Pin_5;// порт А5
	PORTA.GPIO_Speed = GPIO_Speed_2MHz;//скорость низкая
	GPIO_Init (GPIOA, &PORTA);//настраиваем согласно заполненной структуре

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);//включаем тактирование таймера 7

	//Таймер 7 частота срабатывания 1.17кГц или  5 Гц, 2 кГц (предделитель 5, а период 2400)
	//(24 000 000 / (4100 * 1000)) - 5 Гц
	TIM7->PSC = 5;//предделитель
	TIM7->ARR = 600;// для 8 кГц
	TIM_ITConfig(TIM7, TIM_DIER_UIE , ENABLE);//разрешаем запрос ДМА при обновлении счетного регистра
	TIM_SelectOutputTrigger(TIM7, TIM_TRGOSource_Update);//выбирает режим вывода триггера по переполнению
	TIM_Cmd(TIM7, ENABLE);//Запускаем таймер

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);//тактирование DAC
	DAC_InitTypeDef DAC_InitStructure;//структура для ЦАП

	DAC_StructInit(&DAC_InitStructure); //инициализация структуры
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T7_TRGO;//выбираем триггер для ЦАП 7
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;//не генерируем волны
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;//включаем выходной буфер
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);//настраиваем согласно заданным параметрам
	DAC_Cmd(DAC_Channel_2, ENABLE);//запускаем ЦАП
}
