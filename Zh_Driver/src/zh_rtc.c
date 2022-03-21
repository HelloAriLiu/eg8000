#include "zh_base.h"
#include "zh_rtc.h"
#include "zh_pcf8563tDriver.h"


/***************************************************
 * 功能：获取rtc时间信息
 * 参数：tm,时间结构体  one value of Struct_tm.
 * 返回：RES_OK 1; ERROR -1;
 * BY：LR
 * Data:2020-07-06
***************************************************/
int zh_getTm_rtc(struct tm *des_tm)
{
    struct tm *p;
    p = &des_tm;

    if (pcf8563t_getTime(p) != RES_OK)
    {
        return RES_ERROR;
    }

    return RES_OK;
}

/***************************************************
 * 功能：设置rtc时间信息
 * 参数：tm,时间结构体  one value of Struct_tm.
 * 返回：RES_OK 1; ERROR -1;
 * BY：LR
 * Data:2020-07-06
***************************************************/
int zh_setTm_rtc(struct tm *src_tm)
{
    struct tm *p;
    p=&src_tm;

    if (pcf8563t_setTime(p) != RES_OK)
    {
        return RES_ERROR;
    }

    return RES_OK;
}


/***************************************************
 * 功能：系统时间同步到硬件RTC
 * 参数：NULL
 * 返回：RES_OK 1; ERROR -1;
 * BY：LR
 * Data:2020-07-06
***************************************************/
int systime_rtc_sync(void)
{
    time_t timep;
    struct tm *p;
    time(&timep);
    p = localtime(&timep); //获取当前系统时间
    if (p == NULL)
        return RES_ERROR;

    return zh_setTm_rtc(p);
}

/***************************************************
 * 功能：硬件RTC时间同步到系统
 * 参数：NULL
 * 返回：RES_OK 1; ERROR -1;
 * BY：LR
 * Data:2020-07-06
***************************************************/
int rtc_systime_sync(void)
{
    struct tm src_tm;
    struct tm *p;
    p = &src_tm;

    if (zh_getTm_rtc(p) != RES_OK)
        return RES_ERROR;

    char strTime[128];
    sprintf(strTime, "%04d-%02d-%02d %02d:%02d:%02d", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday, (p->tm_hour ) % 24, p->tm_min, p->tm_sec);

    char strtemp[128];
    sprintf(strtemp, "sudo date --s=\"%s\"", strTime);
    system(strtemp);

    return RES_OK;
}

/**
 * @brief :  读出RTC的时间戳
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-06-22 16:17:35
 */
long get_rtc_timestamp(void)
{
    struct tm des_tm;
    struct tm *p1;
    p1 = &des_tm;

    if (zh_getTm_rtc(p1) < 0)
    {
        printf("zh_getTm_rtc Error !\n");
        return RES_ERROR;
    }
    printf("Get RTC:%04d-%02d-%02d %02d:%02d:%02d\n", (1900 + p1->tm_year), (1 + p1->tm_mon), p1->tm_mday, (p1->tm_hour+8) % 24, p1->tm_min, p1->tm_sec);

    return mktime(p1);
}

/**
 * @brief :  根据时间戳设置RTC
 * @param {int} timestamp
 * @return {*}
 * @author: LR
 * @Date: 2021-06-22 16:11:43
 */
int set_rtc_timestamp(int timestamp)
{
    uint32_t t = timestamp;
    struct tm *p;
    time_t lt;
    lt = t; //time(NULL);1471421956
    p = localtime(&lt);
    if (zh_setTm_rtc(p) < 0)
    {
        printf("zh_setTm_rtc Error!\n");
        return RES_ERROR;
    }
    printf("Set RTC:%04d-%02d-%02d %02d:%02d:%02d\n", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday, (p->tm_hour+8) % 24, p->tm_min, p->tm_sec);

    rtc_systime_sync();

    return RES_OK;
}

