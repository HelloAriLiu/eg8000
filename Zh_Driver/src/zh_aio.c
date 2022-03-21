/*
 * @Author: your name
 * @Date: 2021-11-26 17:54:06
 * @LastEditTime: 2022-03-17 05:26:29
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: /app_code/Zh_Driver/src/zh_aio.c
 */
#include "zh_aio.h"
#include <stdio.h>
#include <wiringPi.h>
#include "zh_base.h"
#include "zh_usart.h"

int aio_fd;
/**
 * @brief :  aio的初始化
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2022-03-17 04:47:58
 */
int zh_aio_init(void)
{
	if((aio_fd=zh_usart_open(USART_4))==RES_ERROR)
	return RES_ERROR;

	if(zh_usart_init(aio_fd,B_115200,DB_CS8,SB_1,PB_NONE)==RES_ERROR)
	return RES_ERROR;


	return RES_OK;
}


/**
 * @brief :  
 * @param {Struct_Ain} *ain
 * @return {*}
 * @author: LR
 * @Date: 2022-03-17 04:47:52
 */
int zh_ain_getValue(Struct_Ain *ain)
{
	

	
	return RES_OK;
}


/**
 * @brief :  
 * @param {int} index
 * @param {float} value
 * @return {*}
 * @author: LR
 * @Date: 2022-03-17 04:47:55
 */
int zh_aout_setLevel(int index, float value)
{

	return RES_OK;
}
