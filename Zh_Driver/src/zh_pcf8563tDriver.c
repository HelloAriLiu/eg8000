/*
 * @Description :  
 * @FilePath: /app_code/Zh_Driver/src/zh_pcf8563tDriver.c
 * @Author:  LR
 * @Date: 2022-03-16 04:21:02
 */

#include <stdint.h>
#include "zh_i2c.h"
#include "zh_base.h"
#include "zh_pcf8563tDriver.h"

/*
*******************************************************************************************
*                               CONFIGURE 主配置
*******************************************************************************************
*/

#define PCF8563T_CALLADDR 0x51 // 主叫地址
#define PCF8563T_DEVPATH "/dev/i2c-10"

// only support year from 2000 to 2199
#define BASEYEAR 2000

/******************************************************************************************
 *功能描述 ：获取pcf8563t原始时间
 *参数 tm *time
 * 返回：RES_OK 1; ERROR -1;
 *作者 ：LR
 *修改日期 ：2020年07月6日 
*******************************************************************************************/
int pcf8563t_getTime(struct tm *time)
{
    i2c_t i2c;
    /* Open the i2c-0 bus */
    if (i2c_open(&i2c, PCF8563T_DEVPATH) < 0)
    {
        return RES_ERROR;
    }

    /*     读取寄存器值       */
    uint8_t msg_addr[1] = {0x02}; //RegAddr_Sec
    uint8_t msg_data[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    struct i2c_msg msgs_data[2] =
        {
            /* Write addr  flags = 0:写   */
            {.addr = PCF8563T_CALLADDR, .flags = 0, .len = 1, .buf = msg_addr},
            /* read data    flags = 1:写   */
            {.addr = PCF8563T_CALLADDR, .flags = I2C_M_RD, .len = 7, .buf = msg_data},
        };

    if (i2c_transfer(&i2c, msgs_data, 2) < 0)
    {
        goto REEOR_IIC;
    }

    time->tm_sec = ((msg_data[0]>>4) &0x07) *10 +  ((msg_data[0]) &0x0F);
    time->tm_min = ((msg_data[1]>>4) &0x07) *10 +  ((msg_data[1]) &0x0F);
    time->tm_hour = ((msg_data[2]>>4) &0x03) *10 +  ((msg_data[2]) &0x0F);
     time->tm_mday =   ((msg_data[3]>>4) &0x03) *10 +  ((msg_data[3]) &0x0F);
    time->tm_wday = msg_data[4] &0x07;
    time->tm_mon = ((msg_data[5]>>4) & 0x01) *10 +  ((msg_data[5]) &0x0F)- 1;
    time->tm_year =((msg_data[6]>>4) &0x0F) *10 +  ((msg_data[6]) &0x0F)+ 100;

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
int pcf8563t_setTime(struct tm *time)
{
    uint8_t msg_data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    msg_data[0] = 0x02; //RegAddr_Sec

    uint8_t ten_temp=0x00,unit_temp =0x00 ;
    ten_temp =  (uint8_t)(time->tm_sec) /10;
    unit_temp =  (uint8_t)(time->tm_sec) %10;
    msg_data[1] = ten_temp<<4 | unit_temp;


    ten_temp =  (uint8_t)(time->tm_min) /10;
    unit_temp =  (uint8_t)(time->tm_min) %10;
    msg_data[2] = ten_temp<<4 | unit_temp;


    ten_temp =  (uint8_t)(time->tm_hour) /10;
    unit_temp =  (uint8_t)(time->tm_hour) %10;
    msg_data[3] = ten_temp<<4 | unit_temp;

    ten_temp =  (uint8_t)(time->tm_mday) /10;
    unit_temp =  (uint8_t)(time->tm_mday) %10;
    msg_data[4] = ten_temp<<4 | unit_temp;

    unit_temp =  (uint8_t)(time->tm_wday);
    msg_data[5] =  unit_temp;

    ten_temp =  (uint8_t)(time->tm_mon+1) /10;
    unit_temp =  (uint8_t)(time->tm_mon+1) %10;
    msg_data[6] = ten_temp<<4 | unit_temp;

    ten_temp =  (uint8_t)(time->tm_year - 100) /10;
    unit_temp =  (uint8_t)(time->tm_year - 100) %10;
    msg_data[7] = ten_temp<<4 | unit_temp;


    i2c_t i2c;
    /* Open the i2c-0 bus */
    if (i2c_open(&i2c, PCF8563T_DEVPATH) < 0)
    {
        //printf("i2c_open error\n");
        return RES_ERROR;
    }

    struct i2c_msg msgs_data[1] =
        {
            /* Write data    flags = 0:写   */
            {.addr = PCF8563T_CALLADDR, .flags = 0, .len = 8, .buf = msg_data},
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
