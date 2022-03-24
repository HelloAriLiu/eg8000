/*
 * @Description :  
 * @FilePath: /app_code/mainRtu/src/localServer_ao.c
 * @Author:  LR
 * @Date: 2021-10-23 14:30:09
 */

#include "baseInit.h"
#include "cJSON.h"
#include "zh_network.h"
#include "rtuIfsHandle.h"
#include "localServer_ao.h"

localServer_ao_info_T localServer_ao_info;
pthread_mutex_t AO_Server_Mutex = PTHREAD_MUTEX_INITIALIZER;

/***************************************************
 * 功能：初始化本地DEBUG服务
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日
*************************************************/
void init_localServer_ao_service(void)
{
    memset(&localServer_ao_info, 0, sizeof(localServer_ao_info_T));
}

/**
 * @brief :  ao交互json解析
 * @param {int} *socketFd
 * @param {uint8_t} *recvData
 * @param {int} recvLen
 * @return {*}
 * @author: LR
 * @Date: 2021-09-28 15:20:09
 */
int localServer_ao_dataProc(int socketFd, char *recvData, int recvLen)
{
    if (recvLen < 5)
        return RES_ERROR;

    char _msgType[100] = {0}; // 纪录msgType
    char temp[256] = {0};
    int ao_index;
    float ao_value;

    // 用于获取来自TCP的JSON结构
    cJSON *pR1 = NULL;
    cJSON *pR2 = NULL;
    cJSON *pR3 = NULL;
    char revPDU[AO_MAX_BUFF] = {0};
    if (recvLen > AO_MAX_BUFF)
    {
        recvLen = AO_MAX_BUFF;
    }
    memcpy(revPDU, recvData, recvLen);

    ExcepSign Ex; // 用于实现异常处理的异常标记
    Try(Ex)
    {
        if ((pR1 = cJSON_Parse(revPDU)) == NULL) // json解析，获得句柄
        {
            return RES_ERROR;
        }

        if ((pR2 = cJSON_GetObjectItem(pR1, "msgType")) == NULL) // 获取msgType
        {
            Throw(Ex, RES_ERROR);
        }
        snprintf(_msgType, sizeof(_msgType), "%s", pR2->valuestring); //提取msgType

        if (strcmp(_msgType, "getAoutValue") == 0)
        {
            if ((pR2 = cJSON_GetObjectItem(pR1, "data")) == NULL)
            {
                Throw(Ex, RES_ERROR);
            }
            if ((pR3 = cJSON_GetObjectItem(pR2, "index")) == NULL) // 获取msgType
            {
                Throw(Ex, RES_ERROR);
            }
            ao_index = pR3->valueint;
            if (ao_index < 1 || ao_index > AO_NUM)
                Throw(Ex, RES_ERROR);

            snprintf(temp, sizeof(temp), "{\"msgType\":\"getAoutValueAck\",\"data\":{\"index\":%d,\"value\":%f}}", ao_index, ifs_data.aoutOldValue[ao_index - 1]);
            send_localServer_ao_data(socketFd, (uint8_t *)temp, strlen(temp));
        }
        else if (strcmp(_msgType, "getAoutValueAll") == 0)
        {
            if ((pR2 = cJSON_GetObjectItem(pR1, "data")) == NULL)
            {
                Throw(Ex, RES_ERROR);
            }
            char *pStr = NULL;
            int i = 0;
            //默认基础参数
            cJSON *pS1;
            cJSON *pS2;

            if ((pS1 = cJSON_CreateObject()) == NULL)
            {
                log_e("cJSON_CreateObject()) == NULL\n");
                Throw(Ex, RES_ERROR);
            }

            cJSON_AddStringToObject(pS1, "msgType", "getAoutValueAllAck");
            cJSON_AddItemToObject(pS1, "data", pS2 = cJSON_CreateObject());
            {
                for (i = 0; i < AO_NUM; i++)
                {
                    sprintf(temp, "ch%d", i + 1);
                    cJSON_AddNumberToObject(pS2, temp, ifs_data.aoutOldValue[i]);
                }
            }
            pStr = cJSON_Print(pS1);
            cJSON_Delete(pS1);
            send_localServer_ao_data(socketFd, (uint8_t *)pStr, strlen(pStr));
            free(pStr);
        }
        else if (strcmp(_msgType, "setAoutValue") == 0)
        {
            if ((pR2 = cJSON_GetObjectItem(pR1, "data")) == NULL)
            {
                Throw(Ex, RES_ERROR);
            }
            if ((pR3 = cJSON_GetObjectItem(pR2, "index")) == NULL)
            {
                Throw(Ex, RES_ERROR);
            }
            ao_index = pR3->valueint;
            if (ao_index < 1 || ao_index > AO_NUM)
            {
                snprintf(temp, sizeof(temp), "{\"msgType\":\"setAoutValueAck\",\"data\":{\"status\":1}}");
                send_localServer_ao_data(socketFd, (uint8_t *)temp, strlen(temp));
                Throw(Ex, RES_ERROR);
            }

            if ((pR3 = cJSON_GetObjectItem(pR2, "value")) == NULL)
            {
                Throw(Ex, RES_ERROR);
            }
            ao_value = pR3->valuedouble;
            if (ao_value < 0 || ao_value > 20)
            {
                snprintf(temp, sizeof(temp), "{\"msgType\":\"setAoutValueAck\",\"data\":{\"status\":1}}");
                send_localServer_ao_data(socketFd, (uint8_t *)temp, strlen(temp));
                Throw(Ex, RES_ERROR);
            }

            ifs_data.aoutNewValue[ao_index - 1] = ao_value;
            snprintf(temp, sizeof(temp), "{\"msgType\":\"setAoutValueAck\",\"data\":{\"status\":0}}");
            send_localServer_ao_data(socketFd, (uint8_t *)temp, strlen(temp));
        }
        else
        {
            Throw(Ex, RES_ERROR);
        }
    }
    Catch(Ex, RES_ERROR)
    {
        cJSON_Delete(pR1);
        return RES_ERROR;
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
void *fun_ao_thrReceiveHandler(void *socketInfo)
{
    char readBuff[AO_MAX_BUFF];
    int readLen, i;
    clientSocketInfo _socketInfo = *((clientSocketInfo *)socketInfo);
    for (i = 0; i < AO_LISTEN_MAX_CLIENT_NUM; i++)
    {
        if (localServer_ao_info.socketIsLive[i] == 0)
        {
            localServer_ao_info.socketConGroup[i] = _socketInfo.socketCon;
            localServer_ao_info.socketIsLive[i] = 1;
            break;
        }
    }
    pthread_mutex_unlock(&AO_Server_Mutex); //确保这个结构体参数值被一个临时变量保存过后，才允许修改。
    while (1)
    {
        //添加对buffer清零
        bzero(&readBuff, sizeof(readBuff));
        readLen = recv(_socketInfo.socketCon, readBuff, AO_MAX_BUFF, 0);
        if (readLen > 0)
        {
            log_i("[Recv<-Node]--Socket:%d--Len:%d--Data:%s\n", _socketInfo.socketCon, readLen, readBuff);
            //todo:
            localServer_ao_dataProc(_socketInfo.socketCon, readBuff, readLen);
        }
        else
        {
            log_i("AO 客户端[%s:%d]  连接关闭 \n", _socketInfo.ipaddr, _socketInfo.port);
            for (i = 0; i < AO_LISTEN_MAX_CLIENT_NUM; i++)
            {
                if (localServer_ao_info.socketConGroup[i] == _socketInfo.socketCon)
                {
                    localServer_ao_info.socketIsLive[i] = 0;
                }
            }
            localServer_ao_info.conClientCount--;
            close(_socketInfo.socketCon);
            break;
        }
    }
    printf("AO客户端接受数据线程结束了\n");
    return NULL;
}

/***************************************************
 * 功能：创建本地DEBUG服务----上位机通讯
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
*************************************************/
void create_localServer_ao_service(void)
{
    clientSocketInfo socketInfo;
    int server_fd = -1;
    struct sockaddr_in server;                               /* server's address information */
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) /* Create DEBUG socket */
    {
        /* handle exception */
        log_e("AO socket() error. Failed to initiate a socket  : %s\n", strerror(errno));
        sleep(3);
        return;
    }
    /* set socket option */
    int opt = SO_REUSEADDR;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(AO_LISTEN_ADDR);
    server.sin_port = htons(AO_LISTEN_PORT);

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        log_e("AO Server Bind() ERROR: %s\n", strerror(errno));
        close(server_fd); /* close server_fd */
        sleep(3);
        return;
    }
    if (listen(server_fd, AO_LISTEN_MAX_CLIENT_NUM) == -1)
    {
        log_e("AO server Listen() ERROR: %s\n", strerror(errno));
        close(server_fd); /* close server_fd */
        sleep(3);
        return;
    }
    log_i("AO Server Listen SUCCESS\n");

    while (1)
    {
        int sockaddr_in_size = sizeof(struct sockaddr_in);
        struct sockaddr_in client_addr;
        pthread_mutex_lock(&AO_Server_Mutex);
        int socketCon = accept(server_fd, (struct sockaddr *)(&client_addr), (socklen_t *)(&sockaddr_in_size));
        if (socketCon < 0)
        {
            log_i("AO accept 连接失败 : %s\n", strerror(errno));
            pthread_mutex_unlock(&AO_Server_Mutex);
            break;
        }
        else
        {
            if (localServer_ao_info.conClientCount > AO_LISTEN_MAX_CLIENT_NUM)
            {
                log_i("AO服务器 当前连接了【%d】个用户，已达连接上限，请稍后重试\n", localServer_ao_info.conClientCount);
                close(socketCon);
                sleep(3);
                pthread_mutex_unlock(&AO_Server_Mutex);
                continue;
            }
            else
            {
                log_i("AO-node连接成功 ip: %s:%d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
                // struct timeval timeout = {3 * 60, 0}; /*  3min 内无数据主动断开*/
                // setsockopt(socketCon, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
            }
        }
        //开启新的通讯线程，负责同连接上来的客户端进行通讯
        socketInfo.socketCon = socketCon;
        socketInfo.ipaddr = inet_ntoa(client_addr.sin_addr);
        socketInfo.port = client_addr.sin_port;
        localServer_ao_info.conClientCount++;
        log_i("AO 当前共连接【%d】个node\n", localServer_ao_info.conClientCount);
        pthread_t thrReceive = 0;
        if (pthread_create(&thrReceive, NULL, fun_ao_thrReceiveHandler, &socketInfo) == 0)
            pthread_detach(thrReceive); // 线程分离，结束时自动回收资源
        usleep(5000);
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
int send_localServer_ao_data(int client_socket, uint8_t *dataBuff, int dataLen)
{
    if (client_socket < 0 || dataLen <= 0)
        return RES_ERROR;

    //pthread_mutex_lock(&Ai_send_Mutex);
    if (send(client_socket, dataBuff, dataLen, MSG_NOSIGNAL) < 0)
    {
        //pthread_mutex_unlock(&Ai_send_Mutex);
        return RES_ERROR;
    }
    // pthread_mutex_unlock(&Ai_send_Mutex);
    return RES_OK;
}
