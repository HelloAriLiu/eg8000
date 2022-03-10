/*
 * @Author: your name
 * @Date: 2021-11-26 17:54:06
 * @LastEditTime: 2022-01-10 12:48:26
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: /app_code/Zh_Driver/include/zh_base.h
 */
#ifndef __ZH_BASE_H__
#define __ZH_BASE_H__
#include <stdio.h>
#include <termios.h>
#include <linux/ioctl.h>
#include <linux/serial.h>
#include <asm-generic/ioctls.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <assert.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if.h>
#include <linux/tcp.h>
#include <linux/types.h>
#include <asm/byteorder.h>
#include <linux/ip.h>
#include <time.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <linux/input.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <wiringPi.h>
#include "zh_led.h"
#include "zh_usart.h"











/*aiao芯片*/
#define  BASE    64
#define  Address 0x48


#define RES_OK 0
#define RES_ERROR -1
#define RES_NULL ((void *)0)

typedef enum
{
    POW_OFF = 0, //下电
    POW_ON       //上电
} Enum_Pow;


typedef enum
{
    DISABLE =0 , 
    ENABLE      
} Enum_Enable;


typedef enum
{
    CPU_USAGE = 0,
    MEM_USAGE,
    FLASH_USAGE
} SYS_INFO;


typedef struct
{
    float temp;  //温度
    float humi;  //湿度
    int error_code;
} Struct_sysTh;


/**
 * @brief :  获取当前的时间戳-毫秒
 * @param {*}
 * @return {当前时间毫秒值}
 * @author: LR
 * @Date: 2021-10-29 11:26:45
 */
unsigned long get_timestamp(void);



/******************************************************************************************
 * 功能：读取芯片唯一标示SN
 * 参数：NULL
 * 返回：成功 char* SN;  错误：NULL;
 * 作者 ：LR
 *修改日期 ：2020年03月27日 
******************************************************************************************/
char *zh_get_sn(void);


/**
 * @brief :  获取node版本
 * @param 
 * @return {*}
 * @author: LR
 * @Date: 2021-12-01 17:47:10
 */
char *zh_get_nodeVersion(void);

/**
 * @brief : 获取cpu等信息
 * @param {SYS_INFO} sysInfo
 * @return {*}
 * @author: LR
 * @Date: 2021-12-02 16:56:24
 */
int zh_get_sysInfo(SYS_INFO sysInfo);

/**
 * @brief  获取IMEI号
 * @param  void
 * @return 返回IMEI指针
 * @note：注意点
 */
char *get_at_imei(void);


/**
 * @brief  获取SIMID号
 * @param  void
 * @return 返回simID指针
 * @note：注意点
 */
char *get_at_ccid(void);

/**
 * @brief  获取4G信号
 * @param  void
 * @return int 成功返回信号值0-100 失败返回RES_ERROR
 * @note：注意点
 */
int get_at_csq(void);

/**
 * @brief :  获取系统温湿度信息
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-10-29 10:25:58
 */
int zh_get_sysTH(Struct_sysTh *sysTh);


/**
 * @brief :  获取断电信息值
 * @param {*}
 * @return {正常：0； 断电：1;}
 * @author: LR
 * @Date: 2021-11-25 14:59:10
 */
int zh_vin_getValue(void);

/*******************************************************************************************
 * 功能：硬件喂狗
 * 参数：NULL
 * 返回：Int ，成功RES_OK ; 错误RES_ERROR;
 * 作者 ：LR
 *修改日期 ：2020年07月4日 
*******************************************************************************************/
int zh_iwdg_feed(void);

/******************************************************************************************
*功能描述 ：杀死指定名称的进程
*参数 ：null
*返回值 ： NULL
*作者 ：LR
*修改日期 ：2019年04月9日 
*******************************************************************************************/
int kill_process_by_name(char *processname);


/**
 * @brief :  查找指定名称的进程
 * @param {char} *processname
 * @return {*}
 * @author: LR
 * @Date: 2022-01-10 12:47:22
 */
int find_process_by_name(char *processname);

/**
 * @brief :  计算modbus CRC16
 * @param {unsigned char} *arr_buff 需要计算的数据
 * @param {int} len 数据长度
 * @return {*} crc
 * @author: LR
 * @Date: 2021-06-02 15:48:26
 */
uint16_t get_crc16(unsigned char *arr_buff, int len);


/**
 * @brief :  设置modbus CRC16
 * @param {unsigned char} *data 需要计算的数据
 * @param {int} dataLen 数据长度
 * @return {*}
 * @author: LR
 * @Date: 2021-06-02 15:49:21
 */
void set_crc16(unsigned char *data, int dataLen);


/**
 * @brief :  modbusRTU格式检查
 * @param {unsigned char} *data 需要检查的数据
 * @param {int} len 检查的数据长度
 * @return {int}  RES_OK-解析正确；RES_ERROR-解析错误；
 * @author: LR
 * @Date: 2021-06-02 15:50:00
 */
int modbus_RTU_check(unsigned char *data, int len);




#endif
