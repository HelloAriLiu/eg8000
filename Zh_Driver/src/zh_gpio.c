/*
 * @Description :  
 * @FilePath: /app_code/Zh_Driver/src/zh_gpio.c
 * @Author:  LR
 * @Date: 2016-02-11 16:34:04
 */
#include <wiringPi.h>
#include <stdio.h>
#include "zh_gpio.h"
#include "zh_base.h"



#define DO_PI_1  1//GPIO18
#define DO_PI_2  4 //GPIO23
#define DO_PI_3  5  //GPIO24
#define DO_PI_4  6  //GPIO25

#define DI_PI_1  0    //GPIO17
#define DI_PI_2  2   //GPIO27
#define DI_PI_3  3  //GPIO22
#define DI_PI_4  12  //GPIO10

unsigned char din_gpio[4] = {DI_PI_1, DI_PI_2, DI_PI_3, DI_PI_4};
unsigned char dout_gpio[4] = {DO_PI_1, DO_PI_2, DO_PI_3, DO_PI_4};


/**
 * @brief :  io部分的初始化
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2022-03-15 06:33:33
 */
void zh_io_init(void)
{
	int i = 0;
	for (i = 0; i < DIN_NUM; i++)
	{
		pinMode(din_gpio[i], INPUT);
	}

	for (i = 0; i < DOUT_NUM; i++)
	{
		pinMode(dout_gpio[i], OUTPUT);
	}
}



/**
 * @brief :  
 * @param {Struct_DinLevel} *dinLevel
 * @return {*}
 * @author: LR
 * @Date: 2022-03-15 06:35:45
 */
int zh_din_get(Struct_DinLevel *dinLevel)
{

	for (int i = 0; i < DIN_NUM; i++)
	{
		dinLevel->DIN_Level[i] = digitalRead(din_gpio[i]);
	}

	dinLevel->error_code = 0;
	return RES_OK;
}

/**
 * @brief :  
 * @param {int} din_num
 * @return {*}
 * @author: LR
 * @Date: 2022-03-15 06:35:49
 * 
 */
int zh_din_getLevel(int din_num)
{
	if (din_num >= 0 && din_num <= 3)
	{
		return digitalRead(din_gpio[din_num]);
	}
	else
	{
		return RES_ERROR;
	}
}


/**
 * @brief :  
 * @param {int} dout_num
 * @return {*}
 * @author: LR
 * @Date: 2022-03-15 06:35:52
 */
int zh_dout_getLevel(int dout_num)
{
	if (dout_num >= 0 && dout_num <= 3)
	{
		int status = 0;
		pinMode(dout_gpio[dout_num], INPUT);
		status = digitalRead(dout_gpio[dout_num]);
		pinMode(dout_gpio[dout_num], OUTPUT);
		return status;
	}
	else
	{
		return RES_ERROR;
	}
}


/**
 * @brief :  
 * @param {Struct_DoutLevel} *doutLevel
 * @return {*}
 * @author: LR
 * @Date: 2022-03-15 06:35:56
 */
int zh_dout_get(Struct_DoutLevel *doutLevel)
{
	for (int i = 0; i < DOUT_NUM; i++)
	{
		//pinMode(dout_gpio[i], INPUT);
		doutLevel->DOUT_Level[i] = digitalRead(dout_gpio[i]);
		//pinMode(dout_gpio[i], OUTPUT);
	}
	return RES_OK;
}

/**
 * @brief :  
 * @param {int} dout_num
 * @param {int} dout_level
 * @return {*}
 * @author: LR
 * @Date: 2022-03-15 06:36:57
 */
int zh_dout_setLevel(int dout_num, int dout_level)
{
	//数据校验
	if (dout_num < 0 || dout_num > 3 || (dout_level != PINLEVEL_LOW && dout_level != PINLEVEL_HIGH))
	{
		return RES_ERROR;
	}

	dout_level = !dout_level;
	digitalWrite(dout_gpio[dout_num], dout_level);
	return RES_OK;
}
