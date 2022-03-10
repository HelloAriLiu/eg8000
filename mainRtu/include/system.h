/*
 * @Author: your name
 * @Date: 2021-06-08 17:40:41
 * @LastEditTime: 2022-02-15 11:56:36
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /app_code/mainRtu/include/system.h
 */
#ifndef __SYSTEM_H
#define __SYSTEM_H
#include "../lib/include/zh_base.h"

typedef enum
{
    LEDS_OFF = 0, /* 常灭 */
    LEDS_ON,      /*常亮  */
    LEDS_TW_SLOW, /* 慢闪 */
    LEDS_TW_FAST, /* 快闪 */
    LEDS_TW_LAW,  /* 频闪 */
    LEDS_TW_DATA, /* 数据 闪*/
} LED_STATE;

typedef struct
{
    unsigned int upsNewState;
    unsigned int upsOldState;

    char led_state[3];   /* led1状态 */
    uint32_t led_1_tick; /* led1计数器 */
    uint32_t led_2_tick;
    uint32_t led_3_tick;
    uint32_t wdg_tick; /* 看门狗定时 */
} __sys_info;
extern __sys_info sys_info;




/**
 * @brief :  系统命令处理
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-06-22 11:43:27
 */
void sysCmdMonitor(void);

/**
 * @brief :  按键监测处理
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-06-22 11:43:27
 */
void keyMonitor(void);




/**************************************************
*功能描述 ：led灯显示处理
*参数 ： NULL
*返回值 ： NULL
*作者 ：LR
*修改日期 ：2020年09月11日
***************************************************/
void led_display(void);


/**
 * @brief :  看门狗-喂狗
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-06-04 15:56:16
 */
void wdg_handle(void);


/**
 * @brief :  读出当前的时间戳
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-06-22 16:17:35
 */
long get_sys_timestamp(void);

/**************************************************
*功能描述 ：计数器处理 --100ms
*参数 ： NULL
*返回值 ： NULL
*作者 ：LR
*修改日期 ：2020年09月11日
***************************************************/
void sys_timerHandle(void);





#endif
