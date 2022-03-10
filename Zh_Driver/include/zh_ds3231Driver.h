/*
*******************************************************************************************
*
*
*                                   DS3231 DRIVER MODULE
*                                      DS3231驱动模块
*
* File : DS3231Driver.h
* By   : Ari
* Date:  2018/05/14
* version: V1.0
* History: 2018/05/14  V1.0   the prototype
*********************************************************************************************
*/

#ifndef   DS3231_DRIVER_H
#define   DS3231_DRIVER_H

#include <time.h>


/*
*******************************************************************************************
*                               CONFIGURE 主配置
*******************************************************************************************
*/
#define DS3231_CALLADDR     0x68      // DS32331的主叫地址
#define  DS3231_DEVPATH   "/dev/i2c-10"

/*
********************************************************************************************
*                                   MISCELLANEOUS
********************************************************************************************
*/

#ifndef  FALSE
#define  FALSE    0
#endif

#ifndef  TRUE
#define  TRUE     1
#endif

/*
******************************************************************************************
*                                    CONSTANT
******************************************************************************************
*/
// address of register
typedef enum{
  RegAddr_Sec,    // Seconds              00-59
  RegAddr_Min,    // Minutes              00-59
  RegAddr_Hour,   // Hours                1–12 + AM/PM 00–23
  RegAddr_Day,    // Day                  1 - 7
  RegAddr_Date,   // Data                 01-31
  RegAddr_CMon,   // Century and month    Century + 01–12
  RegAddr_Year,   // Year                 00 - 99
  RegAddr_Sec_A1, // Alarm 1 Seconds      00-59
  RegAddr_Min_A1, // Alarm 1 Minutes      00-59
  RegAddr_Hour_A1,// Alarm 1 Hours        1–12 + AM/PM 00–23
  RegAddr_Da_A1,  // Alarm 1 Day/Date     1 - 7 / 1 – 31
  RegAddr_Min_A2, // Alarm 2 Minutes      00-59
  RegAddr_Hour_A2,// Alarm 2 Hours        1–12 + AM/PM 00–23
  RegAddr_Da_A2,  // Alarm 2 Day/Date     1 - 7 / 1 – 31
  RegAddr_Control,// Control
  RegAddr_CtlStat,// Control/Status
  RegAddr_AgOfset,// Aging offset
  RegAddr_TempMSB,// MSB of Temp
  RegAddr_TempLSB,// LSB of Temp
}DS3231REG_ADDR;
#define DS3231REG_ADDR_MAX  RegAddr_TempLSB

/*
****************************************************************************************
*                                  ERROR CODES
****************************************************************************************
*/
#define DS3231_ERR_NULL   0      // if success
#define DS3231_ERR_COMM   1      // any error in communication
#define DS3231_ERR_REG    2      // wrong register 
#define DS3231_ERR_ARG    3      // if wrong argument

/*
******************************************************************************************
*                                  TYPE DEFINE
******************************************************************************************
*/

// struct of DS3231 register 
typedef struct {
  unsigned int sec_sd: 4; // ones digit of second  秒的个位
  unsigned int sec_td: 3; // tens digit of second  秒的十位
  unsigned int       : 1;
} REG_SEC;

typedef struct {
  unsigned int min_sd: 4; // ones digit of minute 分的个位
  unsigned int min_td: 3; // tens digit of minute   分的十位
  unsigned int       : 1;
} REG_MIN;

typedef union {
  struct{
    unsigned int hour_sd:4; // ones digit of hour 分的个位
    unsigned int hour_td:1; // tens digit of hour 分的个位
    unsigned int isPM   :1; // whether is pm
    unsigned int is12sys:1; // whether is 12 hours system. 
    unsigned int        :1;
  }SYS12;
  struct{
    unsigned int hour_sd:4; // ones digit of hour 分的个位
    unsigned int hour_td:2; // tens digit of hour   分的个位
    unsigned int is12sys:1; // whether is 12 hours system. 
    unsigned int        :1;
  }SYS24;
} REG_HOUR;

typedef struct {
  unsigned int day    : 3; // the day of the week 
  unsigned int        : 5;
} REG_DAY;

typedef struct {
  unsigned int date_sd: 4; // ones digit of second  秒的个位
  unsigned int date_td: 2; // tens digit of second  秒的十位
  unsigned int        : 2;
} REG_DATE;

typedef struct {
  unsigned int mon_sd: 4; // ones digit of month 月的个位
  unsigned int mon_td: 1; // tens digit of month 月的个位
  unsigned int       : 2; 
  unsigned int century:1; // hundreds digit of year
} REG_CMON;

typedef struct {
  unsigned int year_sd: 4; // ones digit of year 月的个位
  unsigned int year_td: 4; // tens digit of year 月的个位
} REG_YEAR;


typedef struct {
  unsigned int sec_sd: 4; // ones digit of second  秒的个位
  unsigned int sec_td: 3; // tens digit of second  秒的十位
  unsigned int A1M1  : 1;
} REG_SEC_A1;

typedef struct {
  unsigned int min_sd: 4; // ones digit of minute 分的个位
  unsigned int min_td: 3; // tens digit of minute   分的十位
  unsigned int A1M2  : 1;
} REG_MIN_A1;

typedef union {
  struct{
    unsigned int hour_sd:4; // ones digit of hour 分的个位
    unsigned int hour_td:1; // tens digit of hour 分的个位
    unsigned int isPM   :1; // whether is pm
    unsigned int is12sys:1; // whether is 12 hours system. 
    unsigned int A1M3   :1;
  }SYS12;
  struct{
    unsigned int hour_sd:4; // ones digit of hour 分的个位
    unsigned int hour_td:2; // tens digit of hour 分的个位
    unsigned int is12sys:1; // whether is 12 hours system. 
    unsigned int A1M3   :1;
  }SYS24;
} REG_HOUR_A1;

typedef union {
  struct{
    unsigned int day    : 4; // the day of the week 
    unsigned int        : 2;
	unsigned int isDY   : 1; // day selected
	unsigned int A1M4   : 1;
  }DAY;
  struct{
    unsigned int date_sd: 4; // ones digit of date
    unsigned int date_td: 2; // tens digit of date
	unsigned int isDY   : 1; // day selected
	unsigned int A1M4   : 1;
  }DATE;
} REG_DA_A1;

typedef struct {
  unsigned int min_sd: 4; // ones digit of minute 分的个位
  unsigned int min_td: 3; // tens digit of minute   分的十位
  unsigned int A2M2  : 1;
} REG_MIN_A2;

typedef union {
  struct{
    unsigned int hour_sd:4; // ones digit of hour 分的个位
    unsigned int hour_td:1; // tens digit of hour 分的个位
    unsigned int isPM   :1; // whether is pm
    unsigned int is12sys:1; // whether is 12 hours system. 
    unsigned int A2M3   :1;
  }SYS12;
  struct{
    unsigned int hour_sd:4; // ones digit of hour 分的个位
    unsigned int hour_td:2; // tens digit of hour 分的个位
    unsigned int is12sys:1; // whether is 12 hours system. 
    unsigned int A2M3   :1;
  }SYS24;
} REG_HOUR_A2;

typedef union {
  struct{
    unsigned int day    : 4; // the day of the week 
    unsigned int        : 2;
	unsigned int isDY   : 1; // day selected
	unsigned int A2M4   : 1;
  }DAY;
  struct{
    unsigned int date_sd: 4; // ones digit of date
    unsigned int date_td: 2; // tens digit of date
	unsigned int isDY   : 1; // day selected
	unsigned int A2M4   : 1;
  }DATE;
} REG_DA_A2;

typedef struct{
  unsigned int A1IE  : 1;
  unsigned int A2IE  : 1;
  unsigned int INTCN : 1;
  unsigned int RS1   : 1;
  unsigned int RS2   : 1;
  unsigned int CONV  : 1;
  unsigned int BBSQW : 1;
  unsigned int EOSC  : 1;
} REG_CONTROL;

typedef struct{
  unsigned int A1IF   : 1;
  unsigned int A2IF   : 1;
  unsigned int BSY    : 1;
  unsigned int EN32kHz: 1;
  unsigned int        : 3;
  unsigned int OSF    : 1;
} REG_CTLSTAT;

typedef struct{
  unsigned int DATA : 7;
  unsigned int SIGN : 1;
} REG_AGOFSET;

typedef struct{
  char DATA;
} REG_TEMPMSB;

typedef struct{
  unsigned int      : 6;
  unsigned int DATA : 2;
} REG_TEMPLSB;

// type of module
typedef struct {
  REG_SEC  sec;
  REG_MIN  min;
  REG_HOUR hour;
  REG_DAY  day;
  REG_DATE date;
  REG_CMON cmon;
  REG_YEAR year;
} TIME_STRUCT;

typedef struct{
  REG_TEMPMSB integral;      // integral part
  REG_TEMPLSB fractional;    // fractional part
} TEMP_STRUCT,*pTEMP_STRUCT;

typedef struct{
  unsigned int errcode : 4; // see DS3231_ERR_XXX
  unsigned int commerr : 4; // hold the errcode by user defined iic functions.
} DS3231_ERRCODE;



void DS3231TimeToSystemTime(struct tm *dst, const TIME_STRUCT *src);

void SystemTimeToDS3231Time(TIME_STRUCT *dst, const struct tm *src);

int DS3231TimeValid(const TIME_STRUCT *time);

int DS3231_getTime(TIME_STRUCT *time);

int DS3231_setTime(TIME_STRUCT *time);




#endif  // of DS3231_DRIVER_H
