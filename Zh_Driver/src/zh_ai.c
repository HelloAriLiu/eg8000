/*
 * @Author: your name
 * @Date: 2021-11-26 17:54:06
 * @LastEditTime: 2022-02-08 08:34:44
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: /app_code/Zh_Driver/src/zh_ai.c
 */
#include "zh_ain.h"
#include "zh_ao.h"

#include <stdio.h>
#include <wiringPi.h>
#include <pcf8591.h>
#include "zh_base.h"

int zh_ain_getValue(Struct_Ain *ain)
{
	int value;

	if (ao_init == 0)
	{
		if (wiringPiSetup() == -1)
		{
			return RES_ERROR;
		}
		if (pcf8591Setup(BASE, Address) == 0)
		{
			//printf("aoai ERROR");
			return RES_ERROR;
		}
		ao_init = 1;
	}

	for (int i = 0; i < AI_NUM; i++)
	{
		value = analogRead(BASE + i);
		//printf("AI_GET_V:%d\r\n",value);
		float adcvalue = 0;

		adcvalue = value * 3300.0 / 255.0 / 300.0 * 480.0;
		//printf("AI_GET_V1:%0.3f\r\n",adcvalue);
		adcvalue = adcvalue / 480.0 + adcvalue / 510.0;
		//printf("AI_GET_V2:%0.3f\r\n",adcvalue);
		ain->ai_value[i] = adcvalue;

	}
	return RES_OK;
}
