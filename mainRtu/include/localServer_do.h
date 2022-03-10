/*
 * @Description :  
 * @FilePath: /app_code/mainRtu/include/localServer_do.h
 * @Author:  LR
 * @Date: 2021-09-28 10:36:30
 */
#ifndef __LOCALSERVER_DO_H
#define __LOCALSERVER_DO_H
#include "baseInit.h"


#define DO_LISTEN_PORT 2001        /* DO默认监听端口 */
#define DO_LISTEN_ADDR "0.0.0.0" /* DO 默认监听本机IP */
#define DO_LISTEN_MAX_CLIENT_NUM 2/* 最大连接数 */
#define DO_MAX_BUFF 256   /*do 通信缓存buff大小 */
typedef struct
{
    int conClientCount;/*  客户端总数记录*/
    int thrReceiveClientCount;/* 客户端线程数 */
    int socketConGroup[DO_LISTEN_MAX_CLIENT_NUM];   //已经连接的客户端 -socket号
    int socketIsLive[DO_LISTEN_MAX_CLIENT_NUM]; //标记1-3号
} localServer_do_info_T;


/***************************************************
 * 功能：初始化本DEBUG服务
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
*************************************************/
void init_localServer_do_service(void);
/***************************************************
 * 功能：创建本DEBUG服务----上位机通讯
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
*************************************************/
void create_localServer_do_service(void);


/**
 * @brief :  
 * @param {int} client_socket
 * @param {uint8_t} *dataBuff
 * @param {int} dataLen
 * @return {*}
 * @author: LR
 * @Date: 2021-09-29 16:09:23
 */
int send_localServer_do_data(int client_socket, uint8_t *dataBuff, int dataLen);

#endif

