/*
 * @Description :  
 * @FilePath: /app_code/mainRtu/include/rtuIfsHandle.h
 * @Author:  LR
 * @Date: 2021-09-28 10:36:30
 */
#ifndef _RTUIFSHANDLE_H
#define _RTUIFSHANDLE_H
#include "../lib/include/zh_ain.h"
#include "../lib/include/zh_gpio.h"

typedef struct
{
    float ainValue[AI_NUM];

    float aoutNewValue[DOUT_NUM];
    float aoutOldValue[DOUT_NUM];


    unsigned int dinNewState[DIN_NUM];
    unsigned int dinOldState[DIN_NUM];

    unsigned int doutNewState[DOUT_NUM];
    unsigned int doutOldState[DOUT_NUM];

    unsigned int ain_tick;
    unsigned int din_tick;
} ifs_data_T;
extern ifs_data_T ifs_data;


#define AO_NUM AI_NUM

/***************************************************
 * 功能：接口初始化
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
***************************************************/
void rtuIfs_init(void);
/***************************************************
 * 功能：AIN处理
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
***************************************************/
void ainHandle(void);

/***************************************************
 * 功能：AO处理
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
***************************************************/
void aoHandle(void);

/***************************************************
 * 功能：DIN处理
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日
***************************************************/
void dinHandle(void);

/***************************************************
 * 功能：DOUT处理
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
***************************************************/
void doutHandle(void);

/**
 * @brief :  UPS
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-10-25 11:40:16
 */
void upsHandle(void);

/***************************************************
 * 功能：获取DOUT状态
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
***************************************************/
void get_doutState(void);

/***************************************************
 * 功能：rtu接口定时器-100ms
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
***************************************************/
void ifs_timerHandle(void);

#endif
