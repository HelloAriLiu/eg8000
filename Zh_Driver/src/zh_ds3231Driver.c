#include <stdint.h>
#include <time.h>

#include "zh_ds3231Driver.h"
#include "zh_i2c.h"
#include "zh_base.h"

// only support year from 2000 to 2199
#define DS3231_BASEYEAR 2000

const int16_t TimeExt_lpdays[] = {-1, 30, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
const int16_t TimeExt_days[] = {-1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364};

/******************************************************************************************
*功能描述 ：RTC时间->系统时间
*参数 ：null
*返回值 ： NULL
*作者 ：LR
*修改日期 ：2020年07月6日 
*******************************************************************************************/
void DS3231TimeToSystemTime(struct tm *dst, const TIME_STRUCT *src)
{
    uint16_t tmp;
    const int16_t *mdays;
    if (dst == NULL || src == NULL)
        return;
    dst->tm_sec = src->sec.sec_sd + src->sec.sec_td * 10;
    dst->tm_min = src->min.min_sd + src->min.min_td * 10;
    if (src->hour.SYS12.is12sys)
    {
        tmp = src->hour.SYS12.hour_sd + src->hour.SYS12.hour_td * 10;
        tmp = (tmp == 12) ? 0 : tmp; // 12:30 am = 00:30(24h);    12:30pm = 12:30(24h)
        dst->tm_hour = (src->hour.SYS12.isPM) ? tmp + 12 : tmp;
    }
    else
    {
        dst->tm_hour = src->hour.SYS24.hour_sd + src->hour.SYS24.hour_td * 10;
    }
    dst->tm_mday = src->date.date_sd + src->date.date_td * 10;
    dst->tm_mon = src->cmon.mon_sd + src->cmon.mon_td * 10 - 1;
    dst->tm_year = src->year.year_sd + src->year.year_td * 10 + (DS3231_BASEYEAR - 1900) + src->cmon.century * 100;
    dst->tm_wday = src->day.day - 1; // treat TIME_STRUCT.day 1 as Sunday
    if (dst->tm_mon < 12)
    {
        tmp = dst->tm_year + 1900;
        // mdays = (IS_LEAP_YEAR(tmp)) ? TimeExt_lpdays : TimeExt_days;
        // dst->tm_yday = mdays[dst->tm_mon] + dst->tm_mday;
    }
    dst->tm_isdst = 0;
    //dst->__tm_gmtoff = -1;
}
/******************************************************************************************
*功能描述 ：RTC时间<-系统时间
*参数 ：TIME_STRUCT *dst, const struct tm *src
*返回值 ： NULL
*作者 ：LR
*修改日期 ：2020年07月6日 
*******************************************************************************************/
void SystemTimeToDS3231Time(TIME_STRUCT *dst, const struct tm *src)
{
    uint16_t tmp;
    if (dst == NULL || src == NULL)
        return;
    if (src->tm_year < (DS3231_BASEYEAR - 1900))
        return;
    dst->sec.sec_sd = src->tm_sec % 10;
    dst->sec.sec_td = src->tm_sec / 10;
    dst->min.min_sd = src->tm_min % 10;
    dst->min.min_td = src->tm_min / 10;
    dst->hour.SYS12.is12sys = 0; // use 24 hours system
    dst->hour.SYS24.hour_sd = src->tm_hour % 10;
    dst->hour.SYS24.hour_td = src->tm_hour / 10;
    dst->date.date_sd = src->tm_mday % 10;
    dst->date.date_td = src->tm_mday / 10;
    dst->day.day = src->tm_wday + 1;
    tmp = src->tm_mon + 1;
    dst->cmon.mon_sd = tmp % 10;
    dst->cmon.mon_td = tmp / 10;
    tmp = src->tm_year - (DS3231_BASEYEAR - 1900);
    dst->cmon.century = tmp / 100 & 1;
    dst->year.year_sd = tmp % 10;
    tmp /= 10;
    dst->year.year_td = tmp % 10;
}

/******************************************************************************************
*功能描述 ：RTC时间检查
*参数 ：null
*返回值 ： NULL
*作者 ：LR
*修改日期 ：2020年07月6日  
*******************************************************************************************/
int DS3231TimeValid(const TIME_STRUCT *time)
{
    uint16_t tmp;
    if (time == NULL)
        return FALSE;
    // BCD check(with some range check)
    if (time->sec.sec_sd > 9 || time->sec.sec_td > 5 ||
        time->min.min_sd > 9 || time->min.min_td > 5 ||
        time->hour.SYS24.hour_sd > 9 ||
        time->day.day == 0 ||
        time->date.date_sd > 9 ||
        time->cmon.mon_sd > 9 ||
        time->year.year_sd > 9 || time->year.year_td > 9)
        return FALSE;
    // range check
    if (time->hour.SYS12.is12sys)
    {
        tmp = time->hour.SYS12.hour_sd + time->hour.SYS12.hour_td * 10;
        if (tmp < 1 || tmp > 12)
            return FALSE;
    }
    else
    {
        tmp = time->hour.SYS24.hour_sd + time->hour.SYS24.hour_td * 10;
        if (tmp > 23)
            return FALSE;
    }
    tmp = time->cmon.mon_sd + time->cmon.mon_td * 10;
    if (tmp == 0 || tmp > 12)
        return FALSE;
    tmp = time->date.date_sd + time->date.date_td * 10;
    if (tmp == 0 || tmp > 31) // 其实更严格的检查的话得判断当前月份
        return FALSE;
    return TRUE;
}

/******************************************************************************************
 *功能描述 ：获取DS3231原始时间
 *参数 TIME_STRUCT *time
 * 返回：RES_OK 1; ERROR -1;
 *作者 ：LR
 *修改日期 ：2020年07月6日 
*******************************************************************************************/
int DS3231_getTime(TIME_STRUCT *time)
{
    i2c_t i2c;
 

    /* Open the i2c-0 bus */
    if (i2c_open(&i2c, DS3231_DEVPATH) < 0)
    {
        return RES_ERROR;
    }

    /*     读取寄存器值       */
    uint8_t msg_addr[1] = {0x00}; //RegAddr_Sec
    uint8_t msg_data[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    struct i2c_msg msgs_data[2] =
        {
            /* Write addr  flags = 0:写   */
            {.addr = DS3231_CALLADDR, .flags = 0, .len = 1, .buf = msg_addr},
            /* read data    flags = 1:写   */
            {.addr = DS3231_CALLADDR, .flags = I2C_M_RD, .len = 7, .buf = msg_data},
        };

    if (i2c_transfer(&i2c, msgs_data, 2) < 0)
    {
        goto REEOR_IIC;
    }

    time->sec.sec_td = (msg_data[0] >> 4) & 0x0F;
    time->sec.sec_sd = msg_data[0] & 0x0F;

    time->min.min_td = (msg_data[1] >> 4) & 0x0F;
    time->min.min_sd = msg_data[1] & 0x0F;

    time->hour.SYS24.is12sys = 0;
    if ((msg_data[2] >> 5 & 0x01) == 1)
    {
        time->hour.SYS24.hour_td = 2;
    }
    else if ((msg_data[2] >> 4 & 0x01) == 1)
    {
        time->hour.SYS24.hour_td = 1;
    }
    else
    {
        time->hour.SYS24.hour_td = 0;
    }
    // int td2 = msg_data[2] >> 5 & 0x01;
    // int td1 = msg_data[2] >> 4 & 0x01;
    // printf("hour bit5:%d   bit4:%d  \n",td2,td1);
    //time->hour.SYS24.hour_td = td1 + td2;
    time->hour.SYS24.hour_sd = msg_data[2] & 0x0F;

    time->day.day = msg_data[3] & 0x0F;

    time->date.date_td = (msg_data[4] >> 4) & 0x0F;
    time->date.date_sd = msg_data[4] & 0x0F;

    time->cmon.century = msg_data[5] >> 7 & 0x01;
    time->cmon.mon_td = (msg_data[5] >> 4) & 0x01;
    time->cmon.mon_sd = msg_data[5] & 0x0F;

    time->year.year_td = (msg_data[6] >> 4) & 0x0F;
    time->year.year_sd = msg_data[6] & 0x0F;

    i2c_close(&i2c);
    return RES_OK;

REEOR_IIC:
{
    i2c_close(&i2c);
    return RES_ERROR;
}
}

/******************************************************************************************
 *功能描述 ：设置DS3231原始时间
 *参数 ：null
 * 返回：RES_OK 1; ERROR -1;
 *作者 ：LR
 *修改日期 ：2020年07月6日 
*******************************************************************************************/
int DS3231_setTime(TIME_STRUCT *time)
{

    if (DS3231TimeValid(time) == FALSE)
        return RES_ERROR;

    //uint8_t msg_addr[1] = {0x00}; //RegAddr_Sec
    uint8_t msg_data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    msg_data[0] = 0x00; //RegAddr_Sec

    msg_data[1] = (uint8_t)(time->sec.sec_td << 4 | time->sec.sec_sd);

    msg_data[2] = (uint8_t)(time->min.min_td << 4 | time->min.min_sd);

    uint8_t hour_temp1 = 0x00;
    if (time->hour.SYS24.hour_td == 2)
    {
        hour_temp1 = 0x20;
    }
    else if (time->hour.SYS24.hour_td == 1)
    {
        hour_temp1 = 0x10;
    }
    else
    {
        hour_temp1 = 0x00;
    }

    msg_data[3] = (uint8_t)(hour_temp1 | time->hour.SYS24.hour_sd);

    msg_data[4] = (uint8_t)time->day.day;

    msg_data[5] = (uint8_t)(time->date.date_td << 4 | time->date.date_sd);

    msg_data[6] = (uint8_t)(time->cmon.mon_td << 4 | time->cmon.mon_sd);
    if (time->cmon.century == 1)
    {
        msg_data[6] |= 0x80;
    }

    msg_data[7] = (uint8_t)(time->year.year_td << 4 | time->year.year_sd);

    // int k=0;
    // printf("msg_data :");
    // for(k=0;k<7;k++)
    // {
    //     printf("%02X",msg_data[k]);
    // }
    // printf("\n");

    i2c_t i2c;

    /* Open the i2c-0 bus */
    if (i2c_open(&i2c, DS3231_DEVPATH) < 0)
    {
        //printf("i2c_open error\n");
        return RES_ERROR;
    }

    struct i2c_msg msgs_data[1] =
        {
            /* Write data    flags = 0:写   */
            {.addr = DS3231_CALLADDR, .flags = 0, .len = 8, .buf = msg_data},
        };

    if (i2c_transfer(&i2c, msgs_data, 1) < 0)
    {
        //printf("i2c_transfer error\n");
        goto REEOR_IIC;
    }

    i2c_close(&i2c);
    return RES_OK;

REEOR_IIC:
{
    i2c_close(&i2c);
    return RES_ERROR;
}
}
