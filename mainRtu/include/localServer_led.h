/*
 * @Description :  
 * @FilePath: /app_code/mainRtu/include/localServer_led.h
 * @Author:  LR
 * @Date: 2021-09-29 11:34:01
 */
/*
 * @Description :  
 * @FilePath: /mainRtu/include/localServer_do.h
 * @Author:  LR
 * @Date: 2021-09-28 10:36:30
 */
#ifndef __LOCALSERVER_LED_H
#define __LOCALSERVER_LED_H
#include "baseInit.h"

#define LED_LISTEN_PORT 2022         /* LED默认监听端口 */
#define LED_LISTEN_ADDR "0.0.0.0"  /* LED 默认监听本机IP */
#define LED_LISTEN_MAX_CLIENT_NUM 2/* 最大连接数 */
#define LED_MAX_BUFF 256             /*do 通信缓存buff大小 */
typedef struct
{
    int conClientCount;                            /*  客户端总数记录*/
    int thrReceiveClientCount;                     /* 客户端线程数 */
    int socketConGroup[LED_LISTEN_MAX_CLIENT_NUM]; //已经连接的客户端 -socket号
    int socketIsLive[LED_LISTEN_MAX_CLIENT_NUM];   //标记1-3号
} localServer_led_info_T;

/***************************************************
 * 功能：初始化本DEBUG服务
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
*************************************************/
void init_localServer_led_service(void);
/***************************************************
 * 功能：创建本DEBUG服务----上位机通讯
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
*************************************************/
void create_localServer_led_service(void);

#endif
