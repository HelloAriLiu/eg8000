#include "baseInit.h"
#include "zh_usart.h"
#include "zh_network.h"
#include "cJSON.h"

sysBasic_T sysBasic;
lteBasic_T lteBasic;
wifiBasic_T wifiBasic;
lanBasic_T lanBasic;
wanBasic_T wanBasic;

/**
 * @brief :  日志初始化
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-06-05 14:41:04
 */
void log_init(void)
{
    /* close printf buffer */
    setbuf(stdout, NULL);
    /* initialize EasyLogger */
    elog_init();
    /* set EasyLogger log format */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL & ~ELOG_FMT_P_INFO & ~ELOG_FMT_T_INFO & ~ELOG_FMT_DIR);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_ALL & ~ELOG_FMT_P_INFO & ~ELOG_FMT_T_INFO & ~ELOG_FMT_DIR);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_ALL & ~ELOG_FMT_P_INFO & ~ELOG_FMT_T_INFO & ~ELOG_FMT_DIR);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_ALL & ~ELOG_FMT_P_INFO & ~ELOG_FMT_T_INFO & ~ELOG_FMT_DIR);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_P_INFO & ~ELOG_FMT_T_INFO & ~ELOG_FMT_DIR);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_P_INFO & ~ELOG_FMT_T_INFO & ~ELOG_FMT_DIR);
#ifdef ELOG_COLOR_ENABLE
    elog_set_text_color_enabled(true);
#endif
    /* start EasyLogger */
    elog_start();
}

/**
 * @brief :  复位配置
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-06-05 14:41:04
 */
int reset_dev_cfg(void)
{
    snprintf(sysBasic.userPassword, sizeof(sysBasic.userPassword), "EG12345678");
    snprintf(sysBasic.model, sizeof(sysBasic.model), "EG8000");
    snprintf(sysBasic.sn, sizeof(sysBasic.sn), "%s", zh_get_sn());
    snprintf(sysBasic.nodeVersion, sizeof(sysBasic.nodeVersion), "%s", zh_get_nodeVersion()); // 获取SN
    snprintf(sysBasic.version, sizeof(sysBasic.version), "%s_%s", DEV_VERSION, sysBasic.nodeVersion);
    sysBasic.defaultNet = NET_WAN;

    snprintf(lteBasic.lteAPN.addr, sizeof(lteBasic.lteAPN.addr), "");
    snprintf(lteBasic.lteAPN.username, sizeof(lteBasic.lteAPN.username), "");
    snprintf(lteBasic.lteAPN.password, sizeof(lteBasic.lteAPN.password), "");

    wifiBasic.enable = DISABLE;
    snprintf(wifiBasic.ssid, sizeof(wifiBasic.ssid), "AP-EG8000");
    snprintf(wifiBasic.password, sizeof(wifiBasic.password), "EG12345678");

    snprintf(lanBasic.ip, sizeof(lanBasic.ip), DEF_LAN_IP);
    snprintf(lanBasic.mask, sizeof(lanBasic.mask), DEF_LAN_MASK);
    snprintf(lanBasic.gateway, sizeof(lanBasic.gateway), DEF_LAN_IP);

    lanBasic.dhcpsBasic.enable = DISABLE;
    snprintf(lanBasic.dhcpsBasic.startIp, sizeof(lanBasic.dhcpsBasic.startIp), "192.168.88.100");
    snprintf(lanBasic.dhcpsBasic.endIp, sizeof(lanBasic.dhcpsBasic.endIp), "192.168.88.200");

    wanBasic.ipMode = IP_MODE_DHCPC;
    //wanBasic.ipMode = IP_MODE_STATIC;
    snprintf(wanBasic.ip, sizeof(wanBasic.ip), "192.168.0.88");
    snprintf(wanBasic.mask, sizeof(wanBasic.mask), "255.255.255.0");
    snprintf(wanBasic.gateway, sizeof(wanBasic.gateway), "192.168.0.1");

    return RES_OK;
}

/**
 * @brief :  获取设备配置
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-06-09 11:19:15
 */
int get_dev_cfg(void)
{
    char config_linebuf[512];
    //判断文件是否存在,打开
    FILE *f;
    f = fopen(DEV_CFG_PATH, "r+");
    if (f == NULL)
    {
        log_e("open DEV_CFG_PATH error!\n");
        return RES_ERROR;
    }

    fseek(f, 0, SEEK_END);        //定位到文件指针末尾
    long congig_lenth = ftell(f); //获取整个文件的字符个数
    if (congig_lenth < 0)
    {
        fclose(f);
        return RES_ERROR;
    }

    char sum_buf[congig_lenth + 1024]; //先分配一个可能最大的数组
    memset(sum_buf, 0, sizeof(sum_buf));

    fseek(f, 0, SEEK_SET); //定位到文件指针开始
    while (fgets(config_linebuf, 512, f) != NULL)
    {
        strcat(sum_buf, config_linebuf);
        if (fgetc(f) == EOF)
        {
            break;
        }
        fseek(f, -1, SEEK_CUR); //定位到当前位置的下一行
        memset(config_linebuf, 0, sizeof(config_linebuf));
    }
    fclose(f);

    /***********************************配置文件json解析******************************************/
    cJSON *pJsonRoot;
    cJSON *pSubJson;
    cJSON *pThdJson;

    char temp[256];

    pJsonRoot = cJSON_Parse(sum_buf); //json解析，获得句柄
    if (pJsonRoot == NULL)
    {
        log_e("get_dev_cfg  cJSON_Parse devcfgfile  error\n");
        return RES_ERROR;
    }

    pSubJson = cJSON_GetObjectItem(pJsonRoot, "sysBasic");
    {
        pThdJson = cJSON_GetObjectItem(pSubJson, "userPassword");
        if (pThdJson == NULL || pThdJson->valuestring == NULL)
        {
            cJSON_Delete(pJsonRoot);
            return RES_ERROR;
        }
        snprintf(sysBasic.userPassword, sizeof(sysBasic.userPassword), "%s", pThdJson->valuestring);

        pThdJson = cJSON_GetObjectItem(pSubJson, "defaultNet");
        if (pThdJson == NULL || pThdJson->type != cJSON_Number)
        {
            cJSON_Delete(pJsonRoot);
            return RES_ERROR;
        }
        sysBasic.defaultNet = pThdJson->valueint;
    }

    pSubJson = cJSON_GetObjectItem(pJsonRoot, "lteBasic");
    {
        pThdJson = cJSON_GetObjectItem(pSubJson, "apnAddr");
        if (pThdJson == NULL || pThdJson->valuestring == NULL)
        {
            cJSON_Delete(pJsonRoot);
            return RES_ERROR;
        }
        snprintf(lteBasic.lteAPN.addr, sizeof(lteBasic.lteAPN.addr), "%s", pThdJson->valuestring);

        pThdJson = cJSON_GetObjectItem(pSubJson, "username");
        if (pThdJson == NULL || pThdJson->valuestring == NULL)
        {
            cJSON_Delete(pJsonRoot);
            return RES_ERROR;
        }
        snprintf(lteBasic.lteAPN.username, sizeof(lteBasic.lteAPN.username), "%s", pThdJson->valuestring);

        pThdJson = cJSON_GetObjectItem(pSubJson, "password");
        if (pThdJson == NULL || pThdJson->valuestring == NULL)
        {
            cJSON_Delete(pJsonRoot);
            return RES_ERROR;
        }
        snprintf(lteBasic.lteAPN.password, sizeof(lteBasic.lteAPN.password), "%s", pThdJson->valuestring);
    }

    pSubJson = cJSON_GetObjectItem(pJsonRoot, "wifiBasic");
    {
        pThdJson = cJSON_GetObjectItem(pSubJson, "enable");
        if (pThdJson == NULL || pThdJson->type != cJSON_Number)
        {
            cJSON_Delete(pJsonRoot);
            return RES_ERROR;
        }
        wifiBasic.enable = pThdJson->valueint;

        pThdJson = cJSON_GetObjectItem(pSubJson, "ssid");
        if (pThdJson == NULL || pThdJson->valuestring == NULL)
        {
            cJSON_Delete(pJsonRoot);
            return RES_ERROR;
        }
        snprintf(wifiBasic.ssid, sizeof(wifiBasic.ssid), "%s", pThdJson->valuestring);

        pThdJson = cJSON_GetObjectItem(pSubJson, "password");
        if (pThdJson == NULL || pThdJson->valuestring == NULL)
        {
            cJSON_Delete(pJsonRoot);
            return RES_ERROR;
        }
        snprintf(wifiBasic.password, sizeof(wifiBasic.password), "%s", pThdJson->valuestring);
    }

    pSubJson = cJSON_GetObjectItem(pJsonRoot, "lanBasic");
    {
        pThdJson = cJSON_GetObjectItem(pSubJson, "ip");
        if (pThdJson == NULL || pThdJson->valuestring == NULL)
        {
            cJSON_Delete(pJsonRoot);
            return RES_ERROR;
        }
        snprintf(lanBasic.ip, sizeof(lanBasic.ip), "%s", pThdJson->valuestring);

        pThdJson = cJSON_GetObjectItem(pSubJson, "mask");
        if (pThdJson == NULL || pThdJson->valuestring == NULL)
        {
            cJSON_Delete(pJsonRoot);
            return RES_ERROR;
        }
        snprintf(lanBasic.mask, sizeof(lanBasic.mask), "%s", pThdJson->valuestring);

        pThdJson = cJSON_GetObjectItem(pSubJson, "gateway");
        if (pThdJson == NULL || pThdJson->valuestring == NULL)
        {
            cJSON_Delete(pJsonRoot);
            return RES_ERROR;
        }
        snprintf(lanBasic.gateway, sizeof(lanBasic.gateway), "%s", pThdJson->valuestring);

        pThdJson = cJSON_GetObjectItem(pSubJson, "dhcpEnable");
        if (pThdJson == NULL || pThdJson->type != cJSON_Number)
        {
            cJSON_Delete(pJsonRoot);
            return RES_ERROR;
        }
        lanBasic.dhcpsBasic.enable = pThdJson->valueint;

        pThdJson = cJSON_GetObjectItem(pSubJson, "dhcpStartIp");
        if (pThdJson == NULL || pThdJson->valuestring == NULL)
        {
            cJSON_Delete(pJsonRoot);
            return RES_ERROR;
        }
        snprintf(lanBasic.dhcpsBasic.startIp, sizeof(lanBasic.dhcpsBasic.startIp), "%s", pThdJson->valuestring);

        pThdJson = cJSON_GetObjectItem(pSubJson, "dhcpEndIp");
        if (pThdJson == NULL || pThdJson->valuestring == NULL)
        {
            cJSON_Delete(pJsonRoot);
            return RES_ERROR;
        }
        snprintf(lanBasic.dhcpsBasic.endIp, sizeof(lanBasic.dhcpsBasic.endIp), "%s", pThdJson->valuestring);
    }

    pSubJson = cJSON_GetObjectItem(pJsonRoot, "wanBasic");
    {
        pThdJson = cJSON_GetObjectItem(pSubJson, "ipMode");
        if (pThdJson == NULL || pThdJson->type != cJSON_Number)
        {
            cJSON_Delete(pJsonRoot);
            return RES_ERROR;
        }
        wanBasic.ipMode = pThdJson->valueint;

        pThdJson = cJSON_GetObjectItem(pSubJson, "ip");
        if (pThdJson == NULL || pThdJson->valuestring == NULL)
        {
            cJSON_Delete(pJsonRoot);
            return RES_ERROR;
        }
        snprintf(wanBasic.ip, sizeof(wanBasic.ip), "%s", pThdJson->valuestring);

        pThdJson = cJSON_GetObjectItem(pSubJson, "mask");
        if (pThdJson == NULL || pThdJson->valuestring == NULL)
        {
            cJSON_Delete(pJsonRoot);
            return RES_ERROR;
        }
        snprintf(wanBasic.mask, sizeof(wanBasic.mask), "%s", pThdJson->valuestring);

        pThdJson = cJSON_GetObjectItem(pSubJson, "gateway");
        if (pThdJson == NULL || pThdJson->valuestring == NULL)
        {
            cJSON_Delete(pJsonRoot);
            return RES_ERROR;
        }
        snprintf(wanBasic.gateway, sizeof(wanBasic.gateway), "%s", pThdJson->valuestring);
    }

    cJSON_Delete(pJsonRoot);
    return RES_OK;
}

/**
 * @brief :  设置设备配置
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-06-09 11:19:47
 */
int set_dev_cfg(void)
{
    //默认基础参数
    cJSON *pJsonRoot;
    cJSON *pSubJson;
    cJSON *pThdJson;

    char *p = NULL;
    char *pStr = NULL;

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        log_e("set_dev_cfg :cJSON_CreateObject  fail\n");
        return RES_ERROR;
    }

    cJSON_AddItemToObject(pJsonRoot, "sysBasic", pSubJson = cJSON_CreateObject());
    {
        cJSON_AddStringToObject(pSubJson, "userPassword", sysBasic.userPassword);
        cJSON_AddStringToObject(pSubJson, "model", sysBasic.model);
        cJSON_AddStringToObject(pSubJson, "version", sysBasic.version);
        cJSON_AddStringToObject(pSubJson, "sn", sysBasic.sn);
        cJSON_AddNumberToObject(pSubJson, "defaultNet", sysBasic.defaultNet);
    }

    cJSON_AddItemToObject(pJsonRoot, "lteBasic", pSubJson = cJSON_CreateObject());
    {
        cJSON_AddStringToObject(pSubJson, "apnAddr", lteBasic.lteAPN.addr);
        cJSON_AddStringToObject(pSubJson, "username", lteBasic.lteAPN.username);
        cJSON_AddStringToObject(pSubJson, "password", lteBasic.lteAPN.password);
    }

    cJSON_AddItemToObject(pJsonRoot, "wifiBasic", pSubJson = cJSON_CreateObject());
    {
        cJSON_AddNumberToObject(pSubJson, "enable", wifiBasic.enable);
        cJSON_AddStringToObject(pSubJson, "ssid", wifiBasic.ssid);
        cJSON_AddStringToObject(pSubJson, "password", wifiBasic.password);
    }

    cJSON_AddItemToObject(pJsonRoot, "lanBasic", pSubJson = cJSON_CreateObject());
    {
        cJSON_AddStringToObject(pSubJson, "ip", lanBasic.ip);
        cJSON_AddStringToObject(pSubJson, "mask", lanBasic.mask);
        cJSON_AddStringToObject(pSubJson, "gateway", lanBasic.gateway);
        cJSON_AddNumberToObject(pSubJson, "dhcpEnable", lanBasic.dhcpsBasic.enable);
        cJSON_AddStringToObject(pSubJson, "dhcpStartIp", lanBasic.dhcpsBasic.startIp);
        cJSON_AddStringToObject(pSubJson, "dhcpEndIp", lanBasic.dhcpsBasic.endIp);
    }

    cJSON_AddItemToObject(pJsonRoot, "wanBasic", pSubJson = cJSON_CreateObject());
    {
        cJSON_AddNumberToObject(pSubJson, "ipMode", wanBasic.ipMode);
        cJSON_AddStringToObject(pSubJson, "ip", wanBasic.ip);
        cJSON_AddStringToObject(pSubJson, "mask", wanBasic.mask);
        cJSON_AddStringToObject(pSubJson, "gateway", wanBasic.gateway);
    }

    p = cJSON_Print(pJsonRoot);
    cJSON_Delete(pJsonRoot);

    FILE *fp;
    fp = fopen(DEV_CFG_PATH, "w+");
    if (fp == NULL)
    {
        log_e("set_dev_cfg:Open devinfo file(w+) fail... \n");
        free(p);
        return RES_ERROR;
    }
    fseek(fp, 0, SEEK_SET); //定位到文件开始
    fputs(p, fp);           //写入文件
    fflush(fp);             //刷新内存缓冲区 将内容写入内核缓冲
    fsync(fileno(fp));      //将缓存区数据写入磁盘，并等待操作结束
    fclose(fp);
    free(p);
    log_i("set_dev_cfg successful \n");

    return RES_OK;
}

/**
 * @brief :  获取掩码位数
 * @param {char} *mask
 * @return {*}
 * @author: LR
 * @Date: 2022-01-11 16:44:10
 */
int get_mask_num(char *mask)
{
    char *ptr2;
    char p1[20], p2[20];
    int i = 0, j = 0, b[5], count = 0;
    sprintf(p2, "%s", mask);

    ptr2 = strtok(p2, ".");
    while (ptr2 != NULL && i < 4)
    {
        b[i++] = atoi(ptr2);
        ptr2 = strtok(NULL, ".");
    }

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (((b[i] >> j) & 1) == 1)
                count++;
        }
    }
    return count;
}

/***************************************************
 * 功能：eth配置设置
 * 参数：eth0 =wan    eth1绑定br-lan 
 * 返回：RES_OK 1; RES_ERROR -1;
 *作者 ：LR
 *修改日期 ：2020年12月8日 
***************************************************/
#define ETH_CONF_DEVPATH "/etc/dhcpcd.conf"
int zh_set_ethCfg(void)
{
    // int mask_num = get_mask_num(lanBasic.mask);

    char databuff[2048];
    FILE *conf_fd;
    conf_fd = fopen(ETH_CONF_DEVPATH, "w+"); //
    if (conf_fd == NULL)
    {
        log_e("fopen(ETH_CONF_DEVPATH, \"w+\") Failed\n");
        return RES_ERROR;
    }
    //snprintf(databuff,sizeof(databuff), "hostname\nclientid\npersistent\noption rapid_commit\noption domain_name_servers, domain_name, domain_search, host_name\noption classless_static_routes\noption interface_mtu\nrequire dhcp_server_identifier\nslaac private\n\ninterface eth1\nstatic ip_address=%s/%d\nstatic routers=%s\nstatic domain_name_servers=114.114.114.114 8.8.8.8\n\n", lanBasic.ip,mask_num,lanBasic.gateway);

    snprintf(databuff, sizeof(databuff), "hostname\nclientid\npersistent\noption rapid_commit\noption domain_name_servers, domain_name, domain_search, host_name\noption classless_static_routes\noption interface_mtu\nrequire dhcp_server_identifier\nslaac private\n\ninterface eth1\nstatic ip_address=0.0.0.0/24\n\n");

    if (wanBasic.ipMode == IP_MODE_STATIC)
    {
        int wanMask_num = get_mask_num(wanBasic.mask);

        char wancfg[1024];
        sprintf(wancfg, "\ninterface eth0\nstatic ip_address=%s/%d\nstatic routers=%s\nstatic domain_name_servers=114.114.114.114 8.8.8.8\n\n", wanBasic.ip, wanMask_num, wanBasic.gateway);

        strcat(databuff, wancfg);
    }
    fseek(conf_fd, 0, SEEK_SET); //定位到文件开始
    fputs(databuff, conf_fd);    //写入文件
    fflush(conf_fd);             //刷新内存缓冲区 将内容写入内核缓冲
    fsync(fileno(conf_fd));      //将缓存区数据写入磁盘，并等待操作结束
    fclose(conf_fd);
    log_i("wan set ok!\n");
    return RES_OK;
}

/**
 * @brief :  4g重播 调用线程
 * @param {void} *thr
 * @return {*}
 * @author: LR
 * @Date: 2021-12-04 16:06:56
 */
void *fun_lte_thrReceiveHandler(void *thr)
{
    prctl(PR_SET_NAME, "4G_check_work");

    FILE *pstr;
    char *temp;
    char cmd[512], buff[512];
    Struct_netCfg netCfg;
    int recall_tick = 600;

    //curl --interface ppp0 --connect-timeout 10  -s www.baidu.com -w %{http_code} | tail -n1
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "curl --interface ppp0 --connect-timeout 10  -s \"www.baidu.com\" -w %%{http_code} | tail -n1");
    while (1)
    {
        if (recall_tick > 600) //10min
        {
            pstr = popen(cmd, "r");
            if (pstr != NULL)
            {
                sleep(10);
                memset(buff, 0, sizeof(buff));
                fgets(buff, 512, pstr);
                printf("4G check curl:%s\n", buff);
                pclose(pstr);
                if (strstr(buff, "200") == NULL) //4G网络断开重播
                {
                    log_i("4G check -recall\n");
                    if ((temp = get_at_ccid()) != NULL)
                    {
                        snprintf(lteBasic.ccid, sizeof(lteBasic.ccid), "%s", temp); // 获取4gID
                        log_i("Get dev 4G->CCID: %s\n", lteBasic.ccid);
                    }
                    lteBasic.signal = get_at_csq();
                    log_i("Get dev 4G->CSQ: %d\n", lteBasic.signal);

                    zh_dataCall_deInit();
                    if (zh_dataCall_init(lteBasic.lteAPN.addr, lteBasic.lteAPN.username, lteBasic.lteAPN.password) == RES_OK)
                    {
                        if (zh_get_netInfo(PPP0, &netCfg) == RES_OK)
                        {
                            snprintf(lteBasic.ip, sizeof(lteBasic.ip), "%s", netCfg.inet_addr);
                            snprintf(lteBasic.mask, sizeof(lteBasic.mask), "%s", netCfg.mask);
                            log_i("4G info [addr:%s  Mask:%s  ]\n", lteBasic.ip, lteBasic.mask);
                        }
                        set_net_dns();
                        system("sudo route del default  dev eth0");                           //删除ETH0口的默认路由
                        system("sudo route add default  dev ppp0");                           //添加4G为默认路由
                        system("sudo iptables -t nat  -A POSTROUTING -o ppp0 -j MASQUERADE"); /* 开启4G路由转发 */
                    }
                }
            }
            recall_tick = 0;
        }

        recall_tick++;
        sleep(1);
    }
    return NULL;
}

/**
 * @brief :  上电初始化
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-06-05 14:41:04
 */
int init_dev_cfg(void)
{
    snprintf(sysBasic.model, sizeof(sysBasic.model), "EG8000");
    snprintf(sysBasic.sn, sizeof(sysBasic.sn), "%s", zh_get_sn());                            // 获取SN
    snprintf(sysBasic.nodeVersion, sizeof(sysBasic.nodeVersion), "%s", zh_get_nodeVersion()); // 获取node 版本
    snprintf(sysBasic.version, sizeof(sysBasic.version), "%s_%s", DEV_VERSION, sysBasic.nodeVersion);
    log_i("Get dev Board->SN[%s] -Version[%s]\n", sysBasic.sn, sysBasic.version);

    if (check_IP_ture(lanBasic.ip) != RES_OK || check_IP_ture(lanBasic.mask) != RES_OK)
    {
        snprintf(lanBasic.ip, sizeof(lanBasic.ip), DEF_LAN_IP);
        snprintf(lanBasic.mask, sizeof(lanBasic.mask), DEF_LAN_MASK);
    }
    char strtemp[255];
    sprintf(strtemp, "sudo ifconfig br-lan %s netmask %s", lanBasic.ip, lanBasic.mask);
    system(strtemp);

    if (lanBasic.dhcpsBasic.enable == ENABLE)
    {
        zh_dhcpS_disable();
        Struct_dhcpCfg *p_dhcp;
        Struct_dhcpCfg dhcpCfg_s;
        p_dhcp = &dhcpCfg_s;
        p_dhcp->interface = BR_LAN;
        snprintf(p_dhcp->dhcp_addr_start, sizeof(p_dhcp->dhcp_addr_start), "%s", lanBasic.dhcpsBasic.startIp); //DHCP 起始地址
        snprintf(p_dhcp->dhcp_addr_end, sizeof(p_dhcp->dhcp_addr_end), "%s", lanBasic.dhcpsBasic.endIp);       //DHCP 结束地址
        sprintf(p_dhcp->router, "%s", lanBasic.ip);                                                            //DHCP网关
        sprintf(p_dhcp->dns, "%s", lanBasic.ip);                                                               //DHCP DNS
        if (zh_set_dhcpCfg(p_dhcp) != RES_OK)                                                                  // 配置ＤＨＣＰ服务器设置
        {
            log_e("SET DHCP ERROR[%d]\n", p_dhcp->error_code);
        }
        log_i("DHCPS CFG SUCCESS !!! \n");

        zh_dhcpS_enable();
    }
    else
    {
        zh_dhcpS_disable();
    }

    if (sysBasic.defaultNet == NET_4G)
    {
        log_i("Init Net -> 4G\n");
        system("sudo route del default  dev eth0"); //删除ETH0口的默认路由
        system("sudo route del default  dev eth1"); //删除ETH1口的默认路由
        if (zh_dataCall_init(lteBasic.lteAPN.addr, lteBasic.lteAPN.username, lteBasic.lteAPN.password) == RES_OK)
        {
            Struct_netCfg netCfg;
            if (zh_get_netInfo(PPP0, &netCfg) == RES_OK)
            {
                snprintf(lteBasic.ip, sizeof(lteBasic.ip), "%s", netCfg.inet_addr);
                snprintf(lteBasic.mask, sizeof(lteBasic.mask), "%s", netCfg.mask);
                log_i("4G info [addr:%s  Mask:%s  ]\n", lteBasic.ip, lteBasic.mask);
            }
        }
        sleep(1);
        system("sudo route add default  dev ppp0");                            //添加4G为默认路由
        system(" sudo iptables -t nat  -A POSTROUTING -o ppp0 -j MASQUERADE"); /* 开启4G路由转发 */

        pthread_t thrReceive = 0;
        if (pthread_create(&thrReceive, NULL, fun_lte_thrReceiveHandler, NULL) == 0)
            pthread_detach(thrReceive); // 线程分离，结束时自动回收资源
        usleep(5000);
    }
    else //NET_WAN
    {
        log_i("Init Net -> Wan \n");
        system("sudo route del default  dev ppp0");                            //删除ppp口的默认路由
        system("sudo route del default  dev eth1");                            //删除ETH1口的默认路由
        //system("sudo route add default  dev eth0");                            //添加ETH0口的默认路由
        system(" sudo iptables -t nat  -A POSTROUTING -o eth0 -j MASQUERADE"); /* 开启eth0路由转发 */
    }

    char *temp;
    if ((temp = get_at_ccid()) != NULL)
    {
        snprintf(lteBasic.ccid, sizeof(lteBasic.ccid), "%s", temp); // 获取4gID
        log_i("Get dev Board->CCID: %s\n", lteBasic.ccid);
    }
    lteBasic.signal = get_at_csq();
    log_i("Get dev Board->CSQ: %d\n", lteBasic.signal);

    set_net_dns();
    system("echo 1 | sudo tee /proc/sys/net/ipv4/ip_forward "); /* 开启路由转发 */
    return RES_OK;
}

/**
 * @brief :  
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-12-01 16:13:31
 */
void init_dev(void)
{
    log_init();
    if(zh_board_init()!=RES_OK)
    {
        log_e("zh_board_init Failed\n");
        exit(0);
    }
    

    memset(&sysBasic, 0, sizeof(sysBasic_T));
    memset(&lteBasic, 0, sizeof(lteBasic_T));
    memset(&wifiBasic, 0, sizeof(wifiBasic_T));
    memset(&lanBasic, 0, sizeof(lanBasic_T));
    memset(&wanBasic, 0, sizeof(wanBasic_T));

    if (get_dev_cfg() == RES_OK) // 读取配置
    {
        log_i("Get of dev config Success\n");
    }
    else // 如果加载配置失败，写入默认配置
    {
        log_w("Get of dev config Failed->Reset Default\n");
        reset_dev_cfg();
        if (set_dev_cfg() != RES_OK)
        {
            log_e("Reset dev default config Failed\n");
            exit(0);
        }
        log_i("Reset dev default config Success\n");
    }

    zh_set_ethCfg();
    init_dev_cfg();
    openvpn_config();
    openvpn_service_ctl(ENABLE);
}
