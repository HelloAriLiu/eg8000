/*
 * @Description :  
 * @FilePath: /app_code/Zh_Driver/src/zh_ao.c
 * @Author:  LR
 * @Date: 2021-12-24 14:59:29
 */
#include "zh_ao.h"
#include <stdio.h>
#include <wiringPi.h>
#include <pcf8591.h>
#include "zh_base.h"

int ao_init = 0;
int zh_aout_setLevel(int index, float value)
{
	//printf("aoai %f\r\n",value);
	if (ao_init == 0)
	{
		if (wiringPiSetup() == -1)
		{
			return RES_ERROR;
		}
		if (pcf8591Setup(BASE, Address) == 0)
		{
			printf("aoai ERROR");
			return RES_ERROR;
		}
		ao_init = 1;
	}

	if (index < 0 || index >= AO_NUM)
		return RES_ERROR;
	if (value < 0)
		value = 0;
	value = value * 1000;
	value = value / 20000 * 3000;
	value /= 1000;
	value = value * 256 / 3300 * 1000;
	analogWrite(BASE, value);
	return RES_OK;
}
