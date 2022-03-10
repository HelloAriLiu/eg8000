#include "zh_network.h"
#include "vpnHandle.h"

frpCfg_T frpCfg;

/**
 * @brief 设置frpc配置项 , 保存并且提交更改到文件
 * @param frpcfg  frp配置参数
 * @return int 	成功返回RES_OK, 失败返回其它值
 */
#define FRPC_DEVPATH "/home/pi/frp/frpc.conf"
int frpCfg_config(void)
{
    char frp_cfg[2048];
    snprintf(frp_cfg, sizeof(frp_cfg),
             "[common]\n\
server_addr=%s\n\
server_port=%d\n\
token=%s\n\
protocol=tcp\n\
tcp_mux=true\n\
login_fail_exit=false\n\
log_level=info\n\
log_max_days=1\n\
\n\
[ssh_%s]\n\
type=tcp\n\
local_ip=127.0.0.1\n\
local_port=22\n\
remote_port=%d\n\
use_encryption=ture\n\
use_compression=true\n\
\n\
[web_%s]\n\
type=tcp\n\
local_ip=0.0.0.0\n\
local_port=8000\n\
remote_port=%d\n\
use_encryption=ture\n\
use_compression=true\n\
\n\
[node-red_%s]\n\
type=tcp\n\
local_ip=0.0.0.0\n\
local_port=1880\n\
remote_port=%d\n\
use_encryption=ture\n\
use_compression=true\n\
\n\
[openvpn_%s]\n\
type=tcp\n\
local_ip=0.0.0.0\n\
local_port=1194\n\
remote_port=%d\n\
use_encryption=ture\n\
use_compression=true",
             frpCfg.server_addr, frpCfg.server_port, frpCfg.token,sysBasic.sn,
             frpCfg.remote_port_ssh,sysBasic.sn, frpCfg.remote_port_web,sysBasic.sn,
             frpCfg.remote_port_nodeRed,sysBasic.sn, frpCfg.remote_port_vpn);

    //printf("frp_cfg: %s \n", frp_cfg);

    FILE *fd;
    fd = fopen(FRPC_DEVPATH, "w+");
    if (fd == NULL)
    {
        log_i("fopen(FRPC_DEVPATH, \"w+\")== -1\n");
        return RES_ERROR;
    }
    fseek(fd, 0, SEEK_SET); //定位到文件开始
    fputs(frp_cfg, fd);     //写入文件
    fflush(fd);             //刷新内存缓冲区 将内容写入内核缓冲
    fsync(fileno(fd));      //将缓存区数据写入磁盘，并等待操作结束
    fclose(fd);

    return RES_OK;
}

/**
 * @brief 设置openvpn配置项 , 保存并且提交更改到文件
 * @param void 
 *              eg:openvpn --daemon --config openvpn.conf
 * @return int 	成功返回RES_OK, 失败返回其它值
 */
#define OPENVPN_DEVPATH "/etc/openvpn/server.conf"
int openvpn_config(void)
{
    char br_lan[20];
    char br_mask[20];

    Struct_netCfg netCfg;
    if (zh_get_netInfo(BR_LAN, &netCfg) != RES_OK)
    {
        log_i("zh_get_netInfo(BR_LAN, &netCfg) != RES_OK\n");
    }
    snprintf(br_lan, sizeof(br_lan), "%s", netCfg.inet_addr);
    snprintf(br_mask, sizeof(br_mask), "%s", netCfg.mask);
    log_i("br-lan info [addr:%s  Mask:%s  ]\n", br_lan, br_mask);

    /* 计算路由 */
    char *ptr1;
    char *ptr2;
    char gw[20];
    char p1[20], p2[20];
    int i = 0, a[5], b[5], c[5] = {0}, count = 0;
    sprintf(p1, "%s", br_lan);
    sprintf(p2, "%s", br_mask);

    i = 0;
    ptr1 = strtok(p1, ".");
    while (ptr1 != NULL && i < 4)
    {
        a[i++] = atoi(ptr1);
        ptr1 = strtok(NULL, ".");
    }
    i = 0;
    ptr2 = strtok(p2, ".");
    while (ptr2 != NULL && i < 4)
    {
        b[i++] = atoi(ptr2);
        ptr2 = strtok(NULL, ".");
    }
    for (i = 0; i < 4; i++)
    {
        c[i] = a[i] & b[i];
    }
    snprintf(gw, sizeof(gw), "%d.%d.%d.%d", c[0], c[1], c[2], c[3]);
    if (check_IP_ture(gw) != RES_OK)
    {
        log_i("check_IP_ture(gw[%s]) != RES_OK\n", gw);
        return RES_ERROR;
    }
    char dhcpIP_start[16], dhcpIP_end[16];
    // sprintf(dhcpIP_start, "%d.%d.%d.%d", c[0], c[1], c[2], c[3] + 2);
    // sprintf(dhcpIP_end, "%d.%d.%d.%d", c[0], c[1], c[2], c[3] + 10);
    sprintf(dhcpIP_start, "%d.%d.%d.240", c[0], c[1], c[2]);
    sprintf(dhcpIP_end, "%d.%d.%d.250", c[0], c[1], c[2]);

    if (check_IP_ture(dhcpIP_start) != RES_OK || check_IP_ture(dhcpIP_end) != RES_OK)
    {
        log_i("check_IP_ture error [%s    %s]\n", dhcpIP_start, dhcpIP_end);
        return RES_ERROR;
    }
    char vpn_cfg[2048];
    snprintf(vpn_cfg, sizeof(vpn_cfg), "port 1194\nproto tcp\ndev tap0\n\
ca /etc/openvpn/server/ca.crt\n\
cert /etc/openvpn/server/server.crt\n\
key /etc/openvpn/server/server.key\n\
dh /etc/openvpn/server/dh.pem\n\
keepalive 10 120\n\
client-to-client\n\
;max_clients 10\n\
duplicate-cn\n\
verb 3\n\
server-bridge %s %s %s %s\n\
push \"dhcp-option DNS %s\"\n\
push \"dhcp-option DNS 114.114.114.114\"\n\
push \"route 0.0.0.0  0.0.0.0\"\n\
push \"route %s %s\"\n",
             br_lan, br_mask, dhcpIP_start, dhcpIP_end, br_lan, gw, br_mask);

    //printf("vpn_cfg: %s \n", vpn_cfg);

    FILE *openvpn_conf_fd;
    openvpn_conf_fd = fopen(OPENVPN_DEVPATH, "w+");
    if (openvpn_conf_fd == NULL)
    {
        log_i(" fopen(OPENVPN_DEVPATH, \"w+\") error\n");
        return RES_ERROR;
    }
    fseek(openvpn_conf_fd, 0, SEEK_SET); //定位到文件开始
    fputs(vpn_cfg, openvpn_conf_fd);     //写入文件
    fflush(openvpn_conf_fd);             //刷新内存缓冲区 将内容写入内核缓冲
    fsync(fileno(openvpn_conf_fd));      //将缓存区数据写入磁盘，并等待操作结束
    fclose(openvpn_conf_fd);
    //sleep(1);
    return RES_OK;
}

/**
 * @brief :  frp开启结果查询
 * @param {char} *logBuff
 * @return {*}
 * @author: LR
 * @Date: 2022-01-10 15:17:57
 */
int frp_getResult(char *logBuff)
{
    int res = FRP_ERROR_UNKNOWN;                        //未知结果
    if (strstr(logBuff, "start proxy success") != NULL) //成功
    {
        res = FRP_OK;
    }
    else if (strstr(logBuff, "start error:port not allowed") != NULL) //端口不能使用
    {
        res = FRP_ERROR_PORT;
    }
    else if (strstr(logBuff, "start error: port already used") != NULL) //端口被占用
    {
        res = FRP_ERROR_PORTUSED;
    }
    else if (strstr(logBuff, "start error: proxy name") != NULL) //名字被占用
    {
        res = FRP_ERROR_NAMEUSED;
    }

    return res;
}

/**
 * @brief frp服务控制
 * @param  runstate	设置运行状态
 *         eg: RUN_STOP
 *                 RUN_START
 * @return int 	成功返回RES_OK, 失败返回其它值
 */
int frp_service_ctl(Enum_Enable runstate)
{
    if (runstate == ENABLE)
    {
        if (find_process_by_name("frpc") == RES_OK)
        {

            system("sudo killall frpc");
            sleep(1);
            //return RES_OK;
        }

        frpCfg.remote_ssh_result = FRP_ERROR_UNKNOWN;
        frpCfg.remote_web_result = FRP_ERROR_UNKNOWN;
        frpCfg.remote_nodeRed_result = FRP_ERROR_UNKNOWN;
        frpCfg.remote_vpn_result = FRP_ERROR_UNKNOWN;
        system("sudo /home/pi/frp/frpc -c /home/pi/frp/frpc.conf > /home/pi/frp/frpc.log &");
        sleep(1);
        if (find_process_by_name("frpc") != RES_OK)
        {
            return RES_ERROR;
        }
        else
        {
            FILE *f;
            f = fopen("/home/pi/frp/frpc.log", "r");
            if (f == NULL)
            {
                log_i("fopen(/home/pi/frp/frpc.log, r) error\n");
                return RES_ERROR;
            }

            //log_i("fopen(/home/pi/frp/frpc.log, r) ok\n");
            int res;
            char config_linebuf[1024];
            fseek(f, 0, SEEK_SET);                                    //定位文件指针到文件开始位置
            while (fgets(config_linebuf, 1024, f) != NULL) //按行获取
            {
                // if (fgetc(f) == EOF) //文件末尾
                // {
                //     break;
                // }
                //printf("frp Log：%s", config_linebuf);
                if (strstr(config_linebuf, "[ssh_") != NULL)
                {
                    frpCfg.remote_ssh_result = frp_getResult(config_linebuf);
                    log_i(" frpCfg.remote_ssh_result = %d\n", frpCfg.remote_ssh_result);
                }
                else if (strstr(config_linebuf, "[web_") != NULL)
                {
                    frpCfg.remote_web_result = frp_getResult(config_linebuf);
                    log_i("  frpCfg.remote_web_result = %d\n", frpCfg.remote_web_result);
                }
                else if (strstr(config_linebuf, "[node-red_") != NULL)
                {
                    frpCfg.remote_nodeRed_result = frp_getResult(config_linebuf);
                    log_i("  frpCfg.remote_nodeRed_result = %d\n", frpCfg.remote_nodeRed_result);
                }
                else if (strstr(config_linebuf, "[openvpn_") != NULL)
                {
                    frpCfg.remote_vpn_result = frp_getResult(config_linebuf);
                    log_i("  frpCfg.remote_vpn_result = %d\n", frpCfg.remote_vpn_result);
                }

                //fseek(f, -1, SEEK_CUR); //定位文件指针为当前位置向下移动一行
            }

            fclose(f);
            log_i("frp_service_ctl Enable\n");
            return RES_OK;
        }
    }
    else if (runstate == DISABLE)
    {
        frpCfg.remote_ssh_result = FRP_ERROR_UNKNOWN;
        frpCfg.remote_web_result = FRP_ERROR_UNKNOWN;
        frpCfg.remote_nodeRed_result = FRP_ERROR_UNKNOWN;
        frpCfg.remote_vpn_result = FRP_ERROR_UNKNOWN;

        if (find_process_by_name("frpc") != RES_OK)
        {
            return RES_OK;
        }

        //system("ps -ef | grep frpc | grep -v grep |awk '{print $2}'|xargs sudo  kill -9");
        log_i("frp_service_ctl Disable\n");
        system("sudo killall frpc");
        usleep(500000);
        // if (find_process_by_name("frpc") != RES_OK)
        // {
        //     return RES_OK;
        // }
        // else
        // {
        //     return RES_ERROR;
        // }
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }
}

/**
 * @brief openvpn服务控制
 * @param  runstate	设置运行状态
 *         eg: RUN_STOP
 *                 RUN_START
 * @return int 	成功返回RES_OK, 失败返回其它值
 */
int openvpn_service_ctl(Enum_Enable runstate)
{
    if (runstate == ENABLE)
    {
        //system("/usr/sbin/openvpn --config  /etc/openvpn/server.conf &");
        system("sudo /etc/init.d/openvpn restart");
        sleep(5); //openvpn启动后大概10s后tap0接口才能加载成功。否则可能造成失败
        system("sudo ifconfig tap0 up");
        system("sudo brctl  addif  br-lan tap0");
        if (find_process_by_name("openvpn") != RES_OK)
        {
            return RES_ERROR;
        }
        else
        {
            return RES_OK;
        }
    }
    else if (runstate == DISABLE)
    {
        system("sudo /etc/init.d/openvpn stop");
        usleep(200000);
        if (find_process_by_name("openvpn") != RES_ERROR)
        {
            return RES_OK;
        }
        else
        {
            return RES_ERROR;
        }
    }
    else
    {
        return RES_ERROR;
    }
}

/**
 * @brief frp连接状态检查
 * @param void
 * @return void
 */
void frp_connect_check(void)
{
    static unsigned int frp_off_add_tick = 0;

    if (frpCfg.enable == DISABLE)
    {
        frpCfg.check_tick = 0;
        frp_off_add_tick = 0;
        return;
    }

    if (frpCfg.check_tick > 30)
    {
        if (find_process_by_name("frpc") == RES_OK) //frp存活
        {
            if (frpCfg.connect_flag == 0)
            {
                frp_off_add_tick++;
                if (frp_off_add_tick > 3)
                {
                    frp_service_ctl(DISABLE);
                    frpCfg.enable = DISABLE;
                    log_i("frp_service_ctl passive check  down\n");
                    frp_off_add_tick = 0;
                }
            }
            else
            {
                frp_off_add_tick = 0;
            }
        }

        frpCfg.check_tick = 0;
        frpCfg.connect_flag = 0;
    }
}

/**
 * @brief :  远程计数器
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2022-01-18 17:08:09
 */
void vpn_timerHandle(void)
{
    frpCfg.check_tick++;
}
