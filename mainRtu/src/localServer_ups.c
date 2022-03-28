/*
 * @Description :  
 * @FilePath: /app_code/mainRtu/src/localServer_ups.c
 * @Author:  LR
 * @Date: 2021-10-23 14:35:47
 */

#include "baseInit.h"
#include "cJSON.h"
#include "zh_network.h"
#include "rtuIfsHandle.h"
#include "localServer_ups.h"
#include "system.h"

localServer_ups_info_T localServer_ups_info;
pthread_mutex_t UPS_Server_Mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t UPS_send_Mutex = PTHREAD_MUTEX_INITIALIZER;

/***************************************************
 * 功能：初始化本地DEBUG服务
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日
*************************************************/
void init_localServer_ups_service(void)
{
    memset(&localServer_ups_info, 0, sizeof(localServer_ups_info_T));
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
int localServer_ups_dataProc(int socketFd, char *recvData, int recvLen)
{
    if (recvLen < 5)
        return RES_ERROR;

    char _msgType[100] = {0}; // 纪录msgType
    char temp[256];
    int ups_value;

    // 用于获取来自TCP的JSON结构
    cJSON *pR1 = NULL;
    cJSON *pR2 = NULL;
    cJSON *pR3 = NULL;
    char revPDU[UPS_MAX_BUFF] = {0};
    if (recvLen > UPS_MAX_BUFF)
    {
        recvLen = UPS_MAX_BUFF;
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
        if (strcmp(_msgType, "getEXPValue") == 0)
        {
            snprintf(temp, sizeof(temp), "{\"msgType\":\"getEXPValueAck\",\"data\":{\"value\":%d}}", sys_info.upsNewState);
            send_localServer_ups_data(socketFd, (uint8_t *)temp, strlen(temp));
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
void *fun_ups_thrReceiveHandler(void *socketInfo)
{
    char readBuff[UPS_MAX_BUFF];
    int readLen, i;
    clientSocketInfo _socketInfo = *((clientSocketInfo *)socketInfo);
    for (i = 0; i < UPS_LISTEN_MAX_CLIENT_NUM; i++)
    {
        if (localServer_ups_info.socketIsLive[i] == 0)
        {
            localServer_ups_info.socketConGroup[i] = _socketInfo.socketCon;
            localServer_ups_info.socketIsLive[i] = 1;
            break;
        }
    }
    pthread_mutex_unlock(&UPS_Server_Mutex); //确保这个结构体参数值被一个临时变量保存过后，才允许修改。
    while (1)
    {
        //添加对buffer清零
        bzero(&readBuff, sizeof(readBuff));
        readLen = recv(_socketInfo.socketCon, readBuff, UPS_MAX_BUFF, 0);
        if (readLen > 0)
        {
            log_i("[Recv<-Node]--Socket:%d--Len:%d--Data:%s\n", _socketInfo.socketCon, readLen, readBuff);
            //:
            localServer_ups_dataProc(_socketInfo.socketCon, readBuff, readLen);
        }
        else
        {
            log_i("UPS 客户端[%s:%d]  连接关闭 \n", _socketInfo.ipaddr, _socketInfo.port);
            for (i = 0; i < UPS_LISTEN_MAX_CLIENT_NUM; i++)
            {
                if (localServer_ups_info.socketConGroup[i] == _socketInfo.socketCon)
                {
                    localServer_ups_info.socketIsLive[i] = 0;
                }
            }
            localServer_ups_info.conClientCount--;
            close(_socketInfo.socketCon);
            break;
        }
    }
    printf("UPS客户端接受数据线程结束了\n");
    return NULL;
}

/***************************************************
 * 功能：创建本地DEBUG服务----上位机通讯
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
*************************************************/
void create_localServer_ups_service(void)
{
    clientSocketInfo socketInfo;
    int server_fd = -1;
    struct sockaddr_in server;                               /* server's address information */
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) /* Create DEBUG socket */
    {
        /* handle exception */
        log_e("UPS socket() error. Failed to initiate a socket  : %s\n", strerror(errno));
        sleep(3);
        return;
    }
    /* set socket option */
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(UPS_LISTEN_ADDR);
    server.sin_port = htons(UPS_LISTEN_PORT);

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        log_e("UPS Server Bind() ERROR: %s\n", strerror(errno));
        close(server_fd); /* close server_fd */
        sleep(3);
        return;
    }
    if (listen(server_fd, UPS_LISTEN_MAX_CLIENT_NUM) == -1)
    {
        log_e("UPS server Listen() ERROR: %s\n", strerror(errno));
        close(server_fd); /* close server_fd */
        sleep(3);
        return;
    }
    log_i("UPS Server Listen SUCCESS\n");

    while (1)
    {
        int sockaddr_in_size = sizeof(struct sockaddr_in);
        struct sockaddr_in client_addr;
        pthread_mutex_lock(&UPS_Server_Mutex);
        int socketCon = accept(server_fd, (struct sockaddr *)(&client_addr), (socklen_t *)(&sockaddr_in_size));
        if (socketCon < 0)
        {
            log_i("UPS accept 连接失败 : %s\n", strerror(errno));
            pthread_mutex_unlock(&UPS_Server_Mutex);
            break;
        }
        else
        {
            if (localServer_ups_info.conClientCount > UPS_LISTEN_MAX_CLIENT_NUM)
            {
                log_i("UPS 服务器 当前连接了【%d】个用户，已达连接上限，请稍后重试\n", localServer_ups_info.conClientCount);
                close(socketCon);
                sleep(3);
                pthread_mutex_unlock(&UPS_Server_Mutex);
                continue;
            }
            else
            {
                log_i("UPS node连接成功 ip: %s:%d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
                // struct timeval timeout = {3 * 60, 0}; /*  3min 内无数据主动断开*/
                // setsockopt(socketCon, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
            }
        }
        //开启新的通讯线程，负责同连接上来的客户端进行通讯
        socketInfo.socketCon = socketCon;
        socketInfo.ipaddr = inet_ntoa(client_addr.sin_addr);
        socketInfo.port = client_addr.sin_port;
        localServer_ups_info.conClientCount++;
        log_i("UPS 当前共连接【%d】个node\n", localServer_ups_info.conClientCount);
        pthread_t thrReceive = 0;
        if (pthread_create(&thrReceive, NULL, fun_ups_thrReceiveHandler, &socketInfo) == 0)
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
int send_localServer_ups_data(int client_socket, uint8_t *dataBuff, int dataLen)
{
    if (client_socket < 0 || dataLen <= 0)
        return RES_ERROR;

    pthread_mutex_lock(&UPS_send_Mutex);
    if (send(client_socket, dataBuff, dataLen, MSG_NOSIGNAL) < 0)
    {
        pthread_mutex_unlock(&UPS_send_Mutex);
        return RES_ERROR;
    }
    pthread_mutex_unlock(&UPS_send_Mutex);
    return RES_OK;
}
