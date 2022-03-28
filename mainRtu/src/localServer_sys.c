/*
 * @Description :  
 * @FilePath: /app_code/mainRtu/src/localServer_sys.c
 * @Author:  LR
 * @Date: 2021-10-23 14:36:24
 */

#include "baseInit.h"
#include "zh_rtc.h"
#include "cJSON.h"
#include "zh_network.h"
#include "rtuIfsHandle.h"
#include "localServer_sys.h"

localServer_sys_info_T localServer_sys_info;
pthread_mutex_t SYS_Server_Mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t SYS_send_Mutex = PTHREAD_MUTEX_INITIALIZER;

/***************************************************
 * 功能：初始化本地DEBUG服务
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日
*************************************************/
void init_localServer_sys_service(void)
{
    memset(&localServer_sys_info, 0, sizeof(localServer_sys_info_T));
}

/**
 * @brief :  th交互json解析
 * @param {int} *socketFd
 * @param {uint8_t} *recvData
 * @param {int} recvLen
 * @return {*}
 * @author: LR
 * @Date: 2021-09-28 15:20:09
 */
int localServer_sys_dataProc(int socketFd, char *recvData, int recvLen)
{
    if (recvLen < 5)
        return RES_ERROR;

    char _msgType[128] = {0}; // 纪录msgType
    char _tempBuff1[128] = {0};

    // 用于获取来自TCP的JSON结构
    cJSON *pR1 = NULL;
    cJSON *pR2 = NULL;
    cJSON *pR3 = NULL;
    cJSON *pR4 = NULL;
    char revPDU[SYS_MAX_BUFF] = {0};
    if (recvLen > SYS_MAX_BUFF)
    {
        recvLen = SYS_MAX_BUFF;
    }
    memcpy(revPDU, recvData, recvLen);

    ExcepSign Ex; // 用于实现异常处理的异常标记

    pR1 = cJSON_Parse(revPDU); // json解析，获得句柄
    {
        if (pR1 == NULL)
        {
            //log_i("json_proc -cJSON_Parse devcfgfile error\n");
            return RES_ERROR;
        }
        pR2 = cJSON_GetObjectItem(pR1, "msgType"); // 获取msgType
        {
            if (pR2 == NULL || pR2->valuestring == NULL)
            {
                cJSON_Delete(pR1);
                return RES_ERROR;
            }
            snprintf(_msgType, sizeof(_msgType), "%s", pR2->valuestring); //提取msgType
        }

        if (strncmp(_msgType, "get", 3) == 0)
        {
            char *p;
            cJSON *pJsonRoot;
            cJSON *pSubJson;

            pJsonRoot = cJSON_CreateObject();
            if (NULL == pJsonRoot)
            {
                cJSON_Delete(pR1);
                return RES_ERROR;
            }
            snprintf(_tempBuff1, sizeof(_tempBuff1), "%sAck", _msgType);
            cJSON_AddStringToObject(pJsonRoot, "msgType", _tempBuff1);
            cJSON_AddItemToObject(pJsonRoot, "data", pSubJson = cJSON_CreateObject());

            if (strcmp(_msgType, "getSysBasic") == 0)
            {
                cJSON_AddStringToObject(pSubJson, "model", sysBasic.model);
                cJSON_AddStringToObject(pSubJson, "sn", sysBasic.sn);
                cJSON_AddStringToObject(pSubJson, "version", sysBasic.version);
                sysBasic.cpuUsage = zh_get_sysInfo(CPU_USAGE);
                cJSON_AddNumberToObject(pSubJson, "cpu", sysBasic.cpuUsage < 0 ? 0 : sysBasic.cpuUsage);
                sysBasic.memUsage = zh_get_sysInfo(MEM_USAGE);
                cJSON_AddNumberToObject(pSubJson, "ram", sysBasic.memUsage < 0 ? 0 : sysBasic.memUsage);
                cJSON_AddNumberToObject(pSubJson, "date", get_rtc_timestamp());
                cJSON_AddNumberToObject(pSubJson, "defaultNet", sysBasic.defaultNet);

                p = cJSON_PrintUnformatted(pJsonRoot);
                if (p != NULL)
                {
                    send_localServer_sys_data(socketFd, (uint8_t *)p, strlen(p));
                    free(p);
                    cJSON_Delete(pJsonRoot);
                }
                else
                {
                    cJSON_Delete(pJsonRoot);
                    cJSON_Delete(pR1);
                    return RES_ERROR;
                }
            }
            else if (strcmp(_msgType, "getLTEBasic") == 0)
            {
                Struct_netCfg netCfg;
                if (zh_get_netInfo(PPP0, &netCfg) == RES_OK)
                {
                    snprintf(lteBasic.ip, sizeof(lteBasic.ip), "%s", netCfg.inet_addr);
                    snprintf(lteBasic.mask, sizeof(lteBasic.mask), "%s", netCfg.mask);
                    log_i("4G info [addr:%s  Mask:%s  ]\n", lteBasic.ip, lteBasic.mask);
                }
                cJSON_AddStringToObject(pSubJson, "ip", lteBasic.ip);
                cJSON_AddStringToObject(pSubJson, "mask", lteBasic.mask);
                cJSON_AddStringToObject(pSubJson, "gateway", lteBasic.gateway);
                char *temp;
                if ((temp = get_at_ccid()) != NULL)
                {
                    snprintf(lteBasic.ccid, sizeof(lteBasic.ccid), "%s", temp); // 获取4gID
                }
                cJSON_AddStringToObject(pSubJson, "ccid", lteBasic.ccid);
                lteBasic.signal = get_at_csq();
                cJSON_AddNumberToObject(pSubJson, "signal", lteBasic.signal);
                p = cJSON_PrintUnformatted(pJsonRoot);
                if (p != NULL)
                {
                    send_localServer_sys_data(socketFd, (uint8_t *)p, strlen(p));
                    free(p);
                    cJSON_Delete(pJsonRoot);
                }
                else
                {
                    cJSON_Delete(pJsonRoot);
                    cJSON_Delete(pR1);
                    return RES_ERROR;
                }
            }
            else if (strcmp(_msgType, "getLTEAPN") == 0)
            {
                cJSON_AddStringToObject(pSubJson, "addr", lteBasic.lteAPN.addr);
                cJSON_AddStringToObject(pSubJson, "username", lteBasic.lteAPN.username);
                cJSON_AddStringToObject(pSubJson, "password", lteBasic.lteAPN.password);

                p = cJSON_PrintUnformatted(pJsonRoot);
                if (p != NULL)
                {
                    send_localServer_sys_data(socketFd, (uint8_t *)p, strlen(p));
                    free(p);
                    cJSON_Delete(pJsonRoot);
                }
                else
                {
                    cJSON_Delete(pJsonRoot);
                    cJSON_Delete(pR1);
                    return RES_ERROR;
                }
            }
            else if (strcmp(_msgType, "getWIFIBasic") == 0)
            {

                cJSON_AddNumberToObject(pSubJson, "enable", wifiBasic.enable);
                cJSON_AddStringToObject(pSubJson, "ssid", wifiBasic.ssid);
                cJSON_AddStringToObject(pSubJson, "password", wifiBasic.password);

                p = cJSON_PrintUnformatted(pJsonRoot);
                if (p != NULL)
                {
                    send_localServer_sys_data(socketFd, (uint8_t *)p, strlen(p));
                    free(p);
                    cJSON_Delete(pJsonRoot);
                }
                else
                {
                    cJSON_Delete(pJsonRoot);
                    cJSON_Delete(pR1);
                    return RES_ERROR;
                }
            }
            else if (strcmp(_msgType, "getLANBasic") == 0)
            {

                cJSON_AddStringToObject(pSubJson, "ip", lanBasic.ip);
                cJSON_AddStringToObject(pSubJson, "mask", lanBasic.mask);
                cJSON_AddStringToObject(pSubJson, "gateway", lanBasic.gateway);
                cJSON_AddNumberToObject(pSubJson, "dhcpEnable", lanBasic.dhcpsBasic.enable);
                cJSON_AddStringToObject(pSubJson, "dhcpStart", lanBasic.dhcpsBasic.startIp);
                cJSON_AddStringToObject(pSubJson, "dhcpEnd", lanBasic.dhcpsBasic.endIp);

                p = cJSON_PrintUnformatted(pJsonRoot);
                if (p != NULL)
                {
                    send_localServer_sys_data(socketFd, (uint8_t *)p, strlen(p));
                    free(p);
                    cJSON_Delete(pJsonRoot);
                }
                else
                {
                    cJSON_Delete(pJsonRoot);
                    cJSON_Delete(pR1);
                    return RES_ERROR;
                }
            }
            else if (strcmp(_msgType, "getWANBasic") == 0)
            {

                cJSON_AddNumberToObject(pSubJson, "ipMode", wanBasic.ipMode);
                cJSON_AddStringToObject(pSubJson, "ip", wanBasic.ip);
                cJSON_AddStringToObject(pSubJson, "mask", wanBasic.mask);
                cJSON_AddStringToObject(pSubJson, "gateway", wanBasic.gateway);

                p = cJSON_PrintUnformatted(pJsonRoot);
                if (p != NULL)
                {
                    send_localServer_sys_data(socketFd, (uint8_t *)p, strlen(p));
                    free(p);
                    cJSON_Delete(pJsonRoot);
                }
                else
                {
                    cJSON_Delete(pJsonRoot);
                    cJSON_Delete(pR1);
                    return RES_ERROR;
                }
            }
            else if (strcmp(_msgType, "getUserLogin") == 0)
            {
                cJSON_AddStringToObject(pSubJson, "password", sysBasic.userPassword);
                p = cJSON_PrintUnformatted(pJsonRoot);
                if (p != NULL)
                {
                    send_localServer_sys_data(socketFd, (uint8_t *)p, strlen(p));
                    free(p);
                    cJSON_Delete(pJsonRoot);
                }
                else
                {
                    cJSON_Delete(pJsonRoot);
                    cJSON_Delete(pR1);
                    return RES_ERROR;
                }
            }
            else if (strcmp(_msgType, "getSysTH") == 0)
            {
                // Struct_sysTh sysTempHumi;
                // if (zh_get_sysTH(&sysTempHumi) != RES_OK)
                // {
                //     cJSON_Delete(pJsonRoot);
                //     cJSON_Delete(pR1);
                //     return RES_ERROR;
                // }
                // cJSON_AddNumberToObject(pSubJson, "temp", sysTempHumi.temp);
                // cJSON_AddNumberToObject(pSubJson, "humi", sysTempHumi.humi);

                // p = cJSON_PrintUnformatted(pJsonRoot);
                // if (p != NULL)
                // {
                //     send_localServer_sys_data(socketFd, (uint8_t *)p, strlen(p));
                //     free(p);
                //     cJSON_Delete(pJsonRoot);
                // }
                // else
                // {
                //     cJSON_Delete(pJsonRoot);
                //     cJSON_Delete(pR1);
                //     return RES_ERROR;
                // }
            }
            
            else
            {
                cJSON_Delete(pR1);
                return RES_ERROR;
            }
        }
        else if (strncmp(_msgType, "set", 3) == 0)
        {
            int status = 0;
            Try(Ex)
            {
                if ((pR2 = cJSON_GetObjectItem(pR1, "data")) == NULL)
                {
                    Throw(Ex, RES_ERROR);
                }
                if (strcmp(_msgType, "setSysBasicDate") == 0)
                {
                    if ((pR3 = cJSON_GetObjectItem(pR2, "date")) != NULL)
                    {
                        if (pR3->type != cJSON_Number)
                        {
                            Throw(Ex, RES_ERROR);
                        }
                        if (set_rtc_timestamp(pR3->valueint) == RES_ERROR)
                        {
                            Throw(Ex, RES_ERROR);
                        }
                    }
                }
                else if (strcmp(_msgType, "setSysBasicNet") == 0)
                {
                    if ((pR3 = cJSON_GetObjectItem(pR2, "defaultNet")) != NULL)
                    {
                        if (pR3->type != cJSON_Number)
                        {
                            Throw(Ex, RES_ERROR);
                        }
                        if (pR3->valueint != NET_4G && pR3->valueint != NET_WAN)
                        {
                            Throw(Ex, RES_ERROR);
                        }
                        sysBasic.defaultNet = pR3->valueint;
                        set_dev_cfg();
                    }
                }
                else if (strcmp(_msgType, "setLTEAPN") == 0)
                {
                    if ((pR3 = cJSON_GetObjectItem(pR2, "addr")) != NULL)
                    {
                        snprintf(lteBasic.lteAPN.addr, sizeof(lteBasic.lteAPN.addr), "%s", pR3->valuestring);
                    }
                    if ((pR3 = cJSON_GetObjectItem(pR2, "username")) != NULL)
                    {
                        snprintf(lteBasic.lteAPN.username, sizeof(lteBasic.lteAPN.username), "%s", pR3->valuestring);
                    }
                    if ((pR3 = cJSON_GetObjectItem(pR2, "password")) != NULL)
                    {
                        snprintf(lteBasic.lteAPN.password, sizeof(lteBasic.lteAPN.password), "%s", pR3->valuestring);
                    }
                    set_dev_cfg();
                }
                else if (strcmp(_msgType, "setLTEAct") == 0)
                {
                    if ((pR3 = cJSON_GetObjectItem(pR2, "act")) != NULL)
                    {
                        if (pR3->valueint != ENABLE)
                        {
                            Throw(Ex, RES_ERROR);
                        }
                        if (zh_dataCall_init(lteBasic.lteAPN.addr, lteBasic.lteAPN.username, lteBasic.lteAPN.password) != RES_OK)
                        {
                            Throw(Ex, RES_ERROR);
                        }
                    }
                }
                else if (strcmp(_msgType, "setWIFIBasic") == 0)
                {
                    if ((pR3 = cJSON_GetObjectItem(pR2, "enable")) != NULL)
                    {
                        if (pR3->type == cJSON_Number)
                        {
                            int temp = pR3->valueint;
                            if (temp != ENABLE && temp != DISABLE)
                            {
                                Throw(Ex, RES_ERROR);
                            }
                            wifiBasic.enable = temp;
                        }
                    }
                    if ((pR3 = cJSON_GetObjectItem(pR2, "ssid")) != NULL)
                    {
                        if (pR3->valuestring != NULL && zh_string_check(pR3->valuestring) != RES_ERROR)
                        {
                            snprintf(wifiBasic.ssid, sizeof(wifiBasic.ssid), "%s", pR3->valuestring);
                        }
                        else
                        {
                            Throw(Ex, RES_ERROR);
                        }
                    }
                    if ((pR3 = cJSON_GetObjectItem(pR2, "password")) != NULL)
                    {
                        if (pR3->valuestring != NULL && zh_string_check(pR3->valuestring) != RES_ERROR)
                        {
                            snprintf(wifiBasic.password, sizeof(wifiBasic.password), "%s", pR3->valuestring);
                        }
                        else
                        {
                            Throw(Ex, RES_ERROR);
                        }
                    }
                    set_dev_cfg();
                }
                else if (strcmp(_msgType, "setLANBasic") == 0)
                {
                    if ((pR3 = cJSON_GetObjectItem(pR2, "ip")) != NULL)
                    {
                        if (pR3->valuestring != NULL && check_IP_ture(pR3->valuestring) != RES_ERROR)
                        {
                            snprintf(lanBasic.ip, sizeof(lanBasic.ip), "%s", pR3->valuestring);
                        }
                        else
                        {
                            Throw(Ex, RES_ERROR);
                        }
                    }
                    if ((pR3 = cJSON_GetObjectItem(pR2, "mask")) != NULL)
                    {
                        if (pR3->valuestring != NULL && check_IP_ture(pR3->valuestring) != RES_ERROR)
                        {
                            snprintf(lanBasic.mask, sizeof(lanBasic.mask), "%s", pR3->valuestring);
                        }
                        else
                        {
                            Throw(Ex, RES_ERROR);
                        }
                    }
                    if ((pR3 = cJSON_GetObjectItem(pR2, "gateway")) != NULL)
                    {
                        if (pR3->valuestring != NULL)
                        {
                            if (strlen(pR3->valuestring) != 0 && check_IP_ture(pR3->valuestring) != RES_OK)
                            {
                                Throw(Ex, RES_ERROR);
                            }
                            snprintf(lanBasic.gateway, sizeof(lanBasic.gateway), "%s", pR3->valuestring);
                        }
                        else
                        {
                            Throw(Ex, RES_ERROR);
                        }
                    }
                    if ((pR3 = cJSON_GetObjectItem(pR2, "dhcpEnable")) != NULL)
                    {
                        if (pR3->type != cJSON_Number || (pR3->valueint != ENABLE && pR3->valueint != DISABLE))
                        {
                            Throw(Ex, RES_ERROR);
                        }
                        lanBasic.dhcpsBasic.enable = pR3->valueint;
                    }
                    if ((pR3 = cJSON_GetObjectItem(pR2, "dhcpStart")) != NULL)
                    {
                        if (pR3->valuestring != NULL && check_IP_ture(pR3->valuestring) != RES_ERROR)
                        {
                            snprintf(lanBasic.dhcpsBasic.startIp, sizeof(lanBasic.dhcpsBasic.startIp), "%s", pR3->valuestring);
                        }
                        else
                        {
                            Throw(Ex, RES_ERROR);
                        }
                    }
                    if ((pR3 = cJSON_GetObjectItem(pR2, "dhcpEnd")) != NULL)
                    {
                        if (pR3->valuestring != NULL && check_IP_ture(pR3->valuestring) != RES_ERROR)
                        {
                            snprintf(lanBasic.dhcpsBasic.endIp, sizeof(lanBasic.dhcpsBasic.endIp), "%s", pR3->valuestring);
                        }
                        else
                        {
                            Throw(Ex, RES_ERROR);
                        }
                    }
                    zh_set_ethCfg();
                    set_dev_cfg();
                }
                else if (strcmp(_msgType, "setWANBasic") == 0)
                {
                    if ((pR3 = cJSON_GetObjectItem(pR2, "ipMode")) != NULL)
                    {
                        if (pR3->type != cJSON_Number || (pR3->valueint != IP_MODE_DHCPC && pR3->valueint != IP_MODE_STATIC))
                        {
                            Throw(Ex, RES_ERROR);
                        }

                        wanBasic.ipMode = pR3->valueint;
                    }
                    if ((pR3 = cJSON_GetObjectItem(pR2, "ip")) != NULL)
                    {
                        if (pR3->valuestring != NULL && check_IP_ture(pR3->valuestring) != RES_ERROR)
                        {
                            snprintf(wanBasic.ip, sizeof(wanBasic.ip), "%s", pR3->valuestring);
                        }
                        else
                        {
                            Throw(Ex, RES_ERROR);
                        }
                    }
                    if ((pR3 = cJSON_GetObjectItem(pR2, "mask")) != NULL)
                    {
                        if (pR3->valuestring != NULL && check_IP_ture(pR3->valuestring) != RES_ERROR)
                        {
                            snprintf(wanBasic.mask, sizeof(wanBasic.mask), "%s", pR3->valuestring);
                        }
                        else
                        {
                            Throw(Ex, RES_ERROR);
                        }
                    }
                    if ((pR3 = cJSON_GetObjectItem(pR2, "gateway")) != NULL)
                    {
                        if (pR3->valuestring != NULL && check_IP_ture(pR3->valuestring) != RES_ERROR)
                        {
                            snprintf(wanBasic.gateway, sizeof(wanBasic.gateway), "%s", pR3->valuestring);
                        }
                        else
                        {
                            Throw(Ex, RES_ERROR);
                        }
                    }
                    zh_set_ethCfg();
                    set_dev_cfg();
                }
                else if (strcmp(_msgType, "setUserAdmin") == 0)
                {
                    if ((pR3 = cJSON_GetObjectItem(pR2, "prePass")) == NULL)
                    {
                        Throw(Ex, RES_ERROR);
                    }
                    if (pR3->valuestring == NULL)
                    {
                        Throw(Ex, RES_ERROR);
                    }
                    if (strcmp(pR3->valuestring, sysBasic.userPassword) != 0)
                    {
                        Throw(Ex, RES_ERROR);
                    }

                    if ((pR3 = cJSON_GetObjectItem(pR2, "newPass")) == NULL)
                    {
                        Throw(Ex, RES_ERROR);
                    }
                    if (pR3->valuestring == NULL)
                    {
                        Throw(Ex, RES_ERROR);
                    }
                    if (zh_string_check(pR3->valuestring) == RES_ERROR)
                    {
                        Throw(Ex, RES_ERROR);
                    }
                    snprintf(sysBasic.userPassword, sizeof(sysBasic.userPassword), "%s", pR3->valuestring);

                    set_dev_cfg();
                }
                else if (strcmp(_msgType, "setUserCMD") == 0)
                {
                    if ((pR3 = cJSON_GetObjectItem(pR2, "cmd")) != NULL)
                    {
                        if (pR3->valueint != REBOOT && pR3->valueint != RESET && pR3->valueint != UPGRADE)
                        {
                            Throw(Ex, RES_ERROR);
                        }
                        sysBasic.cmd = pR3->valueint;
                    }
                }
                else
                {
                    Throw(Ex, RES_ERROR);
                }
            }
            Catch(Ex, RES_ERROR)
            {
                status = 1; //设置结果出错
            }

            char *p;
            cJSON *pJsonRoot;
            cJSON *pSubJson;

            pJsonRoot = cJSON_CreateObject();
            if (NULL != pJsonRoot)
            {
                snprintf(_tempBuff1, sizeof(_tempBuff1), "%sAck", _msgType);
                cJSON_AddStringToObject(pJsonRoot, "msgType", _tempBuff1);
                cJSON_AddItemToObject(pJsonRoot, "data", pSubJson = cJSON_CreateObject());
                cJSON_AddNumberToObject(pSubJson, "status", status);
                p = cJSON_PrintUnformatted(pJsonRoot);
                if (p != NULL)
                {
                    send_localServer_sys_data(socketFd, (uint8_t *)p, strlen(p));
                    free(p);
                    cJSON_Delete(pJsonRoot);
                }
                else
                {
                    cJSON_Delete(pJsonRoot);
                }
            }
        }
        else
        {
            cJSON_Delete(pR1);
            return RES_ERROR;
        }
    }

    cJSON_Delete(pR1);
    return RES_OK;
}

/***************************************************
 * 功能：客户端接收处理 调用函数
 * 参数：*socketInfo 连接客户端信息地址
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年09月12日 
*************************************************/
void *fun_sys_thrReceiveHandler(void *socketInfo)
{
    char readBuff[SYS_MAX_BUFF];
    int readLen, i;
    clientSocketInfo _socketInfo = *((clientSocketInfo *)socketInfo);
    for (i = 0; i < SYS_LISTEN_MAX_CLIENT_NUM; i++)
    {
        if (localServer_sys_info.socketIsLive[i] == 0)
        {
            localServer_sys_info.socketConGroup[i] = _socketInfo.socketCon;
            localServer_sys_info.socketIsLive[i] = 1;
            break;
        }
    }
    //pthread_mutex_unlock(&SYS_Server_Mutex); //确保这个结构体参数值被一个临时变量保存过后，才允许修改。
    while (1)
    {
        //添加对buffer清零
        bzero(&readBuff, sizeof(readBuff));
        readLen = recv(_socketInfo.socketCon, readBuff, SYS_MAX_BUFF, 0);
        if (readLen > 0)
        {
            log_i("[Recv<-Node]--Socket:%d--Len:%d--Data:%s\n", _socketInfo.socketCon, readLen, readBuff);
            //:
            localServer_sys_dataProc(_socketInfo.socketCon, readBuff, readLen);
        }
        else
        {
            log_i("SYS 客户端[%s:%d]  连接关闭 \n", _socketInfo.ipaddr, _socketInfo.port);
            for (i = 0; i < SYS_LISTEN_MAX_CLIENT_NUM; i++)
            {
                if (localServer_sys_info.socketConGroup[i] == _socketInfo.socketCon)
                {
                    localServer_sys_info.socketIsLive[i] = 0;
                }
            }
            localServer_sys_info.conClientCount--;
            close(_socketInfo.socketCon);
            break;
        }
    }
    log_i("SYS 客户端接受数据线程结束了\n");
    return NULL;
}

/***************************************************
 * 功能：创建本地DEBUG服务----上位机通讯
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
*************************************************/
void create_localServer_sys_service(void)
{
    clientSocketInfo socketInfo;
    int server_fd = -1;
    struct sockaddr_in server;                               /* server's address information */
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) /* Create DEBUG socket */
    {
        /* handle exception */
        log_e("SYS socket() error. Failed to initiate a socket  : %s\n", strerror(errno));
        sleep(3);
        return;
    }
    /* set socket option */
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SYS_LISTEN_ADDR);
    server.sin_port = htons(SYS_LISTEN_PORT);

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        log_e("SYS Server Bind() ERROR: %s\n", strerror(errno));
        close(server_fd); /* close server_fd */
        sleep(3);
        return;
    }
    if (listen(server_fd, SYS_LISTEN_MAX_CLIENT_NUM) == -1)
    {
        log_e("SYS server Listen() ERROR: %s\n", strerror(errno));
        close(server_fd); /* close server_fd */
        sleep(3);
        return;
    }
    log_i("SYS Server Listen SUCCESS\n");

    while (1)
    {
        int sockaddr_in_size = sizeof(struct sockaddr_in);
        struct sockaddr_in client_addr;
        //pthread_mutex_lock(&SYS_Server_Mutex);
        int socketCon = accept(server_fd, (struct sockaddr *)(&client_addr), (socklen_t *)(&sockaddr_in_size));
        if (socketCon < 0)
        {
            log_i("SYS accept 连接失败 : %s\n", strerror(errno));
            // pthread_mutex_unlock(&SYS_Server_Mutex);
            break;
        }
        else
        {
            if (localServer_sys_info.conClientCount >= SYS_LISTEN_MAX_CLIENT_NUM)
            {
                log_i("SYS 服务器 当前连接了【%d】个用户，已达连接上限，请稍后重试\n", localServer_sys_info.conClientCount);
                close(socketCon);
                sleep(3);
                // pthread_mutex_unlock(&SYS_Server_Mutex);
                continue;
            }
            else
            {
                log_i("SYS -node连接成功 ip: %s:%d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
                // struct timeval timeout = {3 * 60, 0}; /*  3min 内无数据主动断开*/
                // setsockopt(socketCon, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
            }
        }
        //开启新的通讯线程，负责同连接上来的客户端进行通讯
        socketInfo.socketCon = socketCon;
        socketInfo.ipaddr = inet_ntoa(client_addr.sin_addr);
        socketInfo.port = client_addr.sin_port;
        localServer_sys_info.conClientCount++;
        log_i("SYS 当前共连接【%d】个node\n", localServer_sys_info.conClientCount);
        pthread_t thrReceive = 0;
        if (pthread_create(&thrReceive, NULL, fun_sys_thrReceiveHandler, &socketInfo) == 0)
            pthread_detach(thrReceive); // 线程分离，结束时自动回收资源
        usleep(50000);
    }
    // 等待子进程退出
    close(server_fd); /* close server_fd */
    return;
}

/**
 * @brief :  
 * @param {int} client_socket
 * @param {uint8_t} *dataBuff
 * @param {int} dataLen
 * @return {*}
 * @author: LR
 * @Date: 2021-09-29 16:09:23
 */
int send_localServer_sys_data(int client_socket, uint8_t *dataBuff, int dataLen)
{
    if (client_socket < 0 || dataLen <= 0)
        return RES_ERROR;

    //pthread_mutex_lock(&SYS_send_Mutex);
    if (send(client_socket, dataBuff, dataLen, MSG_NOSIGNAL) < 0)
    {
        // pthread_mutex_unlock(&SYS_send_Mutex);
        return RES_ERROR;
    }
    log_i("[Send->Node]--Socket:%d--Len:%d--Data:%s\n", client_socket, dataLen, dataBuff);
    //pthread_mutex_unlock(&SYS_send_Mutex);
    return RES_OK;
}
