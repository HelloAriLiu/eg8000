/*
 * @Description :  
 * @FilePath: /app_code/mainRtu/include/localServer_usart.h
 * @Author:  LR
 * @Date: 2021-10-09 15:21:44
 */

#ifndef __LOCALSERVER_USART_H
#define __LOCALSERVER_USART_H
#include "baseInit.h"
#include "usartHandle.h"

#define USART_LISTEN_ADDR "0.0.0.0" /* 串口默认监听本机IP */
#define USART_0_LISTEN_PORT 2010        /* 串口0默认监听端口 */
#define USART_1_LISTEN_PORT 2011        /* 串口1默认监听端口 */
#define USART_2_LISTEN_PORT 2012        /* 串口2默认监听端口 */

#define USART_PIPE_LISTEN_PORT 2019        /* 透传管道扩展串口板默认监听端口 */


#define USART_LISTEN_MAX_CLIENT_NUM 2 /* 最大连接数 */
#define USART_MAX_BUFF 1024   /*do 通信缓存buff大小 */
typedef struct
{
    int conClientCount;/*  客户端总数记录*/
    int thrReceiveClientCount;/* 客户端线程数 */
    int socketConGroup[USART_LISTEN_MAX_CLIENT_NUM];   //已经连接的客户端 -socket号
    int socketIsLive[USART_LISTEN_MAX_CLIENT_NUM]; //标记1-3号
} localServer_usart_info_T;

extern localServer_usart_info_T localServer_usart_info[USART_NUM];

extern pthread_mutex_t Usart_Server_Mutex[USART_NUM];


/***************************************************
 * 功能：初始化本DEBUG服务
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
*************************************************/
void init_localServer_usart_service(int index);

/***************************************************
 * 功能：创建本地DEBUG服务----上位机通讯
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日
*************************************************/
void create_localServer_usart_service(int index);


/**
 * @brief :  
 * @param {int} client_socket
 * @param {uint8_t} *dataBuff
 * @param {int} dataLen
 * @return {*}
 * @author: LR
 * @Date: 2021-09-29 16:09:23
 */
int send_localServer_usart_data(int client_socket, uint8_t *dataBuff, int dataLen);

#endif

