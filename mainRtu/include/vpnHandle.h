/*
 * @Description :  
 * @FilePath: /app_code/mainRtu/include/vpnHandle.h
 * @Author:  LR
 * @Date: 2022-01-06 15:08:26
 */
#ifndef _VPNHANDLE_H
#define _VPNHANDLE_H
#include "baseInit.h"


typedef struct
{
    char enable;
    char server_addr[64]; //远程服务器IP
    int server_port;      //远程服务器端口
    char token[64];       //frp服务器token验证

    //远程映射端口
    int remote_port_ssh; //远程映射端口-升级
    int remote_ssh_result;
    int remote_port_web; //远程映射端口-升级
    int remote_web_result;
    int remote_port_nodeRed; //远程映射端口-升级
    int remote_nodeRed_result;
    int remote_port_vpn; //远程映射端口-升级
    int remote_vpn_result;

    int check_tick;
    int connect_flag;

} frpCfg_T; //frp外部参数
extern frpCfg_T frpCfg;

typedef enum
{
    FRP_OK = 0,
    FRP_ERROR_PORT = 1,
    FRP_ERROR_PORTUSED = 2,
     FRP_ERROR_NAMEUSED = 3,
    FRP_ERROR_UNKNOWN = 255
} frp_result;

/**
 * @brief 设置frpc配置项 , 保存并且提交更改到文件
 * @param frpcfg  frp配置参数
 * @return int 	成功返回RES_OK, 失败返回其它值
 */
int frpCfg_config(void);

/**
 * @brief 设置openvpn配置项 , 保存并且提交更改到文件
 * @param void 
 *              eg:openvpn --daemon --config openvpn.conf
 * @return int 	成功返回RES_OK, 失败返回其它值
 */
int openvpn_config(void);

/**
 * @brief frp服务控制
 * @param  runstate	设置运行状态
 *         eg: RUN_STOP
 *                 RUN_START
 * @return int 	成功返回RES_OK, 失败返回其它值
 */
int frp_service_ctl(Enum_Enable runstate);


/**
 * @brief frp连接状态检查
 * @param void
 * @return void
 */
void frp_connect_check(void);

/**
 * @brief :  远程计数器
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2022-01-18 17:08:09
 */
void vpn_timerHandle(void);

#endif
