/*
 * @Description :  
 * @FilePath: /app_code/bsp_driver/include/zh_pcf8563tDriver.h
 * @Author:  LR
 * @Date: 2022-03-16 04:21:28
 */


#ifndef   PCF8563T_DRIVER_H
#define   PCF8563T_DRIVER_H
#include <time.h>


/******************************************************************************************
 *功能描述 ：获取pcf8563t原始时间
 *参数 tm *time
 * 返回：RES_OK 1; ERROR -1;
 *作者 ：LR
 *修改日期 ：2020年07月6日 
*******************************************************************************************/
int pcf8563t_getTime(struct tm *time);

/******************************************************************************************
 *功能描述 ：设置DS3231原始时间
 *参数 ：null
 * 返回：RES_OK 1; ERROR -1;
 *作者 ：LR
 *修改日期 ：2020年07月6日 
*******************************************************************************************/
int pcf8563t_setTime(struct tm *time);




#endif