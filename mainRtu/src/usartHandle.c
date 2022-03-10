/*
 * @Description :  
 * @FilePath: /app_code/mainRtu/src/usartHandle.c
 * @Author:  LR
 * @Date: 2021-09-28 10:36:30
 */
#include "baseInit.h"
#include "zh_usart.h"
#include "usartHandle.h"
#include "localServer_usart.h"

usart_info_T usart_info;
pthread_mutex_t usart_Mutex[USART_NUM] = PTHREAD_MUTEX_INITIALIZER;
/***************************************************
 * 功能：设备串口初始化 232-1;232-2;485-1;485-2;
 * 参数：void;
 * 返回：void;
 * 作者 ：LR
 * 修改日期 ：2020年08月12日
***************************************************/
void usart_init(void)
{
    memset(&usart_info, 0, sizeof(usart_info_T));
    int i = 0;
    for (i = 0; i < USART_NUM; i++)
    {
        if(i == 0) usart_info.usart_fd[i] = zh_usart_open(3);
        else if(i == 1) usart_info.usart_fd[i] = zh_usart_open(4);
        // else if(i == 2) usart_info.usart_fd[i] = zh_usart_open(2);
        // else if(i == 3) usart_info.usart_fd[i] = zh_usart_open(5);
        // else if(i == 4) usart_info.usart_fd[i] = zh_usart_open(1);
            
        if (usart_info.usart_fd[i] > 0)
        {
            zh_usart_init(usart_info.usart_fd[i], B_115200, DB_CS8, SB_1, PB_NONE);
        }
        else
        {
            log_e("[USART_%d] open error \n", i);
        }
    }
}

/***************************************************
 * 功能：串口处理函数-调试
 * 参数：void;
 * 返回：void;
 * 作者 ：LR
 * 修改日期 ：2020年08月12日
***************************************************/
void usart_listen_handle(void)
{
    int dataLen = 0, i, k;
    uint8_t dataBuff[USART_READ_MAX_BUFF];

    for (k = 0; k < USART_NUM; k++)
    {
        dataLen = zh_usart_read(usart_info.usart_fd[k], (char *)dataBuff);
        if (dataLen > 0)
        {
            log_i( "[REV<-usart]--Usart:%d--Len:%d--Data:%s\n", k, dataLen, dataBuff);
            if (dataLen > USART_READ_MAX_BUFF)
                dataLen = USART_READ_MAX_BUFF;                /* 防止访问出错 */
            for (i = 0; i < USART_LISTEN_MAX_CLIENT_NUM; i++) // 给节点上报
            {
                if (localServer_usart_info[k].socketIsLive[i] == 1)
                {
                    send_localServer_usart_data(localServer_usart_info[k].socketConGroup[i], (uint8_t *)dataBuff, dataLen);
                }
            }
        }
    }
}

/**************************************************
*功能描述 ：usart定时器  100ms
*参数 ： NULL
*返回值 ： NULL
*作者 ：LR
*修改日期 ：2019年03月29日
***************************************************/
void usart_timerHandle(void)
{
}
