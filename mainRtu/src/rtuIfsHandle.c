#include "baseInit.h"
#include "zh_gpio.h"
#include "zh_aio.h"
#include "usartHandle.h"
#include "cJSON.h"
#include "localServer_di.h"
#include "localServer_ups.h"
#include "rtuIfsHandle.h"
#include "system.h"

ifs_data_T ifs_data;

/***************************************************
 * 功能：接口初始化
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
***************************************************/
void rtuIfs_init(void)
{
    memset(&ifs_data, 0, sizeof(ifs_data_T));
    int i = 0;
    for (i = 0; i < DOUT_NUM; i++)
    {
        ifs_data.doutNewState[i] = PINLEVEL_X;
    }

    for (i = 0; i < DIN_NUM; i++)
    {
        ifs_data.dinOldState[i] = PINLEVEL_X;
    }

    for (i = 0; i < AO_NUM; i++)
    {
        ifs_data.aoutNewValue[i] = 0;
    }
}

/***************************************************
 * 功能：AIN处理
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
***************************************************/
void ainHandle(void)
{
    Struct_Ain ainvalue;
    if (zh_ain_getValue(&ainvalue) == RES_OK)
    {
        int i = 0;
        for (i = 0; i < AI_NUM; i++)
        {
            ifs_data.ainValue[i] = ainvalue.ai_value[i];
        }
    }

    //ifs_data.ain_tick = 0;
}
/***************************************************
 * 功能：AO处理
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
***************************************************/
void aoHandle(void)
{
    int i = 0;
    for (i = 0; i < AO_NUM; i++)
    {
        if (ifs_data.aoutNewValue[i] != 100)
        {
            //如果状态需要
            if (zh_aout_setValue(i, ifs_data.aoutNewValue[i]) == RES_OK)
                ifs_data.aoutOldValue[i] = ifs_data.aoutNewValue[i]; //状态置位

            ifs_data.aoutNewValue[i] = 100;
        }
    }
}
/***************************************************
 * 功能：DIN处理
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日
***************************************************/
void dinHandle(void)
{
    int i = 0;
    // if (ifs_flag.din_tick > 10) /* 1s采集频率 */
    {
        Struct_DinLevel dinLevel;
        if (zh_din_get(&dinLevel) == RES_OK)
        {
            for (i = 0; i < DIN_NUM; i++)
            {
                if (dinLevel.DIN_Level[i] == PINLEVEL_HIGH)
                {
                    ifs_data.dinNewState[i] = PINLEVEL_LOW;
                }
                else
                {
                    ifs_data.dinNewState[i] = PINLEVEL_HIGH;
                }
            }
        }
        else
        {
            return;
        }
        ifs_data.din_tick = 0;
    }

    //协议组装
    char *p = NULL;
    cJSON *pW1st = {NULL};
    cJSON *pW2 = {NULL};

    //bool di_chenge = false;
    for (i = 0; i < DIN_NUM; i++)
    {
        if (ifs_data.dinOldState[i] != ifs_data.dinNewState[i])
        {
            //di_chenge = true;
            ifs_data.dinOldState[i] = ifs_data.dinNewState[i];
            if ((pW1st = cJSON_CreateObject()) == NULL) // 创建一个JSON
            {
                log_e(" cJSON_CreateObject())==NULL\n");
                cJSON_Delete(pW1st);
                continue;
            }
            cJSON_AddStringToObject(pW1st, "msgType", "rptDinChange"); // 组建回复包
            cJSON_AddItemToObject(pW1st, "data", pW2 = cJSON_CreateObject());
            {
                cJSON_AddNumberToObject(pW2, "index", i + 1);
                cJSON_AddNumberToObject(pW2, "value", ifs_data.dinOldState[i]);
            }

            p = cJSON_PrintUnformatted(pW1st); //未序列化输出
            cJSON_Delete(pW1st);
            for (i = 0; i < DI_LISTEN_MAX_CLIENT_NUM; i++) // 给DI节点上报
            {
                if (localServer_di_info.socketIsLive[i] == 1)
                {
                    send_localServer_di_data(localServer_di_info.socketConGroup[i], (uint8_t *)p, strlen(p));
                }
            }
            free(p);
        }
    }
}

/***************************************************
 * 功能：DOUT处理
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
***************************************************/
void doutHandle(void)
{
    Enum_DoutName doutname;
    int i = 0;
    for (i = 0; i < DOUT_NUM; i++)
    {
        if (ifs_data.doutNewState[i] != PINLEVEL_X)
        {
            //如果状态需要更新
            switch (ifs_data.doutNewState[i])
            {
            case PINLEVEL_HIGH: /* OFF */
                doutname = i;
                if (zh_dout_setLevel(doutname, PINLEVEL_LOW) == RES_OK)
                    ifs_data.doutOldState[i] = ifs_data.doutNewState[i]; //状态置位
                break;
            case PINLEVEL_LOW: /* ON */
                doutname = i;
                if (zh_dout_setLevel(doutname, PINLEVEL_HIGH) == RES_OK)
                    ifs_data.doutOldState[i] = ifs_data.doutNewState[i]; //状态置位
                break;
            default:
                break;
            }
            ifs_data.doutNewState[i] = PINLEVEL_X;
        }
    }
}
/***************************************************
 * 功能：获取DOUT状态
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
***************************************************/
void get_doutState(void)
{
    Struct_DoutLevel doutLevel;
    int i = 0;
    for (i = 0; i < DOUT_NUM; i++)
    {
        if (zh_dout_get(&doutLevel) == RES_OK)
        {
            if (doutLevel.DOUT_Level[i] == PINLEVEL_HIGH)
            {
                ifs_data.doutOldState[i] = PINLEVEL_LOW;
            }
            else
            {
                ifs_data.doutOldState[i] = PINLEVEL_HIGH;
            }
        }
        else
        {
        }
    }
}

/**
 * @brief :  UPS
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-10-25 11:40:16
 */
void upsHandle(void)
{
    int i = 0;
    //TODO：获取ups信号
    if (zh_vin_getValue() == 1) //监测到断电后
    {
        usleep(10000);              //防抖
        if (zh_vin_getValue() == 1) //监测到断电后关闭膨胀阀
        {
            sys_info.upsNewState = 1;
        }
    }
    else
    {
        sys_info.upsNewState = 0;
    }
    bool ups_change = false;
    if (sys_info.upsOldState != sys_info.upsNewState)
    {
        ups_change = true;
        sys_info.upsOldState = sys_info.upsNewState;
    }
    if (ups_change == true)
    {
        //协议组装
        char *p = NULL;
        cJSON *pW1st = {NULL};
        cJSON *pW2 = {NULL};
        pW1st = cJSON_CreateObject(); // 创建一个JSON
        if (pW1st == NULL)
        {
            log_e("setDevcfg :cJSON_CreateObject  fail\n");
            cJSON_Delete(pW1st);
            return;
        }
        cJSON_AddStringToObject(pW1st, "msgType", "rptEXPChange"); // 组建回复包
        cJSON_AddItemToObject(pW1st, "data", pW2 = cJSON_CreateObject());
        {
            cJSON_AddNumberToObject(pW2, "value", sys_info.upsNewState);
        }

        p = cJSON_PrintUnformatted(pW1st); //未序列化输出
        cJSON_Delete(pW1st);
        if (p != NULL)
        {
            for (i = 0; i < UPS_LISTEN_MAX_CLIENT_NUM; i++) // 给DI节点上报
            {
                if (localServer_ups_info.socketIsLive[i] == 1)
                {
                    send_localServer_di_data(localServer_ups_info.socketConGroup[i], (uint8_t *)p, strlen(p));
                }
            }
            free(p);
        }
    }
}

/***************************************************
 * 功能：rtu接口定时器-100ms
 * 参数：void
 * 返回：void
 * 作者 ：LR
 * 修改日期 ：2020年08月12日 
***************************************************/
void ifs_timerHandle(void)
{
    ifs_data.ain_tick++;
    ifs_data.din_tick++;
}
