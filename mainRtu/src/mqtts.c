/*
 * @Description :  
 * @FilePath: /app_code/mainRtu/src/mqtts.c
 * @Author:  LR
 * @Date: 2022-01-17 16:47:23
 */

/*
 * @Description :  
 * @FilePath: /mqtt-test/src/my-mqttcs.c
 * @Author:  LR
 * @Date: 2022-01-12 15:37:06
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "cJSON.h"
#include "vpnHandle.h"
#include "baseInit.h"
#include "fifoBuff.h"
#include "mqtts.h"
#include "../lib/libmqtt/include/MQTTClient.h"
//#include "MQTTClient.h"

fifoBuffList_t *mqtt_read_BUFF = NULL;
fifoBuffList_t *mqtt_write_BUFF = NULL;

pthread_mutex_t mqtt_readData_mutex = PTHREAD_MUTEX_INITIALIZER;  //读队列操作锁
pthread_mutex_t mqtt_writeData_mutex = PTHREAD_MUTEX_INITIALIZER; //写队列操作锁

#define MQTT_STATE_DISCONN 0
#define MQTT_STATE_SUB 1
#define MQTT_STATE_REG 2
#define MQTT_STATE_REG_ACK 3
#define MQTT_STATE_READY 4
char MQTT_STATE = 0;            //Mqtt状态机
unsigned int mqtt_reg_tick = 0; //Mqtt注册计时器
char mqtt_reg_flag = 0;         //Mqtt注册

#define QOS 0
#define TIMEOUT 10000L

MQTTClient client;
MQTTClient_deliveryToken token;
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;

char ADDRESS[128];
char CLIENTID[128];
char REGPUB[128];
char REGSUB[128];
char DATAPUB[128];
char DATASUB[128];

/**
 * @brief :  mqtt初始化
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2022-01-18 09:43:07
 */
int mqtt_init(void)
{
    fifoBuffListFree(&mqtt_read_BUFF);
    fifoBuffListFree(&mqtt_write_BUFF);

    conn_opts.username = "device_A584A6CA21E88B27";
    conn_opts.password = "A584A6CA21E88B27";
    conn_opts.connectTimeout = 10;
    //snprintf(ADDRESS, sizeof(ADDRESS), "ssl://121.36.229.225:8883");
    snprintf(ADDRESS, sizeof(ADDRESS), "tcp://139.129.229.113:1883");
    snprintf(CLIENTID, sizeof(CLIENTID), "%s", sysBasic.sn);
    snprintf(REGPUB, sizeof(REGPUB), "client/device/eg8000/register/up");
    snprintf(REGSUB, sizeof(REGSUB), "client/device/eg8000/%s/register/down", sysBasic.sn);
    snprintf(DATASUB, sizeof(DATASUB), "client/device/eg8000/%s/data/down", sysBasic.sn);
    snprintf(DATAPUB, sizeof(DATAPUB), "client/device/eg8000/%s/data/up", sysBasic.sn);

    // ssl_opts.trustStore = "/home/pi/edge8000/app/mqtt-ssl/cacert.pem";     //CA证书
    // ssl_opts.keyStore = "/home/pi/edge8000/app/mqtt-ssl/client-cert.pem";  //客户端公钥
    // ssl_opts.privateKey = "/home/pi/edge8000/app/mqtt-ssl/client-key.pem"; //客户端私钥
    // ssl_opts.sslVersion = MQTT_SSL_VERSION_DEFAULT;
    // conn_opts.ssl = &ssl_opts;

    return 0;
}

/**
 * @brief :  向mqtt任务-写缓存-填充数据
 * @param {unsigned char} *dataBuff
 * @param {int} dataLen
 * @return {*}
 * @author: LR
 * @Date: 2021-06-15 10:44:12
 */
void add_mqtt_write_buff(unsigned char *dataBuff, int dataLen)
{
    if (dataLen <= 0 || dataBuff == NULL)
        return;

    pthread_mutex_lock(&mqtt_writeData_mutex); /* 上锁 */
    fifoBuffListAdd(&mqtt_write_BUFF, dataBuff, dataLen);
    pthread_mutex_unlock(&mqtt_writeData_mutex); /* 解锁 */
    usleep(5000);
}

/**
 * @brief :  表示 MQTT 消息的值。发布消息时，传递令牌将返回给客户端应用程序。然后可以使用令牌来检查消息是否已成功传送到其目的地
 * @param {void} *context
 * @param {MQTTClient_deliveryToken} dt
 * @return {*}
 * @author: LR
 * @Date: 2021-07-19 15:05:02
 */
volatile MQTTClient_deliveryToken deliveredtoken;
void mqtt_delivered_callback(void *context, MQTTClient_deliveryToken dt)
{
    log_i("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

/**
 * @brief :  这是一个回调函数。客户端应用程序必须提供此函数的实现以启用消息的异步接收。
 * 该函数通过将其作为参数传递给MQTTClient_setCallbacks()来向客户端库注册。当从服务器收到与客户端订阅匹配的新消息时，客户端库会调用它。
 * 此函数在与运行客户端应用程序的线程不同的线程上执行。 
 * @param {void} *context
 * @param {char} *topicName
 * @param {int} topicLen
 * @param {MQTTClient_message} *message
 * @return {*}
 * @author: LR
 * @Date: 2021-07-19 15:04:23
 */
int mqtt_msgarrvd_callback(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    char *payloadptr;
    int payloadlen;
    payloadptr = message->payload;
    payloadlen = message->payloadlen;
    log_i("topic[%s] sublishMessage [%d]:%s  \n\n", topicName, payloadlen, payloadptr);
    {
        //Todo:填入接收buff
        pthread_mutex_lock(&mqtt_readData_mutex); /* 上锁 */
        fifoBuffListAdd(&mqtt_read_BUFF, payloadptr, payloadlen);
        pthread_mutex_unlock(&mqtt_readData_mutex); /* 解锁 */
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

/**
 * @brief :  断开连接的回调处理
 * @param {void} *context
 * @param {char} *cause
 * @return {*}
 * @author: LR
 * @Date: 2021-07-19 15:05:37
 */
void mqtt_connlost_callback(void *context, char *cause)
{
    log_i("Connection lost cause: %s\n", cause);
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    MQTT_STATE = MQTT_STATE_DISCONN;
}

/**
 * @brief :  指数补偿算法 ，如果连接失败后每一次休眠的时间会指数级增长。防止频繁重连.
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2022-01-13 11:16:51
 */
#define MAXSLEEP 120
int mqtt_connect_retry(void)
{
    int numsec;
    for (numsec = 1; numsec <= MAXSLEEP; numsec <<= 1)
    {
        if (mqtt_connect() == MQTTCLIENT_SUCCESS)
        {
            return MQTTCLIENT_SUCCESS;
        }
        if (numsec <= MAXSLEEP / 2)
            sleep(numsec);
    }

    return MQTTCLIENT_FAILURE;
}


/**
 * @brief :  mqtt-连接处理
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-07-19 16:00:38
 */
int mqtt_connect(void)
{
    mqtt_init();

    int rc;
    if ((rc = MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
        log_i("Failed to create, return code %d\n", rc);
        return MQTTCLIENT_FAILURE;
    }

    if ((rc = MQTTClient_setCallbacks(client, NULL, mqtt_connlost_callback, mqtt_msgarrvd_callback, mqtt_delivered_callback)) != MQTTCLIENT_SUCCESS)
    {
        log_i("Failed to setCallbacks, return code %d\n", rc);
        MQTTClient_destroy(&client);
        return MQTTCLIENT_FAILURE;
    }

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        log_i("Failed to connect, return code %d\n", rc);
        sleep(10);
        MQTTClient_destroy(&client);
        return MQTTCLIENT_FAILURE;
    }

    log_i("MQTTClient_connect  OK    \n");
    return MQTTCLIENT_SUCCESS;
}


/**
 * @brief :  mqtt-订阅消息
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-07-19 16:14:44
 */
int mqtt_subscribe(char *subtopic)
{
    if (subtopic == NULL)
    {
        return MQTTCLIENT_FAILURE;
    }

    int rc;
    if ((rc = MQTTClient_subscribe(client, subtopic, QOS)) != MQTTCLIENT_SUCCESS)
    {
        log_i("Failed to subscribe[%s], return code %d\n", subtopic, rc);
        return MQTTCLIENT_FAILURE;
    }

    return MQTTCLIENT_SUCCESS;
}

/**
 * @brief :  mqtt 发布消息
 * @param {char} *pubMsg
 * @param {int} pubMsgLen
 * @return {*}
 * @author: LR
 * @Date: 2021-07-19 16:00:29
 */
int mqtt_publish(char *pubtopic, char *pubMsg, int pubMsgLen)
{
    if (pubtopic == NULL || pubMsg == NULL || pubMsgLen <= 0)
        return MQTTCLIENT_FAILURE;

    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = pubMsg;
    pubmsg.payloadlen = pubMsgLen;
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    deliveredtoken = 0;

    if (MQTTClient_publishMessage(client, pubtopic, &pubmsg, &token) != MQTTCLIENT_SUCCESS)
    {
        log_i("MQTTClient_publishMessage of error   \n");
        return MQTTCLIENT_FAILURE;
    }

    log_i(" publishMessage->[%s] [%d]:%s    \n", pubtopic, pubMsgLen, pubMsg);
    return MQTTCLIENT_SUCCESS;
}

/**
 * @brief :  mqttc处理状态机
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-07-19 16:51:36
 */
void mqtt_state_proc(void)
{
    switch (MQTT_STATE)
    {
    default:
    case MQTT_STATE_DISCONN:
    {
        if (mqtt_connect() == MQTTCLIENT_SUCCESS)
        {
            MQTT_STATE = MQTT_STATE_SUB;
            log_i("MQTT_STATE_CONN-->OK\n");
        }
    }
    break;
    case MQTT_STATE_SUB:
    {
        if (mqtt_subscribe(REGSUB) != MQTTCLIENT_SUCCESS)
        {
            log_i("reg_subscribe error\n");
            MQTT_STATE = MQTT_STATE_DISCONN;
        }
        if (mqtt_subscribe(DATASUB) != MQTTCLIENT_SUCCESS)
        {
            log_i("data_subscribe error\n");
            MQTT_STATE = MQTT_STATE_DISCONN;
        }
        MQTT_STATE = MQTT_STATE_REG;
        log_i("MQTT_STATE_SUB-->OK\n");
    }
    break;
    case MQTT_STATE_REG:
    {
        //注册包：
        char mqttReg[1024];
        snprintf(mqttReg, sizeof(mqttReg), "{\"msgType\": \"devReg\",\
\"msgID\": \"%ld\",\
\"data\": {\
\"model\": \"%s\",\
\"sn\": \"%s\",\
\"version\": \"%s\",\
\"psk\": \"%s\"}}",
                 get_sys_timestamp(), sysBasic.model, sysBasic.sn, sysBasic.version, sysBasic.userPassword);

        mqtt_publish(REGPUB, mqttReg, strlen(mqttReg)); //发送注册包
        MQTT_STATE = MQTT_STATE_REG_ACK;
        mqtt_reg_flag = 0;
        mqtt_reg_tick = 0;
    }
    break;
    case MQTT_STATE_REG_ACK:
    {
        //SUBLISH DATA 接收解析
        unsigned char *readData;
        unsigned int readLen = 0;
        pthread_mutex_lock(&mqtt_readData_mutex); /* 上锁 */
        readData = fifoBuffListDel(&mqtt_read_BUFF, &readLen);
        if (readLen > 0)
        {
            //mqtt解析处理
            mqtt_dataProc(readData, readLen);
            free(readData);
            readData = NULL;
        }
        readData = NULL;
        pthread_mutex_unlock(&mqtt_readData_mutex); /* 解锁 */

        if (mqtt_reg_flag == 1)
        {
            frpCfg_config();
            MQTT_STATE = MQTT_STATE_READY;
            log_i("MQTT_STATE_REG_ACK-->OK\n");
        }
        else
        {
            if (mqtt_reg_tick > 10)
            {
                MQTTClient_disconnect(client, 0);
                MQTTClient_destroy(&client);
                MQTT_STATE = MQTT_STATE_REG;
                log_i("MQTT_STATE_REG_ACK-->Check OUT\n");
            }
        }
    }
    break;
    case MQTT_STATE_READY:
    {
        //PUBLISH DATA 发送组装
        unsigned char *writeData;
        unsigned int writeLen = 0;
        pthread_mutex_lock(&mqtt_writeData_mutex); /* 上锁 */
        writeData = fifoBuffListDel(&mqtt_write_BUFF, &writeLen);
        if (writeLen > 0 && writeData != NULL)
        {
            //Todo:检测发送数据缓冲区，发送数据pub
            mqtt_publish(DATAPUB, writeData, writeLen);
            free(writeData);
        }
        writeData = NULL;
        pthread_mutex_unlock(&mqtt_writeData_mutex); /* 解锁 */

        //SUBLISH DATA 接收解析
        unsigned char *readData;
        unsigned int readLen = 0;
        pthread_mutex_lock(&mqtt_readData_mutex); /* 上锁 */
        readData = fifoBuffListDel(&mqtt_read_BUFF, &readLen);
        if (readLen > 0)
        {
            //mqtt解析处理
            mqtt_dataProc(readData, readLen);
            free(readData);
        }
        readData = NULL;
        pthread_mutex_unlock(&mqtt_readData_mutex); /* 解锁 */
    }
    break;
    }
}

/**
 * @brief :  mqtt接收解析
 * @param {int} socketFd
 * @param {char} *recvData
 * @param {int} recvLen
 * @return {*}
 * @author: LR
 * @Date: 2022-01-18 16:38:57
 */
int mqtt_dataProc(char *readData, int readLen)
{
    if (readLen < 5)
        return RES_ERROR;

    char _msgid[100] = {0};
    char _msgType[100] = {0}; // 纪录msgType

    // 用于获取来自TCP的JSON结构
    cJSON *pR1 = NULL;
    cJSON *pR2 = NULL;
    cJSON *pR3 = NULL;
    cJSON *pR4 = NULL;
    char revPDU[512] = {0};
    if (readLen > 512)
    {
        readLen = 512;
    }
    memcpy(revPDU, readData, readLen);

    ExcepSign Ex; // 用于实现异常处理的异常标记
    Try(Ex)
    {
        pR1 = cJSON_Parse(revPDU); // json解析，获得句柄
        {
            if (pR1 == NULL)
            {
                return RES_ERROR;
            }

            pR2 = cJSON_GetObjectItem(pR1, "msgID"); // 获取msgType
            {
                if (pR2 == NULL)
                {
                    Throw(Ex, RES_ERROR);
                }
                snprintf(_msgid, sizeof(_msgid), "%s", pR2->valuestring); //提取msgType
            }

            pR2 = cJSON_GetObjectItem(pR1, "msgType"); // 获取msgType
            {
                if (pR2 == NULL)
                {
                    Throw(Ex, RES_ERROR);
                }
                snprintf(_msgType, sizeof(_msgType), "%s", pR2->valuestring); //提取msgType
            }
            if (strcmp(_msgType, "devRegAck") == 0)
            {
                pR2 = cJSON_GetObjectItem(pR1, "result"); // 获取msgType
                {
                    if (pR2 == NULL || pR2->valueint != 0)
                    {
                        Throw(Ex, RES_ERROR);
                    }
                    pR2 = cJSON_GetObjectItem(pR1, "data"); // 获取msgType
                    {
                        if (pR2 == NULL)
                        {
                            Throw(Ex, RES_ERROR);
                        }
                        if ((pR3 = cJSON_GetObjectItem(pR2, "frpsAddr")) == NULL)
                        {
                            Throw(Ex, RES_ERROR);
                        }
                        snprintf(frpCfg.server_addr, sizeof(frpCfg.server_addr), "%s", pR3->valuestring);

                        if ((pR3 = cJSON_GetObjectItem(pR2, "frpsPort")) == NULL)
                        {
                            Throw(Ex, RES_ERROR);
                        }
                        frpCfg.server_port = pR3->valueint;

                        if ((pR3 = cJSON_GetObjectItem(pR2, "frpsToken")) == NULL)
                        {
                            Throw(Ex, RES_ERROR);
                        }
                        snprintf(frpCfg.token, sizeof(frpCfg.token), "%s", pR3->valuestring);

                        if ((pR3 = cJSON_GetObjectItem(pR2, "sshPort")) == NULL)
                        {
                            Throw(Ex, RES_ERROR);
                        }
                        frpCfg.remote_port_ssh = pR3->valueint;

                        if ((pR3 = cJSON_GetObjectItem(pR2, "webPort")) == NULL)
                        {
                            Throw(Ex, RES_ERROR);
                        }
                        frpCfg.remote_port_web = pR3->valueint;

                        if ((pR3 = cJSON_GetObjectItem(pR2, "nodeRedPort")) == NULL)
                        {
                            Throw(Ex, RES_ERROR);
                        }
                        frpCfg.remote_port_nodeRed = pR3->valueint;

                        if ((pR3 = cJSON_GetObjectItem(pR2, "vpnPort")) == NULL)
                        {
                            Throw(Ex, RES_ERROR);
                        }
                        frpCfg.remote_port_vpn = pR3->valueint;

                        mqtt_reg_flag = 1;
                    }
                }
            }
            else if (strcmp(_msgType, "devRemoteCtl") == 0)
            {
                pR2 = cJSON_GetObjectItem(pR1, "data"); // 获取msgType
                {
                    if (pR2 == NULL)
                    {
                        Throw(Ex, RES_ERROR);
                    }
                    if ((pR3 = cJSON_GetObjectItem(pR2, "enable")) == NULL)
                    {
                        Throw(Ex, RES_ERROR);
                    }
                    if (pR3->valueint != ENABLE && pR3->valueint != DISABLE)
                    {
                        Throw(Ex, RES_ERROR);
                    }
                    frpCfg.enable = pR3->valueint;

                    log_i("remte cmd-->%d\n", frpCfg.enable);
                    char *p;
                    cJSON *pJsonRoot;
                    cJSON *pSubJson;
                    pJsonRoot = cJSON_CreateObject();
                    if (NULL == pJsonRoot)
                    {
                        cJSON_Delete(pR1);
                        return RES_ERROR;
                    }
                    cJSON_AddStringToObject(pJsonRoot, "msgType", "devRemoteCtlAck");
                    cJSON_AddStringToObject(pJsonRoot, "msgID", _msgid);
                    if (frp_service_ctl(frpCfg.enable) == RES_OK)
                    {
                        cJSON_AddNumberToObject(pJsonRoot, "result", 0);
                        cJSON_AddItemToObject(pJsonRoot, "data", pSubJson = cJSON_CreateObject());
                        cJSON_AddNumberToObject(pSubJson, "sshState", frpCfg.remote_ssh_result);
                        cJSON_AddNumberToObject(pSubJson, "webState", frpCfg.remote_web_result);
                        cJSON_AddNumberToObject(pSubJson, "nodeRedState", frpCfg.remote_nodeRed_result);
                        cJSON_AddNumberToObject(pSubJson, "vpnState", frpCfg.remote_vpn_result);
                    }
                    else
                    {
                        cJSON_AddNumberToObject(pJsonRoot, "result", 1);
                        cJSON_AddItemToObject(pJsonRoot, "data", pSubJson = cJSON_CreateObject());
                    }
                    p = cJSON_PrintUnformatted(pJsonRoot);
                    if (p != NULL)
                    {
                        add_mqtt_write_buff(p, strlen(p));
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
            }
            else if (strcmp(_msgType, "devHeartbeat") == 0)
            {
                pR2 = cJSON_GetObjectItem(pR1, "data"); // 获取msgType
                {
                    if (pR2 == NULL)
                    {
                        Throw(Ex, RES_ERROR);
                    }

                    frpCfg.connect_flag = 1; //frp启用标志位置位
                    //组装回复
                    char sendData[1024];
                    snprintf(sendData, sizeof(sendData), "{\"msgType\": \"devHeartbeatAck\",\"data\": {}}");
                    add_mqtt_write_buff(sendData, strlen(sendData));
                }
            }
            else
            {
                Throw(Ex, RES_ERROR);
            }
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

/**
 * @brief :  mqtt计时器
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2022-02-14 15:08:23
 */
void mqtt_timerHandle(void)
{
    mqtt_reg_tick++;
}
