/*
 * @Description :  
 * @FilePath: /app_code/Zh_Driver/src/zh_network.c
 * @Author:  LR
 * @Date: 2021-09-10 16:36:12
 */
#include "zh_base.h"
#include "zh_network.h"

/**
 * @brief :  设置DNS域名解析服务器
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-06-22 15:22:38
 */
int set_net_dns(void)
{
    char dnsStr[256];
    sprintf(dnsStr, "domain AirDream\nnameserver 114.114.114.114\nnameserver 8.8.8.8\n");

    FILE *fp;
    fp = fopen("/etc/resolv.conf", "w+");
    if (fp == NULL)
    {
        printf("Open DNS  file(/etc/resolv.conf  w+)  Error \n");
        return RES_ERROR;
    }

    fseek(fp, 0, SEEK_SET); //定位到文件开始
    fputs(dnsStr, fp);      //写入文件
    fflush(fp);             //刷新内存缓冲区 将内容写入内核缓冲
    fsync(fileno(fp));      //将缓存区数据写入磁盘，并等待操作结束
    fclose(fp);
    printf("DNS设置成功：nameserver 114.114.114.114　nameserver 8.8.8.8　\n");
    return RES_OK;
}

/******************************************************************************************
*功能描述 ：获取接口的IP和子网沿码
*参数 ：char *interface-接口名称，例如eth0 bridge0等 （ifconfig查询得到）
*返回值 ： 读取返回到=的字符串
*作者 ：LR
*修改日期 ：2019年04月9日 
*******************************************************************************************/
int get_net_info(char *interface, Struct_netCfg *netCfg)
{
    int rc = 0;
    struct sockaddr_in *addr = NULL;

    char *ifname = interface;

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(struct ifreq));

    /* 0. create a socket */
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (fd == -1)
    {
        return RES_ERROR;
    }

    /* 1. set type of address to retrieve : IPv4 */
    ifr.ifr_addr.sa_family = AF_INET;
    /* 2. copy interface name to ifreq structure */
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
    /* 3. get the IP address */
    if ((rc = ioctl(fd, SIOCGIFADDR, &ifr)) != 0)
    {
        goto done;
    }

    char ipv4[16] = {0};
    addr = (struct sockaddr_in *)&ifr.ifr_addr;
    strncpy(netCfg->inet_addr, inet_ntoa(addr->sin_addr), sizeof(ipv4));

    /* 4. get the mask */
    if ((rc = ioctl(fd, SIOCGIFNETMASK, &ifr)) != 0)
        goto done;

    char mask[16] = {0};
    addr = (struct sockaddr_in *)&ifr.ifr_addr;
    strncpy(netCfg->mask, inet_ntoa(addr->sin_addr), sizeof(mask));

    return RES_OK;

    /* 6. close the socket */
done:
    close(fd);
    return RES_ERROR;
}

/******************************************************************************************
*功能描述 ：检查IP的合法性
*参数 ： *msg-需要计算的数据
*返回值 ：RES_OK-合法; RES-ERROR-非法;  
*作者 ：LR
*修改日期 ：2019年03月29日 
*******************************************************************************************/
int check_IP_ture(char *msg)
{
    char *ptr;
    int a, i = 0, count = 0;
    char p[20];
    sprintf(p, msg, strlen(msg));
    //memcpy(p, msg, strlen(msg));
    //1.判断是不是3个“.”
    //2.判断最前面一个是0
    //3.判断是不是4部分数字
    //4.第一个数字不能为0
    while (p[i] != '\0')
    {
        if (p[i] == '.')
            count++;
        i++;
    }
    if (count != 3)
        return RES_ERROR;

    count = 0;
    ptr = strtok(p, ".");
    while (ptr != NULL)
    {
        count++;
        if (ptr[0] == '0' && isdigit(ptr[1]))
            return RES_ERROR;
        a = atoi(ptr);
        if (count == 1 && a == 0)
            return RES_ERROR;
        if (a < 0 || a > 255)
            return RES_ERROR;
        ptr = strtok(NULL, ".");
    }

    if (count == 4)
        return RES_OK;
    else
        return RES_ERROR;
}
/******************************************************************************************
 * 功能：获取网络信息
 * 参数：netName,网络接口  one value of Enum_netName.
 *              netCfg ,lan口信息结构体 one value of Struct_netCfg.
 * 返回：成功RES_OK ; 错误RES_ERROR;
 * 作者 ：LR
 * 修改日期 ：2020年03月27日 
******************************************************************************************/
int zh_get_netInfo(Enum_netName netName, Struct_netCfg *netCfg)
{
    char strNetname[10];
    switch (netName)
    {
    case BR_LAN:
        sprintf(strNetname, "br-lan");
        break;
    case ETH0:
        sprintf(strNetname, "eth0");
        break;
    case ETH1:
        sprintf(strNetname, "eth1");
        break;
    case ETH2:
        sprintf(strNetname, "ppp0");
        break;
    case WLAN0:
        sprintf(strNetname, "wlan0");
        break;
    case PPP0:
        sprintf(strNetname, "ppp0");
        break;
    default:
    {
        netCfg->error_code = 3001;
        return RES_ERROR;
    }
    }
    if (get_net_info(strNetname, netCfg) != RES_OK)
    {
        netCfg->error_code = 3002;
        return RES_ERROR;
    }
    netCfg->error_code = 0;
    return RES_OK;
}

/*******************************************************************************************
 * 功能：设置网络信息
 * 参数：netName,网络接口  one value of Enum_netName.
 *              netCfg ,lan口信息结构体 one value of Struct_netCfg.
 * 返回：RES_OK 1; 错误RES_ERROR -1;
 * 作者 ：LR
 * 修改日期 ：2020年03月27日 
*******************************************************************************************/
int zh_set_netInfo(Enum_netName netName, Struct_netCfg *netCfg)
{
    char strNetname[10];
    switch (netName)
    {
    case BR_LAN:
        sprintf(strNetname, "br-lan");
        break;
    case ETH0:
        sprintf(strNetname, "eth0");
        break;
    case ETH1:
        sprintf(strNetname, "eth1");
        break;
    case WLAN0:
        sprintf(strNetname, "wlan0");
        break;
    default:
        netCfg->error_code = 3001;
        return RES_ERROR;
    }

    if (check_IP_ture(netCfg->inet_addr) != RES_OK || check_IP_ture(netCfg->mask) != RES_OK)
    {
        printf("zh_set_netInfo check error: ifconfig netaddr %s netmask %s\n", netCfg->inet_addr, netCfg->mask);
        netCfg->error_code = 3003;
        return RES_ERROR;
    }
    else
    {
        char strtemp[255];
        sprintf(strtemp, "sudo ifconfig %s %s netmask %s", strNetname, netCfg->inet_addr, netCfg->mask);
        system(strtemp);
    }

    return RES_OK;
}

/**
 * @brief :  4G拨号
 * @param {char} *apn
 * @param {char} *user
 * @param {char} *password
 * @return {*}
 * @author: LR
 * @Date: 2021-10-29 14:40:00
 */
int zh_dataCall_init(char *apn, char *user, char *password)
{
    char cmd_call[256];
    char cmd_anwser[256];
    char config_linebuf[512];
    int res = RES_ERROR;

    if (apn == NULL || strlen(apn) == 0)
    {
        sprintf(cmd_call, "sudo ./quectel-pppd.sh");
    }
    else if (user == NULL || strlen(user) == 0)
    {
        sprintf(cmd_call, "sudo ./quectel-pppd.sh %s", apn);
    }
    else if (password == NULL || strlen(password) == 0)
    {
        sprintf(cmd_call, "sudo ./quectel-pppd.sh %s %s", apn, user);
    }
    else
    {
        sprintf(cmd_call, "sudo ./quectel-pppd.sh %s %s %s", apn, user, password);
    }
    //sprintf(cmd_call, "nohup unbuffer quectel-CM > /run/quectel.log  &");
    system(cmd_call);
    sprintf(cmd_anwser, "local  IP address");
    sleep(3);

    FILE *f;
    f = fopen("./quectel.log", "r");
    if (f == NULL)
    {
        return RES_ERROR;
    }

    fseek(f, 0, SEEK_SET);                        //定位文件指针到文件开始位置
    while (fgets(config_linebuf, 512, f) != NULL) //按行获取
    {
        //printf("quectel-log：%s", config_linebuf);
        if (strstr(config_linebuf, cmd_anwser) != NULL || strstr(config_linebuf, "Device ttyUSB3 is locked") != NULL)
        {
            res = RES_OK;
            break;
        }
        if (fgetc(f) == EOF) //文件末尾
        {
            break;
        }
        fseek(f, -1, SEEK_CUR); //定位文件指针为当前位置向下移动一行
    }
    fclose(f);

    if (res == RES_OK)
    {
        Struct_netCfg netCfg;
        if (zh_get_netInfo(PPP0, &netCfg) == RES_OK)
        {
            printf("4G info [addr:%s  Mask:%s  ]\n", netCfg.inet_addr, netCfg.mask);
            return RES_OK;
        }
    }
    return RES_ERROR;
}
/*******************************************************************************************
 * 功能：4G 撤销拨号
 * 参数：void
 * 返回：成功RES_OK ; 错误RES_ERROR;
 * 作者 ：LR
 * 修改日期 ：2020年12月8日 
*******************************************************************************************/
int zh_dataCall_deInit(void)
{
    //char cmd_call[50];

    // sprintf(cmd_call, "sudo ifconfig ppp0 down");
    // system(cmd_call);

    kill_process_by_name("pppd");

    return RES_OK;
}

#define WIFI_POW_DEVPATH "/sys/class/gpio/gpio137/value"
#define WIFI_AP_CONF_DEVPATH "/etc/wifi_hostapd.conf"
#define WIFI_STA_CONF_DEVPATH "/etc/wpa_supplicant.conf"
#define DHCPD_DEVPATH "/etc/udhcpd.conf"

/*******************************************************************************************
 * 功能：检查字符串组成（字母，数字 ，'_'，'-'，'@'，'#' ）
 * 参数：*str,  
 * 返回：RES_OK 1; RES_ERROR -1;
 *作者 ：LR
 *修改日期 ：2020年03月27日 
*******************************************************************************************/
int zh_string_check(char *str)
{
    if (str == NULL)
        return RES_ERROR;

    char *p;
    p = str;
    while (*p != '\0')
    {
        if (((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z')) || (*p >= '0' && *p <= '9') || (*p == '_') || (*p == '-') || (*p == '@') || (*p == '#'))
        {
            p++;
        }
        else
        {
            return RES_ERROR;
        }
    }
    return RES_OK;
}

/*******************************************************************************************
*功能描述 ：从配置文件中读取相应的值   并且排除了空行，“=”前后无内容，无“=”的情况 
*输入参数：1，配置文件路径 2，匹配标记 ,返回读取的变量 
*返回值 ： 成功返回查找到值，失败返回NULL
*作者 ：LR
*修改日期 ：2019年03月29日
*******************************************************************************************/
char *ReadwifiConfig(char *conf_path, char *conf_name)
{
    static char config_buff[256];

    char strtemp[100];
    int alter_sign = 0;
    char config_linebuf[256];
    char line_name[64];
    char exchange_buf[256];
    char *config_sign = "=";
    char *leave_line;
    FILE *f;

    f = fopen(conf_path, "r");
    if (f == NULL)
    {
        return NULL;
    }

    fseek(f, 0, SEEK_SET); //定位文件指针到文件开始位置

    while (fgets(config_linebuf, 256, f) != NULL) //按行获取
    {
        if (config_linebuf[0] == '#') //判断是不是注释#
        {
            continue;
        }

        if (strlen(config_linebuf) < 3) //判断是否是空行
        {
            continue;
        }

        if (config_linebuf[strlen(config_linebuf) - 1] == 10) //去除最后一位是\n的情况
        {

            memset(exchange_buf, 0, sizeof(exchange_buf));
            strncpy(exchange_buf, config_linebuf, strlen(config_linebuf) - 1);
            memset(config_linebuf, 0, sizeof(config_linebuf));
            strcpy(config_linebuf, exchange_buf);
        }

        memset(line_name, 0, sizeof(line_name));
        leave_line = strstr(config_linebuf, config_sign);
        if (leave_line == NULL) //去除无"="的情况
        {
            continue;
        }

        int leave_num = leave_line - config_linebuf;

        strncpy(line_name, config_linebuf, leave_num);

        if (strcmp(line_name, conf_name) == 0) //匹配成功
        {
            strncpy(config_buff, config_linebuf + (leave_num + 1), strlen(config_linebuf) - 1);
            alter_sign = 1;
            break;
        }

        if (fgetc(f) == EOF) //文件末尾
        {
            break;
        }

        fseek(f, -1, SEEK_CUR); //定位文件指针为当前位置向下移动一行
        memset(config_linebuf, 0, sizeof(config_linebuf));
    }

    fclose(f);

    if (alter_sign == 0) //如果没有找到
    {
        return NULL;
    }

    return config_buff;
}

/*******************************************************************************************
*功能描述 ：从配置文件中读取相应的值   并且排除了空行，“  ”前后无内容，无“  ”的情况 
*输入参数：1，配置文件路径 2，匹配标记 ,返回读取的变量 
*返回值 ： 成功返回查找到值，失败返回NULL
*作者 ：LR
*修改日期 ：2019年03月29日
*******************************************************************************************/
char *ReadDhcpConfig(char *conf_path, char *conf_name)
{
    static char config_buff[256];

    char strtemp[100];
    int alter_sign = 0;
    char config_linebuf[256];
    char line_name[64];
    char exchange_buf[256];
    char *config_sign = " ";
    char *leave_line;
    FILE *f;

    f = fopen(conf_path, "r");
    if (f == NULL)
    {
        return NULL;
    }

    fseek(f, 0, SEEK_SET); //定位文件指针到文件开始位置

    while (fgets(config_linebuf, 256, f) != NULL) //按行获取
    {
        if (config_linebuf[0] == '#') //判断是不是注释#
        {
            continue;
        }

        if (strlen(config_linebuf) < 3) //判断是否是空行
        {
            continue;
        }

        if (config_linebuf[strlen(config_linebuf) - 1] == 10) //去除最后一位是\n的情况
        {
            memset(exchange_buf, 0, sizeof(exchange_buf));
            strncpy(exchange_buf, config_linebuf, strlen(config_linebuf) - 1);
            memset(config_linebuf, 0, sizeof(config_linebuf));
            strcpy(config_linebuf, exchange_buf);
        }

        memset(line_name, 0, sizeof(line_name));
        leave_line = strstr(config_linebuf, config_sign);
        if (leave_line == NULL) //去除无" "的情况
        {
            continue;
        }

        int leave_num = leave_line - config_linebuf;
        strncpy(line_name, config_linebuf, leave_num);
        if (strcmp(line_name, conf_name) == 0) //匹配成功
        {
            //strncpy(config_buff, config_linebuf + (leave_num + 1), strlen(config_linebuf) - 1);
            alter_sign = 1;
            break;
        }

        if (fgetc(f) == EOF) //文件末尾
        {
            break;
        }

        fseek(f, -1, SEEK_CUR); //定位文件指针为当前位置向下移动一行
        memset(config_linebuf, 0, sizeof(config_linebuf));
    }

    fclose(f);

    if (alter_sign == 0) //如果没有找到
    {
        return NULL;
    }
    else
    {
        int i = 0;
        while (isdigit(config_linebuf[i]) == 0 && config_linebuf[i] != '\0')
        {
            i++;
        }
        if (config_linebuf[i] == '\0' || strlen(config_linebuf) == i + 1)
        {
            return NULL;
        }
        strncpy(config_buff, config_linebuf + i, strlen(config_linebuf + i));

        return config_buff;
    }
}

/***************************************************
 * 功能：wifi 配置获取
 * 参数：Enum_wifiMode ；Struct_wifiCfg；
 * 返回：RES_OK 1; RES_ERROR -1;
 *作者 ：LR
 *修改日期 ：2020年12月8日 
***************************************************/
int zh_get_wifiCfg(Enum_wifiMode wifimode, Struct_wifiCfg *wificfg)
{
    if (wificfg == NULL)
        return RES_ERROR;

    char temp[32];

    if (wifimode == WIFI_AP)
    {
        char *ssidStr;
        if ((ssidStr = ReadwifiConfig(WIFI_AP_CONF_DEVPATH, "ssid")) == NULL)
        {
            wificfg->error_code = 5003;
            return RES_ERROR;
        }
        sprintf(wificfg->ssid, "%s", ssidStr);

        char *passStr;
        if ((passStr = ReadwifiConfig(WIFI_AP_CONF_DEVPATH, "wpa_passphrase")) == NULL)
        {
            wificfg->error_code = 5004;
            return RES_ERROR;
        }
        sprintf(wificfg->password, "%s", passStr);

        return RES_OK;
    }
    else if (wifimode == WIFI_STA)
    {
        char *ssidStr;
        if ((ssidStr = ReadwifiConfig(WIFI_STA_CONF_DEVPATH, "ssid")) == NULL)
        {
            wificfg->error_code = 5003;
            return RES_ERROR;
        }
        if (strlen(ssidStr) <= 2)
        {
            sprintf(wificfg->ssid, "");
        }
        else
        {
            memset(temp, 0, sizeof(temp));
            memcpy(temp, ssidStr + 1, strlen(ssidStr) - 2);
            sprintf(wificfg->ssid, "%s", temp);
        }

        char *passStr;
        if ((passStr = ReadwifiConfig(WIFI_STA_CONF_DEVPATH, "psk")) == NULL)
        {
            wificfg->error_code = 5004;
            return RES_ERROR;
        }
        if (strlen(passStr) <= 2)
        {
            sprintf(wificfg->ssid, "");
        }
        else
        {
            memset(temp, 0, sizeof(temp));
            memcpy(temp, passStr + 1, strlen(passStr) - 2);
            sprintf(wificfg->password, "%s", temp);
        }
        return RES_OK;
    }
    else
    {
        wificfg->error_code = 5001;
    }
    return RES_ERROR;
}

/***************************************************
 * 功能：wifi 配置设置
 * 参数：Enum_wifiMode,Struct_wifiCfg
 * 返回：RES_OK 1; RES_ERROR -1;
 *作者 ：LR
 *修改日期 ：2020年12月8日 
***************************************************/
int zh_set_wifiCfg(Enum_wifiMode wifimode, Struct_wifiCfg *wificfg)
{
    if (wificfg == NULL)
        return RES_ERROR;

    if (strlen(wificfg->ssid) < 1)
    {
        wificfg->error_code = 5006;
        return RES_ERROR;
    }

    if (zh_string_check(wificfg->ssid) != RES_OK || zh_string_check(wificfg->password) != RES_OK)
    {
        wificfg->error_code = 5006;
        return RES_ERROR;
    }

    char databuff[512];

    if (wifimode == WIFI_AP)
    {
        FILE *wificonf_fd;
        wificonf_fd = fopen(WIFI_AP_CONF_DEVPATH, "w+"); //
        if (wificonf_fd == NULL)
        {
            wificfg->error_code = 5005;
            return RES_ERROR;
        }
        sprintf(databuff, "interface=wlan0\nssid=%s\ndriver=nl80211\nchannel=11\nhw_mode=g\nignore_broadcast_ssid=0\nauth_algs=1\nwpa=3\nwpa_passphrase=%s\nwpa_key_mgmt=WPA-PSK\nwpa_pairwise=TKIP\nrsn_pairwise=CCMP", wificfg->ssid, wificfg->password);
        fseek(wificonf_fd, 0, SEEK_SET); //定位到文件开始
        fputs(databuff, wificonf_fd);    //写入文件
        fflush(wificonf_fd);             //刷新内存缓冲区 将内容写入内核缓冲
        fsync(fileno(wificonf_fd));      //将缓存区数据写入磁盘，并等待操作结束
        fclose(wificonf_fd);
    }
    else if (wifimode == WIFI_STA)
    {
        FILE *wificonf_fd;
        wificonf_fd = fopen(WIFI_STA_CONF_DEVPATH, "w+"); //
        if (wificonf_fd == NULL)
        {
            wificfg->error_code = 5005;
            return RES_ERROR;
        }
        sprintf(databuff, "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\nupdate_config=1\nnetwork={\nssid=\"%s\"\npsk=\"%s\"\n}\n", wificfg->ssid, wificfg->password);
        fseek(wificonf_fd, 0, SEEK_SET); //定位到文件开始
        fputs(databuff, wificonf_fd);    //写入文件
        fflush(wificonf_fd);             //刷新内存缓冲区 将内容写入内核缓冲
        fsync(fileno(wificonf_fd));      //将缓存区数据写入磁盘，并等待操作结束
        fclose(wificonf_fd);
    }
    else
    {
        return RES_ERROR;
    }
    return RES_OK;
}

/***************************************************
 * 功能：使能 wifi AP
 * 参数：wifimode: wifi工作模式
 * 返回：RES_OK 1; RES_ERROR -1;
 *作者 ：LR
 *修改日期 ：2020年12月8日 
***************************************************/
int zh_wifi_enable(Enum_wifiMode wifimode)
{
    if (wifimode == WIFI_AP)
    {
        char cmd_call[256];
        char cmd_anwser[256];
        char config_linebuf[512];
        int res = RES_ERROR;

        system("sudo ifconfig wlan0 up");

        sprintf(cmd_call, "nohup unbuffer hostapd -B /etc/wifi_hostapd.conf  > /run/wifiap.log  2>&1 &");
        sprintf(cmd_anwser, "AP-ENABLED");
        system(cmd_call);
        sleep(3);

        FILE *f;
        f = fopen("/run/wifiap.log", "r");
        if (f == NULL)
        {
            return RES_ERROR;
        }
        fseek(f, 0, SEEK_SET);                        //定位文件指针到文件开始位置
        while (fgets(config_linebuf, 512, f) != NULL) //按行获取
        {
            printf("wifi-log：%s", config_linebuf);
            if (strstr(config_linebuf, cmd_anwser) != NULL)
            {
                res = RES_OK;
                system("brctl addif br-lan wlan0");
                break;
            }
            if (fgetc(f) == EOF) //文件末尾
            {
                break;
            }
            fseek(f, -1, SEEK_CUR); //定位文件指针为当前位置向下移动一行
        }
        fclose(f);
        return res;
    }
    else if (wifimode == WIFI_STA)
    {
        char cmd_call[256];
        char cmd_anwser[256];
        char config_linebuf[512];
        int res = RES_ERROR;

        sprintf(cmd_call, "ifconfig wlan0 up");
        sprintf(cmd_anwser, "");
        system(cmd_call);

        //可用wpa_cli -i wlan0 status可以查看网络连接状态
        sprintf(cmd_call, "nohup unbuffer wpa_supplicant -Dnl80211 -c /etc/wpa_supplicant.conf -i wlan0 > /run/wifista.log  &");
        sprintf(cmd_anwser, "CTRL-EVENT-CONNECTED");
        system(cmd_call);
        sleep(5);

        FILE *f;
        f = fopen("/run/wifista.log", "r");
        if (f == NULL)
        {
            return RES_ERROR;
        }
        fseek(f, 0, SEEK_SET);                        //定位文件指针到文件开始位置
        while (fgets(config_linebuf, 512, f) != NULL) //按行获取
        {
            printf("log：%s", config_linebuf);
            if (strstr(config_linebuf, cmd_anwser) != NULL)
            {
                res = RES_OK;
                break;
            }
            if (fgetc(f) == EOF) //文件末尾
            {
                break;
            }
            fseek(f, -1, SEEK_CUR); //定位文件指针为当前位置向下移动一行
        }
        fclose(f);

        if (res != RES_OK)
        {
            return RES_ERROR;
        }

        sprintf(cmd_call, "nohup unbuffer udhcpc -i wlan0 > /run/wifista.log  &");
        sprintf(cmd_anwser, "select for");
        system(cmd_call);
        sleep(1);

        // f = fopen("/run/wifista.log", "r");
        // if (f == NULL)
        // {
        //     return RES_ERROR;
        // }
        // fseek(f, 0, SEEK_SET);                        //定位文件指针到文件开始位置
        // while (fgets(config_linebuf, 512, f) != NULL) //按行获取
        // {
        //     if (fgetc(f) == EOF) //文件末尾
        //     {
        //         break;
        //     }
        //     if (strstr(config_linebuf, cmd_anwser) != NULL)
        //     {
        //         res = RES_OK;
        //         break;
        //     }
        // }
        // fclose(f);

        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }
}

/***************************************************
 * 功能：失能 wifi
 * 参数：wifimode: wifi工作模式
 * 返回：RES_OK 1; RES_ERROR -1;
 *作者 ：LR
 *修改日期 ：2020年12月8日 
***************************************************/
int zh_wifi_disEnable(Enum_wifiMode wifimode)
{
    if (wifimode == WIFI_AP)
    {
        kill_process_by_name("hostapd");
        usleep(1000);

        char cmd_call[50];
        char cmd_anwser[50];

        sprintf(cmd_call, "ifconfig wlan0 down");
        sprintf(cmd_anwser, "");
        system(cmd_call);

        return RES_OK;
    }
    else if (wifimode == WIFI_STA)
    {
        kill_process_by_name("wpa_supplicant"); //杀死进程：tclsh /usr/bin/unbuffer wpa_supplicant -Dnl80211 -c /etc/wpa_supplicant.conf -i wlan0
        kill_process_by_name("wpa_supplicant"); //杀死进程：wpa_supplicant -Dnl80211 -c /etc/wpa_supplicant.conf -i wlan0
        kill_process_by_name("udhcpc");
        usleep(1000);

        char cmd_call[50];
        char cmd_anwser[50];

        sprintf(cmd_call, "ifconfig wlan0 down");
        sprintf(cmd_anwser, "");
        system(cmd_call);

        sprintf(cmd_call, "rm -rf  /var/run/wpa_supplicant/wlan0");
        sprintf(cmd_anwser, "");
        system(cmd_call);

        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }
}

/***************************************************
 * 功能：dhcp 服务器配置获取
 * 参数：Struct_dhcpCfg
 * 返回：RES_OK 1; RES_ERROR -1;
 *作者 ：LR
 *修改日期 ：2020年12月8日 
***************************************************/
int zh_get_dhcpCfg(Struct_dhcpCfg *dhcpcfg)
{
    char *interfaceStr;
    if ((interfaceStr = ReadDhcpConfig(DHCPD_DEVPATH, "interface")) == NULL)
    {
        dhcpcfg->error_code = 5010;
        return RES_ERROR;
    }
    if (strstr(interfaceStr, "eth0") != NULL)
    {
        dhcpcfg->interface = ETH0;
    }
    else if (strstr(interfaceStr, "eth1") != NULL)
    {
        dhcpcfg->interface = ETH1;
    }
    else if (strstr(interfaceStr, "ppp0") != NULL)
    {
        dhcpcfg->interface = ETH2;
    }
    else if (strstr(interfaceStr, "wlan0") != NULL)
    {
        dhcpcfg->interface = WLAN0;
    }
    else if (strstr(interfaceStr, "br-lan") != NULL)
    {
        dhcpcfg->interface = BR_LAN;
    }
    else
    {
        dhcpcfg->interface = UNKNOWN;
    }

    char *startStr;
    if ((startStr = ReadDhcpConfig(DHCPD_DEVPATH, "start")) == NULL)
    {
        dhcpcfg->error_code = 5011;
        return RES_ERROR;
    }
    sprintf(dhcpcfg->dhcp_addr_start, "%s", startStr);

    char *endStr;
    if ((endStr = ReadDhcpConfig(DHCPD_DEVPATH, "end")) == NULL)
    {
        dhcpcfg->error_code = 5012;
        return RES_ERROR;
    }
    sprintf(dhcpcfg->dhcp_addr_end, "%s", endStr);

    char *routerStr;
    if ((routerStr = ReadDhcpConfig(DHCPD_DEVPATH, "opt router")) == NULL)
    {
        dhcpcfg->error_code = 5013;
        return RES_ERROR;
    }
    sprintf(dhcpcfg->router, "%s", routerStr);

    char *dnsStr;
    if ((dnsStr = ReadDhcpConfig(DHCPD_DEVPATH, "option dns")) == NULL)
    {
        dhcpcfg->error_code = 5014;
        return RES_ERROR;
    }
    sprintf(dhcpcfg->dns, "%s", dnsStr);

    return RES_OK;
}

/***************************************************
 * 功能：dhcp 服务器配置设置
 * 参数：Struct_dhcpCfg
 * 返回：RES_OK 1; RES_ERROR -1;
 *作者 ：LR
 *修改日期 ：2020年12月8日 
***************************************************/
int zh_set_dhcpCfg(Struct_dhcpCfg *dhcpcfg)
{
    if (dhcpcfg == NULL)
        return RES_ERROR;

    char dhcp_interface[10];
    if (dhcpcfg->interface == ETH1)
    {
        sprintf(dhcp_interface, "eth1");
    }
    else if (dhcpcfg->interface == BR_LAN)
    {
        sprintf(dhcp_interface, "br-lan");
    }
    else
    {
        dhcpcfg->error_code = 5016;
        return RES_ERROR;
    }

    if (check_IP_ture(dhcpcfg->dhcp_addr_start) != RES_OK || check_IP_ture(dhcpcfg->dhcp_addr_end) != RES_OK || check_IP_ture(dhcpcfg->router) != RES_OK || check_IP_ture(dhcpcfg->dns) != RES_OK)
    {
        dhcpcfg->error_code = 5017;
        return RES_ERROR;
    }

    FILE *dhcpconf_fd;
    dhcpconf_fd = fopen(DHCPD_DEVPATH, "w+");
    if (dhcpconf_fd == NULL)
    {
        dhcpcfg->error_code = 5018;
        return RES_ERROR;
    }

    char dhcpbuff[512];
    sprintf(dhcpbuff, "start  %s\nend  %s\ninterface  %s\nopt dns 8.8.8.8\noption subnet 255.255.255.0\nopt router %s\noption dns %s\noption domain local\noption lease 864000", dhcpcfg->dhcp_addr_start, dhcpcfg->dhcp_addr_end, dhcp_interface, dhcpcfg->router, dhcpcfg->dns);
    fseek(dhcpconf_fd, 0, SEEK_SET); //定位到文件开始
    fputs(dhcpbuff, dhcpconf_fd);    //写入文件
    fflush(dhcpconf_fd);             //刷新内存缓冲区 将内容写入内核缓冲
    fsync(fileno(dhcpconf_fd));      //将缓存区数据写入磁盘，并等待操作结束
    fclose(dhcpconf_fd);

    return RES_OK;
}

/**
 * @brief :  
 * @param 
 * @return {*}
 * @author: LR
 * @Date: 2021-12-01 14:52:14
 */
int zh_dhcpS_enable(void)
{
    char cmd_call[256];
    sprintf(cmd_call, "sudo udhcpd -fS  /etc/udhcpd.conf 2>&1 &");
    system(cmd_call);

    return RES_OK;
}

/**
 * @brief :  
 * @param 
 * @return {*}
 * @author: LR
 * @Date: 2021-12-01 14:52:14
 */
int zh_dhcpS_disable(void)
{
    kill_process_by_name("udhcpd");
    return RES_OK;
}

/**
  * @brief :  查看网口是否连接网线
  * @param {Enum_netName} netName
  * @return {*}
  * @author: LR
  * @Date: 2021-12-04 16:19:14
  */
int zh_get_ethLink(Enum_netName netName)
{
    char ethPath[50];
    switch (netName)
    {
    case ETH0:
        sprintf(ethPath, "/sys/class/net/eth0/carrier");
        break;
    case ETH1:
        sprintf(ethPath, "/sys/class/net/eth1/carrier");
        break;
    default:
        return RES_ERROR;
    }

    FILE *eth_fd;
    eth_fd = fopen(ethPath, "r"); //按键
    if (eth_fd == NULL)
        return RES_ERROR;

    char eth_link[5];
    if (fgets(eth_link, 5, eth_fd) == NULL)
    {
        fclose(eth_fd);
        return RES_ERROR;
    }
    fclose(eth_fd);

    if (atoi(eth_link) == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}