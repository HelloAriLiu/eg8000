#include <wiringPi.h>
#include <stdio.h>
#include "zh_gpio.h"
#include "zh_base.h"
unsigned char din_gpio[4] = {DI1,DI2,DI3,DI4};
unsigned char dout_gpio[4] = {DO1,DO2,DO3,DO4};



// void zh_gpio_init(void)
// {
// 	if(wiringPiSetup() == -1)
// 	{
// 		return ;
// 	}
// 	//ai ao 处理芯片初始化 
// 	if(pcf8591Setup(BASE,Address) == 0)
// 	{
// 		return;
// 	}
// 	//di初始化
// 	for(int i = 0;i < DIN_NUM; i++)
// 	{
// 		pinMode(din_gpio[i],INPUT);
// 	}
// 	//do初始化
// 	for(int i = 0;i < DOUT_NUM; i++)
// 	{
// 		pinMode(dout_gpio[i],OUTPUT);
// 	}
// 	//ups初始化
// 	pinMode(DI_UPS,INPUT);

// }



int di_init = 0;
int zh_din_get(Struct_DinLevel *dinLevel)
{
	int din_level;
	if(di_init == 0)
	{
		if(wiringPiSetup() == -1)
		{
			return RES_ERROR;
		}
		for(int i = 0;i < DIN_NUM; i++)
		{
			pinMode(din_gpio[i],INPUT);
		}
		di_init =1;
	}
	//init GPIO LIB
	if(wiringPiSetup() == -1)
	{
		return RES_ERROR;
	}
	for(int i = 0;i < DIN_NUM; i++)
	{
		dinLevel->DIN_Level[i] = digitalRead(din_gpio[i]);
	}
	dinLevel->error_code = 0;
	return RES_OK;
}
int zh_din_getLevel(int din_num)
{
	if(din_num >=0 && din_num <=3)
	{
		return digitalRead(din_gpio[din_num]);
	}
	else
	{
	return RES_ERROR;
	}
}
int zh_dout_getLevel(int dout_num)
{
	if(dout_num >=0 && dout_num <=3)
	{
		int status = 0;
		pinMode(dout_gpio[dout_num],INPUT);
		status = digitalRead(dout_gpio[dout_num]);
		pinMode(dout_gpio[dout_num],OUTPUT);
		return status;
	}
	else
	{
	return RES_ERROR;
	}
}


int zh_dout_get(Struct_DoutLevel *doutLevel)
{

	if(wiringPiSetup() == -1)
	{
		return RES_ERROR;
	}
	for(int i = 0;i < DOUT_NUM; i++)
	{
		pinMode(dout_gpio[i],INPUT);
		doutLevel->DOUT_Level[i] = digitalRead(dout_gpio[i]);
		pinMode(dout_gpio[i],OUTPUT);
	}
	doutLevel->error_code = 0;
	return RES_OK;
}

//上层驱动不会初始化io
int do_init = 0;

int zh_dout_setLevel(int dout_num, int dout_level)
{
	int res = 0;
	int ch,dout;//
	if(do_init == 0)
	{
		if(wiringPiSetup() == -1)
		{
			return RES_ERROR;
		}
		for(int i = 0;i < DOUT_NUM; i++)
		{
			pinMode(dout_gpio[i],OUTPUT);
		}
		do_init = 1;
	}
	
	//数据校验
	if( dout_num < 0 || dout_num>3 || (dout_level != PINLEVEL_LOW && dout_level != PINLEVEL_HIGH))
	{
		return RES_ERROR;
		
	}
	dout_level = !dout_level;
	digitalWrite(dout_gpio[dout_num],dout_level);
	//printf("DO Set Value:%d\r\n",dout_level);
	return RES_OK;
}




