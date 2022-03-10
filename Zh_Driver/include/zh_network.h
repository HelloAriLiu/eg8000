/*
 * @Description :  
 * @FilePath: /sdk_project/include/zh_network.h
 * @Author:  LR
 * @Date: 2021-09-10 16:36:12
 */
#ifndef __ZH_NETWORK_H__
#define __ZH_NETWORK_H__
#include "zh_base.h"

typedef enum
{
    ETH0 = 0,     //网口0
    ETH1 = 1,     //网口1
    ETH2 = 2,     //4G
    WLAN0 = 3,    //wifi
    BR_LAN = 4,  //网桥
    PPP0 = 5,
    UNKNOWN = 255 //未知
} Enum_netName;

typedef struct
{
    char inet_addr[16]; //ip
    char mask[16];      //子网掩码
    int error_code;     //错误码
} Struct_netCfg;



/*******************************************************************************************
 * wifi
*******************************************************************************************/
typedef enum
{
    WIFI_AP = 0,
    WIFI_STA
} Enum_wifiMode;

typedef struct
{
    char ssid[32];     // 用户名   [字母，数字 ，'_'，'-'，'@'，'#' ]不支持中文
    char password[32]; // 密码     [字母，数字 ，'_'，'-'，'@'，'#' ]不支持中文
    int error_code;
} Struct_wifiCfg;

/********************************************************************************************
 * DHCP
********************************************************************************************/
typedef enum
{
    DHCP_S = 0,
    DHCP_C
} Enum_dhcpMode;

typedef struct
{
    Enum_netName interface;   //默认为wlan0 
    char dhcp_addr_start[16]; //dhcp-start ip;
    char dhcp_addr_end[16];   //dhcp-end ip;
    char router[16];          //dhcp-router;
    char dns[16];             //dhcp-dns;
    int error_code;
} Struct_dhcpCfg; //DHCP 服务器


/**
 * @brief :  设置DNS域名解析服务器
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-06-22 15:22:38
 */
int set_net_dns(void);

/******************************************************************************************
 *功能 ：检查IP的合法性
 *参数 ： *msg-需要计算的数据
 *返回：RES_OK-合法; RES-ERROR-非法;  
 *作者 ：LR
 *修改日期 ：2019年03月29日 
*******************************************************************************************/
int check_IP_ture(char *msg);

/******************************************************************************************
 * 功能：获取网络信息
 * 参数：netName,网络接口  one value of Enum_netName.
 *              netCfg ,lan口信息结构体 one value of Struct_netCfg.
 * 返回：成功RES_OK ; 错误RES_ERROR;
 * 作者 ：LR
 * 修改日期 ：2020年03月27日 
******************************************************************************************/
int get_net_info(char *interface, Struct_netCfg *netCfg);


/******************************************************************************************
 * 功能：获取网络信息
 * 参数：netName,网络接口  one value of Enum_netName.
 *              netCfg ,lan口信息结构体 one value of Struct_netCfg.
 * 返回：成功RES_OK ; 错误RES_ERROR;
 * 作者 ：LR
 * 修改日期 ：2020年03月27日 
******************************************************************************************/
int zh_get_netInfo(Enum_netName netName, Struct_netCfg *netCfg);

/******************************************************************************************
 * 功能：设置网络信息
 * 参数：netName,网络接口  one value of Enum_netName.
 *              netCfg ,lan口信息结构体 one value of Struct_netCfg.
 * 返回：成功RES_OK ; 错误RES_ERROR;
 * 作者 ：LR
 * 修改日期 ：2020年03月27日 
******************************************************************************************/
int zh_set_netInfo(Enum_netName netName, Struct_netCfg *netCfg);

/**
 * @brief :  4G拨号
 * @param {char} *apn
 * @param {char} *user
 * @param {char} *password
 * @return {*}
 * @author: LR
 * @Date: 2021-10-29 14:40:00
 */
int zh_dataCall_init(char *apn,char *user,char *password);

/*******************************************************************************************
 * 功能：4G 撤销拨号
 * 参数：void
 * 返回：成功RES_OK ; 错误RES_ERROR;
 * 作者 ：LR
 * 修改日期 ：2020年12月8日 
*******************************************************************************************/
int zh_dataCall_deInit(void);


/********************************************************************************************
 * 功能：检查字符串组成（字母，数字 ，'_'，'-'，'@'，'#'  ）
 * 参数：*str,  
 * 返回：RES_OK 1; ERROR -1;
 *作者 ：LR
 *修改日期 ：2020年03月27日 
********************************************************************************************/
 int zh_string_check(char *str);

/*******************************************************************************************
 * 功能：wifi 配置获取
 * 参数：Enum_wifiMode ；Struct_wifiCfg；
 * 返回：RES_OK 1; RES_ERROR -1;
 *作者 ：LR
 *修改日期 ：2020年12月8日 
********************************************************************************************/
 int zh_get_wifiCfg(Enum_wifiMode wifimode, Struct_wifiCfg *wificfg);

/********************************************************************************************
 * 功能：wifi 配置设置
 * 参数：Enum_wifiMode,Struct_wifiCfg
 * 返回：RES_OK 1; RES_ERROR -1;
 *作者 ：LR
 *修改日期 ：2020年12月8日 
*******************************************************************************************/
 int zh_set_wifiCfg(Enum_wifiMode wifimode, Struct_wifiCfg *wificfg);

/*******************************************************************************************
 * 功能：dhcp 服务器配置获取
 * 参数：Struct_dhcpCfg
 * 返回：RES_OK 1; RES_ERROR -1;
 *作者 ：LR
 *修改日期 ：2020年12月8日 
*******************************************************************************************/
 int zh_get_dhcpCfg(Struct_dhcpCfg *dhcpcfg);

/*******************************************************************************************
 * 功能：dhcp 服务器配置设置
 * 参数：Struct_dhcpCfg
 * 返回：RES_OK 1; RES_ERROR -1;
 *作者 ：LR
 *修改日期 ：2020年12月8日 
*******************************************************************************************/
 int zh_set_dhcpCfg(Struct_dhcpCfg *dhcpcfg);

/*******************************************************************************************
 * 功能：使能 wifi AP
 * 参数：wifimode: wifi工作模式
 * 返回：RES_OK 1; RES_ERROR -1;
 *作者 ：LR
 *修改日期 ：2020年12月8日 
********************************************************************************************/
 int zh_wifi_enable(Enum_wifiMode wifimode);
/********************************************************************************************
 * 功能：失能 wifi
 * 参数：wifimode: wifi工作模式
 * 返回：RES_OK 1; RES_ERROR -1;
 *作者 ：LR
 *修改日期 ：2020年12月8日 
********************************************************************************************/
 int zh_wifi_disEnable(Enum_wifiMode wifimode);




/**
 * @brief :  
 * @param 
 * @return {*}
 * @author: LR
 * @Date: 2021-12-01 14:52:14
 */
int zh_dhcpS_enable(void);
/**
 * @brief :  
 * @param 
 * @return {*}
 * @author: LR
 * @Date: 2021-12-01 14:52:14
 */
int zh_dhcpS_disable(void);



 /**
  * @brief :  查看网口是否连接网线
  * @param {Enum_netName} netName
  * @return {*}
  * @author: LR
  * @Date: 2021-12-04 16:19:14
  */
 int zh_get_ethLink(Enum_netName netName);

#endif
