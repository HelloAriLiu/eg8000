/*
 * @Description :  
 * @FilePath: /app_code/mainRtu/include/localServer_ao.h
 * @Author:  LR
 * @Date: 2021-10-23 14:30:32
 */
#ifndef __LOCALSERVER_AO_H
#define __LOCALSERVER_AO_H
#include "baseInit.h"


#define AO_LISTEN_PORT 2003        /* AO默认监听端口 */
#define AO_LISTEN_ADDR "0.0.0.0" /* AO 默认监听本机IP */
#define AO_LISTEN_MAX_CLIENT_NUM 2 /* 最大连接数 */
#define AO_MAX_BUFF 256   /*ao 通信缓存buff大小 */
typedef struct
{
    int conClientCount;/*  客户端总数记录*/
    int thrReceiveClientCount;/* 客户端线程数 */
    int socketConGroup[AO_LISTEN_MAX_CLIENT_NUM];   //已经连接的客户端 -socket号
    int socketIsLive[AO_LISTEN_MAX_CLIENT_NUM]; //标记1-3号
} localServer_ao_info_T;


/***************************************************
 * 功能：初始化本DEBUG服务
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
*************************************************/
void init_localServer_ao_service(void);
/***************************************************
 * 功能：创建本DEBUG服务----上位机通讯
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
*************************************************/
void create_localServer_ao_service(void);


/**
 * @brief :  
 * @param {int} client_socket
 * @param {uint8_t} *dataBuff
 * @param {int} dataLen
 * @return {*}
 * @author: LR
 * @Date: 2021-09-29 16:09:23
 */
int send_localServer_ao_data(int client_socket, uint8_t *dataBuff, int dataLen);

#endif

