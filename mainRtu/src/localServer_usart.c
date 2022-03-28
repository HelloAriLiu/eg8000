/*
 * @Description :  
 * @FilePath: /app_code/mainRtu/src/localServer_usart.c
 * @Author:  LR
 * @Date: 2021-10-09 15:21:01
 */

#include "baseInit.h"
#include "cJSON.h"
#include "zh_network.h"
#include "zh_usart.h"
#include "rtuIfsHandle.h"
#include "exception.h"
#include "localServer_usart.h"

localServer_usart_info_T localServer_usart_info[USART_NUM];
pthread_mutex_t Usart_Server_Mutex[USART_NUM] = PTHREAD_MUTEX_INITIALIZER;

/***************************************************
 * 功能：初始化本地DEBUG服务                        
 * 参数：void
 * 返回：void                                                                                                                                                                                  
 * 作者 ：LR
 * 修改日期 ：2020年08月12日           
*************************************************/
void init_localServer_usart_service(int index)
{
    if (index < 0 || index >= USART_NUM)
        return;
    memset(&localServer_usart_info[index], 0, sizeof(localServer_usart_info_T));
}

/**
 * @brief :  
 * @param {char} *readBuff
 * @param {int } readLen
 * @return {*}
 * @author: LR
 * @Date: 2021-10-11 14:49:40
 */
int usart_socket_registerCheck(int usartIndex, char *readBuff, int readLen)
{
    if (usartIndex < 0 || usartIndex >= USART_NUM)
        return RES_ERROR;
    // 用于获取来自TCP的JSON结构
    cJSON *pR1;
    cJSON *pR2;
    cJSON *pR3;
    char _msgType[512];
    int baudRate, dataBit, stopBit, parityBit;

    ExcepSign Ex; // 用于实现异常处理的异常标记
    Try(Ex)
    {
        if ((pR1 = cJSON_Parse(readBuff)) == NULL) // json解析，获得句柄
        {
            return RES_ERROR;
        }

        if ((pR2 = cJSON_GetObjectItem(pR1, "msgType")) == NULL) // 获取msgType
        {
            Throw(Ex, RES_ERROR);
        }
        snprintf(_msgType, sizeof(_msgType), "%s", pR2->valuestring); //提取msgType

        if (strcmp(_msgType, "setRS485Cfg") == 0) // 注册包
        {
            if ((pR2 = cJSON_GetObjectItem(pR1, "data")) == NULL)
            {
                Throw(Ex, RES_ERROR);
            }

            if ((pR3 = cJSON_GetObjectItem(pR2, "br")) == NULL)
            {
                Throw(Ex, RES_ERROR);
            }
            baudRate = pR3->valueint;
            if (baudRate != 115200 && baudRate != 57600 && baudRate != 38400 && baudRate != 19200 && baudRate != 9600 && baudRate != 4800 && baudRate != 2400)
                Throw(Ex, RES_ERROR);

            if ((pR3 = cJSON_GetObjectItem(pR2, "db")) == NULL)
            {
                Throw(Ex, RES_ERROR);
            }
            dataBit = pR3->valueint;
            if (dataBit != 8 && dataBit != 7)
                Throw(Ex, RES_ERROR);

            if ((pR3 = cJSON_GetObjectItem(pR2, "sb")) == NULL)
            {
                Throw(Ex, RES_ERROR);
            }
            stopBit = pR3->valueint;
            if (stopBit != 1 && stopBit != 2)
                Throw(Ex, RES_ERROR);

            if ((pR3 = cJSON_GetObjectItem(pR2, "pa")) == NULL)
            {
                Throw(Ex, RES_ERROR);
            }
            parityBit = pR3->valueint;
            if (parityBit != 0 && parityBit != 1 && parityBit != 2)
                Throw(Ex, RES_ERROR);

            pthread_mutex_lock(&usart_Mutex[usartIndex]);
            zh_usart_init(usart_info.usart_fd[usartIndex], baudRate, dataBit, stopBit, parityBit);
            pthread_mutex_unlock(&usart_Mutex[usartIndex]);
            log_i("usart [%d] 初始化成功\n", usartIndex);
        }
        else
        {
            Throw(Ex, RES_ERROR);
        }
    }
    Catch(Ex, RES_ERROR)
    {
        cJSON_Delete(pR1);
        log_i("usart [%d] 初始化失败\n", usartIndex);
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
void *fun_usart_thrReceiveHandler(void *socketInfo)
{
    char readBuff[USART_MAX_BUFF];
    int readLen, i, index;
    clientSocketInfo _socketInfo = *((clientSocketInfo *)socketInfo);
    _socketInfo.registerCheckFlg = 0;
    index = _socketInfo.index;
    pthread_mutex_unlock(&Usart_Server_Mutex[index]); //确保这个结构体参数值被一个临时变量保存过后，才允许修改。

    if (index < 0 || index >= USART_NUM)
        return NULL;

    while (1)
    {
        //添加对buffer清零
        bzero(&readBuff, sizeof(readBuff));
        readLen = recv(_socketInfo.socketCon, readBuff, USART_MAX_BUFF, 0);
        if (readLen > 0)
        {
            log_i("[Recv<-Node]--Socket:%d--Len:%d--Data:%s\n", _socketInfo.socketCon, readLen, readBuff);
            //todo:
            if (readLen > USART_READ_MAX_BUFF)
                readLen = USART_READ_MAX_BUFF;
            if (_socketInfo.registerCheckFlg == 0) //第一包数据为注册包须校验通过
            {
                if (usart_socket_registerCheck(index, readBuff, readLen) == RES_OK) //校验通过
                {
                    char temp[256];
                    //snprintf(temp, sizeof(temp), "{\"msgType\":\"setRS485CfgAck\",\"data\":{\"status\":0}}");
                    //send_localServer_usart_data(_socketInfo.socketCon, (uint8_t *)temp, strlen(temp));
                    log_i("usart_%d 客户端[%s:%d] 注册初始化成功\n", index, _socketInfo.ipaddr, _socketInfo.port);
                    _socketInfo.registerCheckFlg = 1;
                    for (i = 0; i < USART_LISTEN_MAX_CLIENT_NUM; i++)
                    {
                        if (localServer_usart_info[index].socketIsLive[i] == 0)
                        {
                            localServer_usart_info[index].socketConGroup[i] = _socketInfo.socketCon;
                            localServer_usart_info[index].socketIsLive[i] = 1;
                            break;
                        }
                    }
                }
                else //校验不通过
                {
                    usleep(1000);
                    log_i("usart 客户端[%s:%d] 注册失败-强制断开\n", _socketInfo.ipaddr, _socketInfo.port);
                    localServer_usart_info[index].conClientCount--;
                    close(_socketInfo.socketCon);
                    break;
                }
            }
            else //透传数据
            {
                pthread_mutex_lock(&usart_Mutex[index]);
                log_i("[send->usart_%d]--Socket:%d--Len:%d--Data:%s\n", index, _socketInfo.socketCon, readLen, readBuff);
                zh_usart_send(usart_info.usart_fd[index], (char *)readBuff, readLen);
                pthread_mutex_unlock(&usart_Mutex[index]);
            }
            usleep(1000);
        }
        else
        {
            log_i("USART_%d 客户端[%s:%d]  连接关闭 \n", index, _socketInfo.ipaddr, _socketInfo.port);
            for (i = 0; i < USART_LISTEN_MAX_CLIENT_NUM; i++)
            {
                if (localServer_usart_info[index].socketConGroup[i] == _socketInfo.socketCon)
                {
                    localServer_usart_info[index].socketIsLive[i] = 0;
                }
            }
            localServer_usart_info[index].conClientCount--;
            close(_socketInfo.socketCon);
            break;
        }
    }
    printf("usart 客户端接受数据线程结束了\n");
    return NULL;
}

/***************************************************
 * 功能：创建本地DEBUG服务----上位机通讯
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日
*************************************************/
void create_localServer_usart_service(int index)
{
    if (index < 0 || index >= USART_NUM)
        return;
    clientSocketInfo socketInfo;
    int server_fd = -1;
    struct sockaddr_in server;                               /* server's address information */
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) /* Create DEBUG socket */
    {
        /* handle exception */
        log_e("USART_%d socket() error. Failed to initiate a socket  : %s\n", index, strerror(errno));
        sleep(3);
        return;
    }
    /* set socket option */
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(USART_LISTEN_ADDR);
    server.sin_port = htons(USART_0_LISTEN_PORT + index);

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        log_e("USART_%d Server Bind() ERROR: %s\n", index, strerror(errno));
        close(server_fd); /* close server_fd */
        sleep(3);
        return;
    }
    if (listen(server_fd, USART_LISTEN_MAX_CLIENT_NUM) == -1)
    {
        log_e("USART_%d server Listen() ERROR: %s\n", index, strerror(errno));
        close(server_fd); /* close server_fd */
        sleep(3);
        return;
    }
    log_i("Usart_%d Server Listen SUCCESS\n", index);

    while (1)
    {
        int sockaddr_in_size = sizeof(struct sockaddr_in);
        struct sockaddr_in client_addr;
        pthread_mutex_lock(&Usart_Server_Mutex[index]);
        int socketCon = accept(server_fd, (struct sockaddr *)(&client_addr), (socklen_t *)(&sockaddr_in_size));
        if (socketCon < 0)
        {
            log_i("USART_%d accept 连接失败 : %s\n", index, strerror(errno));
            pthread_mutex_unlock(&Usart_Server_Mutex[index]);
            break;
        }
        else
        {
            if (localServer_usart_info[index].conClientCount > USART_LISTEN_MAX_CLIENT_NUM)
            {
                log_i("USART_%d 服务器 当前连接了【%d】个用户，已达连接上限，请稍后重试\n", index, localServer_usart_info[index].conClientCount);
                close(socketCon);
                sleep(3);
                pthread_mutex_unlock(&Usart_Server_Mutex[index]);
                continue;
            }
            else
            {
                log_i("USART_%d node连接成功 ip: %s:%d\n", index, inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
            }
        }
        //开启新的通讯线程，负责同连接上来的客户端进行通讯
        memset(&socketInfo, 0, sizeof(clientSocketInfo));
        socketInfo.socketCon = socketCon;
        socketInfo.ipaddr = inet_ntoa(client_addr.sin_addr);
        socketInfo.port = client_addr.sin_port;
        socketInfo.index = index;
        localServer_usart_info[index].conClientCount++;
        log_i("USART_%d 当前共连接【%d】个node\n", index, localServer_usart_info[index].conClientCount);
        pthread_t thrReceive = 0;
        if (pthread_create(&thrReceive, NULL, fun_usart_thrReceiveHandler, &socketInfo) == 0)
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
int send_localServer_usart_data(int client_socket, uint8_t *dataBuff, int dataLen)
{
    log_i("[Send->Node]--Socket:%d--Len:%d--Data:%s\n", client_socket, dataLen, dataBuff);
    if (client_socket < 0 || dataLen <= 0)
        return RES_ERROR;

    if (send(client_socket, dataBuff, dataLen, MSG_NOSIGNAL) < 0)
    {
        return RES_ERROR;
    }

    return RES_OK;
}
