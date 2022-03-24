/*
 * @Description :  
 * @FilePath: /app_code/mainRtu/include/usartHandle.h
 * @Author:  LR
 * @Date: 2021-12-24 14:59:29
 */

#ifndef _USARTHANDLE_H
#define _USARTHANDLE_H
#include "baseInit.h"
/* 串口的处理调用 */


#define USART_NUM 3

typedef struct
{
    int usart_fd[USART_NUM];
} usart_info_T;
extern usart_info_T usart_info;

extern pthread_mutex_t usart_Mutex[USART_NUM];



/***************************************************
 * 功能：设备串口初始化 232-1;232-2;485-1;485-2;
 * 参数：void;
 * 返回：void;
 * 作者 ：LR
 * 修改日期 ：2020年08月12日
***************************************************/
void usart_init(void);


/***************************************************
 * 功能：串口处理函数-调试
 * 参数：void;
 * 返回：void;
 * 作者 ：LR
 * 修改日期 ：2020年08月12日
***************************************************/
void usart_listen_handle(void);



#endif
