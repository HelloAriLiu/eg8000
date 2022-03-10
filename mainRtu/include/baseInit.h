/*
 * @Description :  
 * @FilePath: /app_code/mainRtu/include/baseInit.h
 * @Author:  LR
 * @Date: 2021-09-28 10:36:30
 */
#ifndef __BASEINIT_H
#define __BASEINIT_H

#include <sys/time.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include "../lib/include/zh_base.h"
#include "alog_printf.h"
#include "exception.h"

#define LOG_TAG "EG8000"
#include <elog.h>

#define BUFF_LOCK 1
#define BUFF_UNLOCK 0

#define DEV_CFG_PATH "./EG8000.cfg"
#define DEV_VERSION "1.0.2"

/* 系统命令 */
typedef enum
{
    NO_CMD, /* 无 */
    REBOOT, /* 重启 */
    RESET,  /* 复位 */
    UPGRADE /* 升级*/
} sys_cmd;

/* 联网方式 */
typedef enum
{
    NET_4G, /* 4G */
    NET_WAN /* 有线 WAN口*/
} net_type;

#define SYS_MODEL_LEN 10
#define SYS_SN_LEN 16
typedef struct
{
    char userPassword[128];
    char model[20]; /*型号*/
    char sn[17];    /*编号*/
    char version[20];
    char nodeVersion[20];
    int cpuUsage;
    int memUsage;
    int cmd;
    int defaultNet;
} sysBasic_T;
extern sysBasic_T sysBasic;

#define LTE_APN_LEN 100
typedef struct
{
    char addr[LTE_APN_LEN + 1];
    char username[LTE_APN_LEN + 1];
    char password[LTE_APN_LEN + 1];
} lteAPN_T;

#define IP_LEN 16
#define LTE_CCID_LEN 20
typedef struct
{
    char ip[IP_LEN];
    char mask[IP_LEN];
    char gateway[IP_LEN];
    char ccid[LTE_CCID_LEN + 1];
    int signal;
    int act; //重新拨号
    lteAPN_T lteAPN;
} lteBasic_T;
extern lteBasic_T lteBasic;

#define WIFI_NAME_LEN 128
#define WIFI_PASS_LEN 128
typedef struct
{
    int enable;
    char ssid[WIFI_NAME_LEN];
    char password[WIFI_PASS_LEN];
} wifiBasic_T;
extern wifiBasic_T wifiBasic;

#define IP_MODE_DHCPC 0
#define IP_MODE_STATIC 1
typedef struct
{
    int enable;
    char startIp[IP_LEN];
    char endIp[IP_LEN];
} dhcpsBasic_T;


#define DEF_LAN_IP "192.168.88.1"
#define DEF_LAN_MASK "255.255.255.0"

typedef struct
{
    char ip[IP_LEN];
    char mask[IP_LEN];
    char gateway[IP_LEN];
    dhcpsBasic_T dhcpsBasic;
} lanBasic_T;
extern lanBasic_T lanBasic;

typedef struct
{
    int ipMode;
    char ip[IP_LEN];
    char mask[IP_LEN];
    char gateway[IP_LEN];
} wanBasic_T;
extern wanBasic_T wanBasic;

typedef struct
{
    int socketCon; /* 客户端文件描述符 */
    char *ipaddr;  /* 客户端IP */
    int port;      /* 客户端端口 */
    int registerCheckFlg;
    int index;
} clientSocketInfo;



/***************************************************
 * 功能：初始化设备配置
 * 参数：void
 * 返回：RES_OK  0,  RES_ERROR -1;
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
*************************************************/
void init_dev(void);

/**
 * @brief :  复位配置
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-06-05 14:41:04
 */
int reset_dev_cfg(void);

/**
 * @brief :  设置设备配置
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-06-09 11:19:47
 */
int set_dev_cfg(void);

int zh_set_ethCfg(void);
/* ***************************************************
 * 功能：数据打印-16进制
 * 参数：void
 * 返回：当前时间毫秒值
 * 作者 ：LR
 * 修改日期 ：2020年08月12日  
***************************************************/
void printf_hexData(char *dataBuff, unsigned int dataLen);
/***************************************************
 * 功能：判断线程十分存在
 * 参数：*sendBuff   发送buff ;   dataLen  发送长度；
 * 返回：存在  0,  不存在 1;
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
*************************************************/
int checkThrIsKill(pthread_t thr);

#endif
