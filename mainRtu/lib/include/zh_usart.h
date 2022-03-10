/*
 * @Description :  
 * @FilePath: /sdk_project/include/zh_usart.h
 * @Author:  LR
 * @Date: 2021-09-10 16:36:12
 */
#ifndef __ZH_USART_H__
#define __ZH_USART_H__

#include "zh_base.h"
#define USART_READ_MAX_BUFF 1024

typedef enum
{
	PB_NONE = 0,  //none parity check
	PB_ODD,		  //odd parity check
	PB_EVEN		  //even parity check
} Enum_ParityBit; //校验位

typedef enum
{
	DB_CS7 = 7,
	DB_CS8 = 8
} Enum_DataBit; //数据位

typedef enum
{
	SB_1 = 1,
	SB_2 = 2
} Enum_StopBit; //停止位

typedef enum
{
	B_1200 = 1200,
	B_2400 = 2400,
	B_4800 = 4800,
	B_9600 = 9600,
	B_19200 = 19200,
	B_38400 = 38400,
	B_57600 = 57600,
	B_115200 = 115200,
	B_230400 = 230400,
	B_460800 = 460800,
	B_921600 = 921600
} Enum_BaudRate; //波特率

typedef enum
{
	USART_1 = 3,
	USART_2 = 4,
	USART_AT = 100
} Enum_UsartName;

/******************************************************************************************
 * 功能：打开串口
 * 参数：usartName ,串口名称 one value of Enum_UsartName.
 * 返回：OK 文件描述符 >0; RES_ERROR -1;
 * 作者 ：LR
 * 修改日期 ：2020年03月27日 
*******************************************************************************************/
int zh_usart_open(Enum_UsartName usartName);

/******************************************************************************************
 * 功能：初始化串口
 * 参数：usartFd ,串口文件描述符
                baudRate，波特率 one value of Enum_BaudRate.
				dataBit，数据位one value of Enum_DataBit.
				stopBit，停止位 one value of Enum_StopBit.
				parityBit，校验位 one value of Enum_ParityBit.
 * 返回：RES_OK 0; RES_ERROR -1;
 * 作者 ：LR
 * 修改日期 ：2020年03月27日 
*******************************************************************************************/
int zh_usart_init(int usartFd, Enum_BaudRate baudRate, Enum_DataBit dataBit, Enum_StopBit stopBit, Enum_ParityBit parityBit);

/******************************************************************************************
 * 功能：串口数据发送
 * 参数：usartFd ,串口文件描述符
                *sendBuff，发送数据内容.
				dataLen，发送数据长度.
 * 返回：send_len ; RES_ERROR -1;
 * 作者 ：LR
 * 修改日期 ：2020年03月27日 
******************************************************************************************/
int zh_usart_send(int usartFd, unsigned char *sendBuff, int dataLen);

/******************************************************************************************
 * 功能：串口数据接收
 * 参数：usartFd ,串口文件描述符
                *readBuff，接受数据地址.
 * 返回：成功返回接收长度 ; RES_ERROR -1;
 * 作者 ：LR
 * 修改日期 ：2020年03月27日 
******************************************************************************************/
int zh_usart_read(int usartFd, unsigned char *readBuff);

/******************************************************************************************
 * 功能：关闭串口文件
 * 参数：usartFd ,串口文件描述符
 * 返回：RES_OK  0; RES_ERROR -1;
 * 作者 ：LR
 * 修改日期 ：2020年03月27日 
*******************************************************************************************/
int zh_usart_close(int usartFd);

#endif
