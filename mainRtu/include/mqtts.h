


/*
 * @Description :  
 * @FilePath: /app_code/mainRtu/include/mqtts.h
 * @Author:  LR
 * @Date: 2022-01-17 16:47:32
 */

#ifndef __MQTTS_H
#define __MQTTS_H

typedef struct
{
    char addr[128];
    int port;
    char clientid[128];
    char username[128];
    char password[128];
    char pubTopic[128];
    char subTopic[128];
    char trustStorePath[128];
    char keyStorePath[128];
    char privateKeyPath[128];
} mqttBasic_T;



/**
 * @brief :  mqttc处理状态机
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-07-19 16:51:36
 */
void mqtt_state_proc(void);


/**
 * @brief :  mqtt计时器
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2022-02-14 15:08:23
 */
void mqtt_timerHandle(void);



#endif