/*
 * @Author: your name
 * @Date: 2021-11-26 17:54:06
 * @LastEditTime: 2022-03-23 14:59:06
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
	if ((aio_fd = zh_usart_open(USART_4)) == RES_ERROR)
		return RES_ERROR;

	if (zh_usart_init(aio_fd, B_9600, DB_CS8, SB_1, PB_NONE) == RES_ERROR)
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
	uint8_t sendBuff[8] = {0x01, 0x04, 0x00, 0x00, 0x00, 0x04, 0xF1, 0xC9};
	uint8_t readBuff[USART_READ_MAX_BUFF];
	uint8_t i = 0;

	if (zh_usart_send(aio_fd, sendBuff, 8) == RES_ERROR)
		return RES_ERROR;
	usleep(200000);
	bzero(&readBuff, sizeof(readBuff));
	int readBuffLen = zh_usart_read(aio_fd, readBuff);
	if (readBuffLen == 13)
	{
		for (i = 0; i < AI_NUM; i++)
		{
			ain->ai_value[i] = (readBuff[3 + i * 2] << 8 | readBuff[4 + i * 2]) / 1000.0;
		}
		return RES_OK;
	}

	return RES_ERROR;
}

/**
 * @brief :  AO设置值
 * @param {int} index
 * @param {float} value
 * @return {*}
 * @author: LR
 * @Date: 2022-03-17 04:47:55
 */
int zh_aout_setValue(int index, float value)
{
	if(index <0 || index>=AO_NUM  ||  value<0 || value >25.0)
	{
		return RES_ERROR;
	}

	uint8_t  sendBuff[11] = {0x01, 0x10, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0xAA, 0xBB};
	
	uint8_t readBuff[USART_READ_MAX_BUFF];
	uint8_t i = 0;

	uint16_t ao_value = value*1000;
	sendBuff[3] = index;
	sendBuff[7] = ao_value>>8;
	sendBuff[8] = ao_value;
	set_crc16(sendBuff,9);

	if (zh_usart_send(aio_fd, sendBuff, 11) == RES_ERROR)
		return RES_ERROR;
	usleep(100000);
	bzero(&readBuff, sizeof(readBuff));
	int readBuffLen = zh_usart_read(aio_fd, readBuff);
	if (readBuffLen == 8)
	{
		return RES_OK;
	}

	return RES_ERROR;

}
