/*
 * @Description :  
 * @FilePath: /app_code/mainRtu/include/localServer_ai.h
 * @Author:  LR
 * @Date: 2021-10-13 10:57:14
 */
/*
 * @Description :  
 * @FilePath: /mainRtu/include/localServer_ai.h
 * @Author:  LR
 * @Date: 2021-09-28 10:36:30
 */
#ifndef __LOCALSERVER_AI_H
#define __LOCALSERVER_AI_H
#include "baseInit.h"


#define AI_LISTEN_PORT 2002      /* AI默认监听端口 */
#define AI_LISTEN_ADDR "0.0.0.0" /* AI 默认监听本机IP */
#define AI_LISTEN_MAX_CLIENT_NUM 2 /* 最大连接数 */
#define AI_MAX_BUFF 256   /*AI通信缓存buff大小 */
typedef struct
{
    int conClientCount;/*  客户端总数记录*/
    int thrReceiveClientCount;/* 客户端线程数 */
    int socketConGroup[AI_LISTEN_MAX_CLIENT_NUM];   //已经连接的客户端 -socket号
    int socketIsLive[AI_LISTEN_MAX_CLIENT_NUM]; //标记1-3号
} localServer_ai_info_T;
extern localServer_ai_info_T localServer_ai_info;


/***************************************************
 * 功能：初始化本DEBUG服务
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
*************************************************/
void init_localServer_ai_service(void);
/***************************************************
 * 功能：创建本DEBUG服务----上位机通讯
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
*************************************************/
void create_localServer_ai_service(void);


/**
 * @brief :  
 * @param {int} client_socket
 * @param {uint8_t} *dataBuff
 * @param {int} dataLen
 * @return {*}
 * @author: LR
 * @Date: 2021-09-29 16:09:23
 */
int send_localServer_ai_data(int client_socket, uint8_t *dataBuff, int dataLen);


#endif





