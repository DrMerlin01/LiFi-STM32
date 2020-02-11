#include "lcd.h"

void Lcd_write_str(uc8 *STRING)//запись строки
{
	char c; //символ из строки
	while (c=*STRING++){//проходимся по всей строке
	if(c>=192) Lcd_write_data(russian[c-192]);//если у нас русские символы
	else Lcd_write_data(c);//иначе прочие
	}
}

void Lcd_goto(uc8 x,uc8 y)//позиция курсора
{
	switch(x)
		{
			case 0://определяем строку первая
				Lcd_write_cmd(y|0x80);//позиция 00
				break;
			case 1://вторая
				Lcd_write_cmd(0x40+y|0x80);//позиция по счету 40
				break;
			case 2://третья
				Lcd_write_cmd(0x10+y|0x80);//позиция по счету согласно даташиту 14
				break;
			case 3://ну и четвертая
				Lcd_write_cmd(0x50+y|0x80);//позиция по счету 54
				break;
		}
}

void Init_pin_out()
{
	RCC_APB2PeriphClockCmd(init_port, ENABLE);//тактирование порта В
	GPIO_InitTypeDef init_pin;//создание структуры
	init_pin.GPIO_Pin  = pin_e | pin_rs | pin_rw | pin_d7 | pin_d6 | pin_d5 | pin_d4;//определение портов
	init_pin.GPIO_Mode = GPIO_Mode_Out_PP;//на вывод
	init_pin.GPIO_Speed = GPIO_Speed_50MHz;//высокая скорость
	GPIO_Init (port, &init_pin);//загружаем настройки в структуру
}

void Init_pin_in()
{
	RCC_APB2PeriphClockCmd(init_port, ENABLE);//тактирование порта В
	GPIO_InitTypeDef init_pin;//создание структуры
	init_pin.GPIO_Pin  =  pin_d7 | pin_d6 | pin_d5 | pin_d4 ;//определение портов
	init_pin.GPIO_Mode = GPIO_Mode_IPD;//на вход
	init_pin.GPIO_Speed = GPIO_Speed_50MHz;//высокая скорость
	GPIO_Init (port, &init_pin);//загружаем настройки в структуру
}

void Lcd_write_cmd(uc8 cmd )
{
	Init_pin_out();//настройка портов на вывод
	del=24000; while (del--){}//задержка
	rs_0;//показываем что будут передаваться команды
	GPIO_Write(port,((cmd>>4)<<lcd_shift));//передаем старшую часть
	e_1;//начинаем передачу
	del=10; while (del--){}//задержка
	e_0;//заканчиваем передачу
	GPIO_Write(port,(0x00)<<lcd_shift);//
	GPIO_Write(port,((cmd&0x0F)<<lcd_shift));//передаем младшую часть
	del=10; while (del--){}//задержка
	e_1;//начинаем передачу
	del=10; while (del--){}//задержка
	e_0;rs_0;rw_0;//заканчиваем передачу
}

void Lcd_write_data(uint8_t data)
{
	Init_pin_out();//настрайка портов на вывод
	GPIO_Write(port,((data>>4)<<lcd_shift));//передаем старшую часть
	e_1;rs_1;//начинаем передачу данных
	del=10; while (del--){}//задержка
	e_0;//заканчиваем передачу
	GPIO_Write(port,(0x00)<<lcd_shift);
	GPIO_Write(port,((data&0x0F)<<lcd_shift));//передаем младшую часть
	del=10; while (del--){}//задержка
	e_1;rs_1;//начинаем передачу
	del=10; while (del--){}//задержка
	e_0;rs_0;rw_0;//заканчиваем
	GPIO_Write(port,(0x00)<<lcd_shift);
}

void Init_lcd()//инициализация дисплея
{
	Init_pin_out();//настраиваем порты на вывод
	del=72000; while (del--){}//задержка
	Lcd_write_cmd(Function_set);//включить 4 битный режим
	del=72000; while (del--){}//задержка
	Lcd_write_cmd(Display_on_off_control);//включить дисплей, выключить курсор и мигание
	del=72000; while (del--){}//задержка
	Lcd_write_cmd(Display_clear);//очистка экрана
	del=72000; while (del--){}//задержка
    Lcd_write_cmd(Entry_mode_set);//выбрать режим установки
}

void Lcd_clear()
{
	Lcd_write_cmd(Display_clear);//передаем команду очистки дисплея
}

void Return_home()
{
	Lcd_write_cmd(0b0000001);//команда вернуться в начало дисплея
}

void Lcd_definechar(const uint8_t *pc,uint8_t char_code)//создание собственного символа
{
	uint8_t a;//храним здесь код символа
	uint16_t i;//проходимся битам
	a=(char_code<<3)|0x40;//записываем код символа
	for (i=0; i<8; i++)
	{
		Lcd_write_cmd(a++);//передаем код в память
		Lcd_write_data(pc[i]);//запсиываем данные
	}
}

void Lcd_write_adc_number(float number)//доработать!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
{
	int integer_part, fractional_part, int_from_frac, frac_from_frac;
	float edit_adc_value;

	edit_adc_value = (3.3/4096.0)*number*1000;//преобразуем значение в Вольты

	integer_part = edit_adc_value/1000; //получение целой части напряжения
	number_output(integer_part);//вывод целой части
	Lcd_write_str(".");

	if (integer_part!=0)//проверка на отличие целой части от 0
	{
		fractional_part = (int)edit_adc_value%1000;//получение дробной части
	}

	int_from_frac = fractional_part/100;//получение целой сотой части от дробной
	number_output(int_from_frac);//вывод

	frac_from_frac = fractional_part%100;//получение остатка от деления дробной чатси на 100
	number_output(frac_from_frac/10);//вывод
	number_output(frac_from_frac%10);//вывод
	Lcd_write_str(" ");
}

void number_output(int number)
{
	switch(number)
	{
		case 0:
		{
			Lcd_write_str("0");
			break;
		}
		case 1:
		{
			Lcd_write_str("1");
			break;
		}
		case 2:
		{
			Lcd_write_str("2");
			break;
		}
		case 3:
		{
			Lcd_write_str("3");
			break;
		}
		case 4:
		{
			Lcd_write_str("4");
			break;
		}
		case 5:
		{
			Lcd_write_str("5");
			break;
		}
		case 6:
		{
			Lcd_write_str("6");
			break;
		}
		case 7:
		{
			Lcd_write_str("7");
			break;
		}
		case 8:
		{
			Lcd_write_str("8");
			break;
		}
		case 9:
		{
			Lcd_write_str("9");
			break;
		}
		default:
		{
			Lcd_write_str("error");
			break;
		}
	}
}
