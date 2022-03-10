/*
 * @Description :  
 * @FilePaups: /mainRtu/include/localServer_ups.h
 * @Auupsor:  LR
 * @Date: 2021-10-23 14:35:38
 */


#ifndef __LOCALSERVER_UPS_H
#define __LOCALSERVER_UPS_H
#include "baseInit.h"


#define UPS_LISTEN_PORT 2020      /* UPS默认监听端口 */
#define UPS_LISTEN_ADDR "0.0.0.0" /*  默认监听本机IP */
#define UPS_LISTEN_MAX_CLIENT_NUM 2 /* 最大连接数 */
#define UPS_MAX_BUFF 256   /*通信缓存buff大小 */
typedef struct
{
    int conClientCount;/*  客户端总数记录*/
    int upsrReceiveClientCount;/* 客户端线程数 */
    int socketConGroup[UPS_LISTEN_MAX_CLIENT_NUM];   //已经连接的客户端 -socket号
    int socketIsLive[UPS_LISTEN_MAX_CLIENT_NUM]; //标记1-3号
} localServer_ups_info_T;
extern localServer_ups_info_T localServer_ups_info;


/***************************************************
 * 功能：初始化本DEBUG服务
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
*************************************************/
void init_localServer_ups_service(void);
/***************************************************
 * 功能：创建本DEBUG服务----上位机通讯
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
*************************************************/
void create_localServer_ups_service(void);


/**
 * @brief :  
 * @param {int} client_socket
 * @param {uint8_t} *dataBuff
 * @param {int} dataLen
 * @return {*}
 * @Date: 2021-09-29 16:09:23
 */
int send_localServer_ups_data(int client_socket, uint8_t *dataBuff, int dataLen);


#endif
