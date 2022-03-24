/*
 * @Description :  
 * @FilePath: /app_code/mainRtu/src/system.c
 * @Author:  LR
 * @Date: 2021-12-02 10:09:58
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include "baseInit.h"
#include "alog_printf.h"
#include "system.h"

__sys_info sys_info;



/**
 * @brief :  复位node-red流程
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-12-03 14:49:19
 */
#define NODE_FLOW_PATH "/home/pi/.node-red/"
int reset_nodeRed_flow(void)
{
    // DIR *dir;
    // struct dirent *ptr;
    // if ((dir = opendir(NODE_FLOW_PATH)) == NULL) //打开日志的文件目录，如果没有则建立
    // {
    //     return RES_ERROR;
    // }
    // if (access(NODE_FLOW_PATH, F_OK) != 0) //如果文件不存在
    // {
    //     return RES_ERROR;
    // }
    // if (remove(NODE_FLOW_PATH) != 0)
    // {
    //     return RES_ERROR;
    // }


    system("sudo rm -rf  /home/pi/.node-red/*");

    //closedir(dir);
    return RES_OK;
}


/**
 * @brief :  系统命令处理
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-06-22 11:43:27
 */
void sysCmdMonitor(void)
{
    switch (sysBasic.cmd)
    {
    case RESET: //复位－恢复默认配置
    {
        log_i("sysCmd : SYSCMD_RESET\n");
        reset_dev_cfg(); //复位机组主板
        zh_set_ethCfg();
        if (set_dev_cfg() != RES_OK)
        {
            log_i("restore  devcfgfile error-(exit)\n");
        }
        reset_nodeRed_flow();
        sleep(1);
        sysBasic.cmd = NO_CMD;
    }
    break;
    case REBOOT: //重启系统
    {
        log_i("sysCmd : SYSCMD_RESTART\n");
        system("sudo sync");
        system("sudo reboot");
        sysBasic.cmd = NO_CMD;
        sleep(5);
    }
    break;
    case UPGRADE: //升级
    {

        
    }
    break;
    case NO_CMD:
    default:
        sysBasic.cmd = NO_CMD;
        break;
    }
}

/**
 * @brief :  按键监测处理
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-06-22 11:43:27
 */
void keyMonitor(void)
{
    //static uint32_t resetPress = 0;

    // if (zh_key_getValue(KEY0) == 0)
    // {
    //     resetPress++;
    //     if (resetPress > 25)
    //     {
    //         sysBasic.cmd = RESET;
    //     }
    // }
    // else
    // {
    //     resetPress = 0;
    // }
}

/**************************************************
*功能描述 ：led灯显示处理
*参数 ： NULL
*返回值 ： NULL
*作者 ：LR
*修改日期 ：2020年09月11日
***************************************************/
void led_display(void)
{
    switch (sys_info.led_state[0])
    {
    case LEDS_OFF: //灭
        zh_led_setValue(LED_1, LED_OFF);
        sys_info.led_1_tick = 0;
        break;
    case LEDS_ON: //亮
        zh_led_setValue(LED_1, LED_ON);
        sys_info.led_1_tick = 0;
        break;
    case LEDS_TW_SLOW: //慢闪
        if (sys_info.led_1_tick >= 6)
        {
            zh_led_setValue(LED_1, LED_ON);
        }
        if (sys_info.led_1_tick >= 12)
        {
            zh_led_setValue(LED_1, LED_OFF);
            sys_info.led_1_tick = 0;
        }
        break;
    case LEDS_TW_FAST: //快闪
        if (sys_info.led_1_tick >= 2)
        {
            zh_led_setValue(LED_1, LED_ON);
        }
        if (sys_info.led_1_tick >= 4)
        {
            zh_led_setValue(LED_1, LED_OFF);
            sys_info.led_1_tick = 0;
        }
        break;
    case LEDS_TW_LAW: //频闪
        if (sys_info.led_1_tick >= 27)
        {
            zh_led_setValue(LED_1, LED_ON);
        }
        if (sys_info.led_1_tick >= 28)
        {
            zh_led_setValue(LED_1, LED_OFF);
            sys_info.led_1_tick = 0;
        }
        break;
    default:
        zh_led_setValue(LED_1, LED_OFF);
        sys_info.led_1_tick = 0;
        break;
    }

    // switch (sys_info.led_state[1])
    // {
    // case LEDS_OFF: //灭
    //     zh_led_setValue(LED_2, LED_OFF);
    //     sys_info.led_2_tick = 0;
    //     break;
    // case LEDS_ON: //亮
    //     zh_led_setValue(LED_2, LED_ON);
    //     sys_info.led_2_tick = 0;
    //     break;
    // case LEDS_TW_SLOW: //慢闪
    //     if (sys_info.led_2_tick >= 6)
    //     {
    //         zh_led_setValue(LED_2, LED_ON);
    //     }
    //     if (sys_info.led_2_tick >= 12)
    //     {
    //         zh_led_setValue(LED_2, LED_OFF);
    //         sys_info.led_2_tick = 0;
    //     }
    //     break;
    // case LEDS_TW_FAST: //快闪
    //     if (sys_info.led_2_tick >= 2)
    //     {
    //         zh_led_setValue(LED_2, LED_ON);
    //     }
    //     if (sys_info.led_2_tick >= 4)
    //     {
    //         zh_led_setValue(LED_2, LED_OFF);
    //         sys_info.led_2_tick = 0;
    //     }
    //     break;
    // case LEDS_TW_LAW: //频闪
    //     if (sys_info.led_2_tick >= 27)
    //     {
    //         zh_led_setValue(LED_2, LED_ON);
    //     }
    //     if (sys_info.led_2_tick >= 28)
    //     {
    //         zh_led_setValue(LED_2, LED_OFF);
    //         sys_info.led_2_tick = 0;
    //     }
    //     break;
    // case LEDS_TW_DATA:
    //     if (sys_info.led_2_tick >= 1)
    //     {
    //         zh_led_setValue(LED_2, LED_ON);
    //     }
    //     if (sys_info.led_2_tick >= 2)
    //     {
    //         zh_led_setValue(LED_2, LED_OFF);
    //     }
    //     if (sys_info.led_2_tick >= 3)
    //     {
    //         zh_led_setValue(LED_2, LED_ON);
    //         sys_info.led_2_tick = 0;
    //         sys_info.led_state[1] = LEDS_OFF;
    //     }
    //     break;
    // default:
    //     zh_led_setValue(LED_2, LED_OFF);
    //     sys_info.led_2_tick = 0;
    //     break;
    // }

    // switch (sys_info.led_state[2])
    // {
    // case LEDS_OFF: //灭
    //     zh_led_setValue(LED_3, LED_OFF);
    //     sys_info.led_3_tick = 0;
    //     break;
    // case LEDS_ON: //亮
    //     zh_led_setValue(LED_3, LED_ON);
    //     sys_info.led_3_tick = 0;
    //     break;
    // case LEDS_TW_SLOW: //慢闪
    //     if (sys_info.led_3_tick >= 6)
    //     {
    //         zh_led_setValue(LED_3, LED_ON);
    //     }
    //     if (sys_info.led_3_tick >= 12)
    //     {
    //         zh_led_setValue(LED_3, LED_OFF);
    //         sys_info.led_3_tick = 0;
    //     }
    //     break;
    // case LEDS_TW_FAST: //快闪
    //     if (sys_info.led_3_tick >= 2)
    //     {
    //         zh_led_setValue(LED_3, LED_ON);
    //     }
    //     if (sys_info.led_3_tick >= 4)
    //     {
    //         zh_led_setValue(LED_3, LED_OFF);
    //         sys_info.led_3_tick = 0;
    //     }
    //     break;
    // case LEDS_TW_LAW: //频闪
    //     if (sys_info.led_3_tick >= 27)
    //     {
    //         zh_led_setValue(LED_3, LED_ON);
    //     }
    //     if (sys_info.led_3_tick >= 28)
    //     {
    //         zh_led_setValue(LED_3, LED_OFF);
    //         sys_info.led_3_tick = 0;
    //     }
    //     break;
    // default:
    //     zh_led_setValue(LED_3, LED_OFF);
    //     sys_info.led_3_tick = 0;
    //     break;
    // }


}

/**
 * @brief :  看门狗-喂狗
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-06-04 15:56:16
 */
void wdg_handle(void)
{
    if (sys_info.wdg_tick > 100) /* 10s */
    {
        zh_wdg_feed();
        sys_info.wdg_tick = 0;
    }
}

/**************************************************
*功能描述 ：计数器处理 --100ms
*参数 ： NULL
*返回值 ： NULL
*作者 ：LR
*修改日期 ：2020年09月11日
***************************************************/
void sys_timerHandle(void)
{
    sys_info.led_1_tick++;
    sys_info.led_2_tick++;
    sys_info.led_3_tick++;
    sys_info.wdg_tick++;
}

/* ***************************************************
 * 功能：数据打印-16进制
 * 参数：void
 * 返回：当前时间毫秒值
 * 作者 ：LR
 * 修改日期 ：2020年08月12日  
***************************************************/
void printf_hexData(char *dataBuff, unsigned int dataLen)
{
    int i = 0;
    printf("HexData Len %d:", dataLen);
    for (i = 0; i < dataLen; i++)
    {
        printf("%02X ", dataBuff[i]);
    }
    printf("\n");
}



/**
 * @brief :  读出当前的时间戳
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-06-22 16:17:35
 */
long get_sys_timestamp(void)
{

    time_t timep;
    struct tm *p;
    time(&timep);
    p = localtime(&timep); //获取当前系统时间

    return mktime(p);
}




/**
 * @brief :  查找杀死占用指定端口的进程
 * @param {int} port
 * @return {*}
 * @author: LR
 * @Date: 2022-02-16 11:34:58
 */
int kill_process_by_port(int port)
{
    FILE *pstr;
    char cmd[128], buff[512], *p;
    int pidnum;
    pid_t pID;
    int ret = RES_ERROR;

    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "sudo netstat -nlp | grep :%d | awk '{print $7}' | awk -F\"/\" '{print $1}'", port);
    pstr = popen(cmd, "r");
    if (pstr == NULL)
    {
        return RES_ERROR;
    }

    if ((fgets(buff, 512, pstr)) != NULL)
    {
        pidnum = atoi(buff);
        if ((pidnum - 1) == 0)
        {
            ret = RES_ERROR;
        }
        else
        {
            ret = RES_OK;
        }
    }
    pclose(pstr);

    printf("占用端口-pidnum: %d\n", pidnum);
    pID = (pid_t)pidnum;
    ret = kill(pID, SIGKILL);

    return ret;
}


/**
 * @brief :  网络发送
 * @param {int} client_socket
 * @param {uint8_t} *dataBuff
 * @param {int} dataLen
 * @return {*}
 * @author: LR
 * @Date: 2021-12-06 17:59:28
 */
int send_localServer_data(int client_socket, uint8_t *dataBuff, int dataLen)
{
    if (client_socket < 0 || dataLen <= 0)
        return RES_ERROR;

    if (send(client_socket, dataBuff, dataLen, MSG_NOSIGNAL) < 0)
    {
        return RES_ERROR;
    }
    return RES_OK;
}
