#include "dac_dma.h"

void DAC_DMAInit(void)
{
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//тактирование порта А
	GPIO_InitTypeDef PORTA;//создание структр для порта А

	GPIO_StructInit (&PORTA);//инициализация структуры
	PORTA.GPIO_Mode = GPIO_Mode_AIN;//аналоговый вход режим работы
	PORTA.GPIO_Pin = GPIO_Pin_4;// порт А4
	PORTA.GPIO_Speed = GPIO_Speed_2MHz;//скорость низкая
	GPIO_Init (GPIOA, &PORTA);//настраиваем согласно заполненной структуре

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);//включаем тактирование таймера 6

	//Таймер 6 частота срабатывания 1.17кГц или  5 Гц, 2 кГц (предделитель 5, а период 2400)
	//(24 000 000 / (4100 * 1000)) - 5 Гц
	TIM6->PSC = 5;//предделитель
	TIM6->ARR = 600;// для 8 Гц, был период 2100
	TIM_ITConfig(TIM6, TIM_DIER_UIE , ENABLE);//разрешаем запрос ДМА при обновлении счетного регистра
	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);//выбирает режим вывода триггера по переполнению
	TIM_Cmd(TIM6, ENABLE);//Запускаем таймер
	//NVIC_EnableIRQ(TIM6_DAC_IRQn);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);//тактирование DAC
	DAC_InitTypeDef DAC_InitStructure;//структура для ЦАП

	//DAC_DeInit();//отключаем
	DAC_StructInit(&DAC_InitStructure); //инициализация структуры
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;//выбираем триггер для ЦАП 6
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;//не генерируем волны
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;//включаем выходной буфер
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);//настраиваем согласно заданным параметрам

	DAC_Cmd(DAC_Channel_1, ENABLE);//запускаем ЦАП

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//тактирование DMA
	DMA_InitTypeDef DMA_InitStructure;//структура для ДМА

	DMA_DeInit(DMA1_Channel3);//настройки по умолчанию
	DMA_StructInit(&DMA_InitStructure);//инициализация структуры
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(DAC->DHR12R1);//адрес памяти, куда передаем данные, в данном случае ЦАП
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&dataForSend;//ссылка на адрес памяти, с которого будет передача данных
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;//направление передачи память периферия
	DMA_InitStructure.DMA_BufferSize = 1;//размер буфера, который передаем
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//отключаем увеличение адреса в периферии при работе
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//включаем увеличение адреса данных в памяти
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//размер посылки в периферии половина слова
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//размер посылки в памяти половина слова
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//Циклический режим работы
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;//Высоки приоритет текущего канала
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//Отключаем передачу из памяти в память
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);//настраиваем согласно заданным параметрам

	DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);
	NVIC_EnableIRQ(DMA1_Channel3_IRQn);//прерывание по ДМА

	DMA_Cmd(DMA1_Channel3, ENABLE);//Запускаем ДМА
	DAC_DMACmd(DAC_Channel_1, ENABLE);//Запускаем работу ДМА с ЦАП
}

int c = 0; //флаг для включения таймера 6
int flagSend=0;
void DMA1_Channel3_IRQHandler(void)
{
	if (firstWorkTimer==1)//синхронизация ацп и цап
	{
		TIM3->ARR = 1100; // 2300
	}
	DMA1->IFCR|=DMA_ISR_TCIF3; //сбрасываем флаг прерывания
	firstWorkTimer++;
}
