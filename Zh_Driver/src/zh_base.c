#include "zh_base.h"
#include <math.h>
#include "zh_i2c.h"

#define UPS_PI_1 24 //GPIO19-IN
#define KEY_PI_1 9  //GPIO3-IN
#define POW4G_PI 22 //GPIO6-OUT
#define WDI_PI 25   //GPIO26-OUT

#define LED_PI_1 29 //GPIO21-OUT
#define LED_PI_2 8  //GPIO2-OUT

#define HUBRST_PI 28 //GPIO20-OUT

/**
 * @brief :  需要在程序执行后全局调用一次
 * @param {*}
 * @return {}
 * @author: LR
 * @Date: 2021-10-29 10:17:19
 */
int zh_board_init(void)
{
    if (zh_aio_init() != RES_OK)
        return RES_ERROR;

    if (wiringPiSetup() == -1)
    {
        return RES_ERROR;
    }

    zh_io_init();
    pinMode(WDI_PI, OUTPUT);
    pinMode(KEY_PI_1, INPUT);
    pinMode(UPS_PI_1, INPUT);

    pinMode(LED_PI_1, OUTPUT);
    pinMode(LED_PI_2, OUTPUT);

    pinMode(POW4G_PI, OUTPUT);
    digitalWrite(POW4G_PI, HIGH);

    // pinMode(HUBRST_PI, OUTPUT);   此处使能USB-HUB复位用
    return RES_OK;
}

/**
 * @brief :  读取芯片唯一标示SN
 * @param {*}
 * @return {返回SN指针}
 * @author: LR
 * @Date: 2021-10-29 10:17:19
 */
char *zh_get_sn(void)
{
    system("sudo cat /proc/cpuinfo |grep \"Serial\" |awk '{print $3}' > /home/pi/edge8000/app/cpuId.log");
    usleep(50000);

    static char devSN[17];
    FILE *verptr1;
    char lineStr[128];

    verptr1 = fopen("/home/pi/edge8000/app/cpuId.log", "r");
    if (verptr1 != NULL)
    {
        if (fgets(lineStr, 128, verptr1) != NULL)
        {
            snprintf(devSN, sizeof(devSN), "%s", lineStr);
            devSN[16] = 0x00;
            devSN[strlen(devSN) - 1] = '\0';
            unsigned int i = 0;
            while (devSN[i] != 0x00 && devSN[i] != '\0')
            {
                if (devSN[i] >= 'a' && devSN[i] <= 'z')
                {
                    devSN[i] -= 32;
                }
                i++;
            }
            fclose(verptr1);
            return devSN;
        }
        fclose(verptr1);
    }

    return NULL;
}

/**
 * @brief :  获取node版本
 * @param 
 * @return {*}
 * @author: LR
 * @Date: 2021-12-01 17:47:10
 */
#define NODE_VERSION_PATH "/usr/local/lib/node_modules/node-red/node_modules/@node-red/nodes/core/version.txt"
char *zh_get_nodeVersion(void)
{
    FILE *sys_fd;
    if ((sys_fd = fopen(NODE_VERSION_PATH, "r")) == NULL)
        return RES_NULL;

    static char version_value[64];
    memset(version_value, 0, sizeof(version_value));
    if (fgets(version_value, sizeof(version_value), sys_fd) == NULL)
    {
        fclose(sys_fd);
        return RES_NULL;
    }
    fclose(sys_fd);

    version_value[strlen(version_value) - 1] = '\0';
    return version_value;
}

/**
 * @brief :  获取系统的占用情况
 * @param {SYS_INFO} sysInfo
 * @return {*}
 * @author: LR
 * @Date: 2021-12-01 17:47:10
 */
#define CPU_USAGE_INFOPATH "/home/pi/edge8000/app/cpu_usage.log"
#define MEM_USAGE_INFOPATH "/home/pi/edge8000/app/mem_usage.log"
#define FLASH_USAGE_INFOPATH "/home/pi/edge8000/app/flash_usage.log"
int zh_get_sysInfo(SYS_INFO sysInfo)
{
    FILE *sys_fd;
    switch (sysInfo)
    {
    case CPU_USAGE:
        if ((sys_fd = fopen(CPU_USAGE_INFOPATH, "r")) == NULL)
            return RES_ERROR;
        break;
    case MEM_USAGE:
        if ((sys_fd = fopen(MEM_USAGE_INFOPATH, "r")) == NULL)
            return RES_ERROR;
        break;
    case FLASH_USAGE:
        if ((sys_fd = fopen(FLASH_USAGE_INFOPATH, "r")) == NULL)
            return RES_ERROR;
        break;
    default:
        return RES_ERROR;
        break;
    }

    char usage_value[256];
    if (fgets(usage_value, sizeof(usage_value), sys_fd) == NULL)
    {
        fclose(sys_fd);
        return RES_ERROR;
    }
    fclose(sys_fd);

    return atoi(usage_value);
}

/**
 * @brief :  获取当前的时间戳-毫秒
 * @param {*}
 * @return {当前时间毫秒值}
 * @author: LR
 * @Date: 2021-10-29 11:26:45
 */
unsigned long get_timestamp(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    unsigned long millisecond = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return millisecond;
}

/**
 * @brief  获取IMEI号
 * @param  void
 * @return 返回IMEI指针
 * @note：注意点
 */
char *get_at_imei(void)
{
    char *temp = NULL;
    int at_fd = -1;
    static char cpuID[20];
    char ATtempStr[64];
    memset(ATtempStr, 0, 64);

    if ((at_fd = zh_usart_open(USART_AT)) == RES_ERROR)
        return NULL;

    if (zh_usart_init(at_fd, B_115200, DB_CS8, SB_1, PB_NONE) < 0)
    {
        zh_usart_close(at_fd);
        return NULL;
    }

    char *AT_IMEI = "AT+CGSN\r\n";
    zh_usart_send(at_fd, (unsigned char *)AT_IMEI, strlen(AT_IMEI));

    char data_buff[USART_READ_MAX_BUFF];
    int data_len;
    int i = 0;

    unsigned long timetemp = get_timestamp();
    while ((get_timestamp() - timetemp) < 600)
    {
        memset(data_buff, 0, sizeof(data_buff));
        data_len = zh_usart_read(at_fd, (unsigned char *)data_buff);
        if (data_len == 33)
        {
            if (strstr(data_buff, "OK") != NULL)
            {
                memcpy(ATtempStr, data_buff + 10, 15);
                snprintf(cpuID, sizeof(cpuID), "0%s", ATtempStr);
                //printf("%s\n", cpuID);
                zh_usart_close(at_fd);
                return cpuID;
            }
        }
        usleep(10000);
    }
    zh_usart_close(at_fd);
    return NULL;
}

/**
 * @brief  获取SIMID号
 * @param  void
 * @return 返回simID指针
 * @note：注意点
 */
char *get_at_ccid(void)
{
    char *temp = NULL;
    int at_fd = -1;
    static char simID[21];
    char ATtempStr[100];
    memset(ATtempStr, 0, 100);

    if ((at_fd = zh_usart_open(USART_AT)) == RES_ERROR)
        return NULL;

    if (zh_usart_init(at_fd, B_115200, DB_CS8, SB_1, PB_NONE) < 0)
    {
        //printf("at_fd error\n");
        zh_usart_close(at_fd);
        return NULL;
    }

    char *AT_CCID = "AT+QCCID\r\n";
    zh_usart_send(at_fd, (unsigned char *)AT_CCID, strlen(AT_CCID));

    char data_buff[USART_READ_MAX_BUFF];
    int data_len;
    int i = 0;

    unsigned long timetemp = 0;
    while (timetemp < 30)
    {
        memset(data_buff, 0, sizeof(data_buff));
        data_len = zh_usart_read(at_fd, (unsigned char *)data_buff);
        if (data_len > 20)
        {
            //printf("QCCID-ACK:%s\n",data_buff);
            if ((temp = strstr(data_buff, "+QCCID:")) != NULL)
            {
                if (strlen(temp) >= 28)
                {
                    memcpy(simID, temp + 8, 20);
                    //printf("+CCID:%s\n", simID);
                    zh_usart_close(at_fd);
                    return simID;
                }
            }
        }
        timetemp++;
        usleep(100000);
    }
    zh_usart_close(at_fd);
    return NULL;
}

/**
 * @brief  获取4G信号
 * @param  void
 * @return int 成功返回信号值0-100 失败返回RES_ERROR
 * @note：注意点
 */
int get_at_csq(void)
{
    char *temp = NULL;
    int at_fd = -1;
    char ATtempStr[100];
    memset(ATtempStr, 0, 100);

    if ((at_fd = zh_usart_open(USART_AT)) == RES_ERROR)
        return RES_ERROR;

    if (zh_usart_init(at_fd, B_115200, DB_CS8, SB_1, PB_NONE) < 0)
    {
        //printf("at_fd error\n");
        zh_usart_close(at_fd);
        return RES_ERROR;
    }

    char *AT_CCID = "AT+CSQ\r\n";
    zh_usart_send(at_fd, (unsigned char *)AT_CCID, strlen(AT_CCID));

    char data_buff[USART_READ_MAX_BUFF];
    int data_len;
    //char *temp;
    int i = 0;

    unsigned long timetemp = 0;
    while (timetemp < 30)
    {
        memset(data_buff, 0, sizeof(data_buff));
        data_len = zh_usart_read(at_fd, (unsigned char *)data_buff);
        if (data_len > 10)
        {
            //printf("CSQ-ACK:%s\n",data_buff);
            if ((temp = strstr(data_buff, "+CSQ:")) != NULL)
            {
                int csq = atoi(temp + 6);
                if (csq > 0 && csq < 32)
                {
                    //printf("+CSQ:%d%%\n", csq * 100 / 31);
                    csq = csq * 100 / 31;
                }
                else
                {
                    //printf("+CSQ:0%%\n");
                    csq = 0;
                }
                zh_usart_close(at_fd);
                return csq;
            }
        }
        timetemp++;
        usleep(100000);
    }
    zh_usart_close(at_fd);
    return RES_ERROR;
}

/**
 * @brief :  设置LED状态
 * @param {Enum_LedName} ledName
 * @param {Enum_LedState} ledState
 * @return {*}
 * @author: LR
 * @Date: 2021-10-29 11:31:02
 */
int zh_led_setValue(Enum_LedName ledName, Enum_LedState ledState)
{
    int led_pin;
    switch (ledName)
    {
    case LED_1:
        led_pin = LED_PI_1;
        break;
    case LED_2:
        led_pin = LED_PI_2;
        break;
    default:
        return RES_ERROR;
    }

    if (ledState != LED_ON && ledState != LED_OFF)
        return RES_ERROR;

    digitalWrite(led_pin, ledState);
    return RES_OK;
}

/**
 * @brief :  获取按键key值
 * @param {Enum_KeyName} keyName
 * @return {*}
 * @author: LR
 * @Date: 2021-10-29 11:32:44
 */
int zh_key_getValue(Enum_KeyName keyName)
{
    if (digitalRead(KEY_PI_1) == LOW) //按下
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief :  看门狗
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2022-03-16 03:35:24
 */
int zh_wdg_feed(void)
{
    if (digitalRead(WDI_PI) == LOW)
    {
        digitalWrite(WDI_PI, HIGH);
    }
    else
    {
        digitalWrite(WDI_PI, LOW);
    }
    return RES_OK;
}

/**
 * @brief :  获取断电信息值
 * @param {*}
 * @return {正常：0； 断电：1;}
 * @author: LR
 * @Date: 2021-11-25 14:59:10
 */
int zh_vin_getValue(void)
{
    return digitalRead(UPS_PI_1);
}

/**
 * @brief :  计算modbus CRC16
 * @param {unsigned char} *arr_buff 需要计算的数据
 * @param {int} len 数据长度
 * @return {*} crc
 * @author: LR
 * @Date: 2021-06-02 15:48:26
 */
uint16_t get_crc16(unsigned char *arr_buff, int len)
{
    uint16_t crc = 0xFFFF;
    int i, j;

    for (j = 0; j < len; j++)
    {
        crc = crc ^ *arr_buff++;
        for (i = 0; i < 8; i++)
        {
            if ((crc & 0x0001) > 0)
            {
                crc = crc >> 1;
                crc = crc ^ 0xa001;
            }
            else
                crc = crc >> 1;
        }
    }

    return (crc);
}

/**
 * @brief :  设置modbus CRC16
 * @param {unsigned char} *data 需要计算的数据
 * @param {int} dataLen 数据长度
 * @return {*}
 * @author: LR
 * @Date: 2021-06-02 15:49:21
 */
void set_crc16(unsigned char *data, int dataLen)
{
    uint16_t Crc;
    Crc = get_crc16(data, dataLen);
    data[dataLen] = Crc & 0xFF;
    data[dataLen + 1] = Crc >> 8;
}

/**
 * @brief :  modbusRTU格式检查
 * @param {unsigned char} *data 需要检查的数据
 * @param {int} len 检查的数据长度
 * @return {int}  RES_OK-解析正确；RES_ERROR-解析错误；
 * @author: LR
 * @Date: 2021-06-02 15:50:00
 */
int modbus_RTU_check(unsigned char *data, int len)
{
    uint8_t crc[2];
    uint16_t Crc;

    if (len < 4)
        return RES_ERROR;

    Crc = get_crc16(data, len - 2);
    crc[1] = Crc & 0xFF;
    crc[0] = Crc >> 8;

    if (crc[0] != data[len - 1] || crc[1] != data[len - 2])
    {
        return RES_ERROR;
    }

    data[len - 1] = 0x00;
    data[len - 2] = 0x00;

    return RES_OK;
}

/******************************************************************************************
*功能描述 ：杀死指定名称的进程
*参数 ：null
*返回值 ： NULL
*作者 ：LR
*修改日期 ：2019年04月9日 
*******************************************************************************************/
int kill_process_by_name(char *processname)
{
    char cmd[512];
    sprintf(cmd, "sudo killall %s", processname);
    system(cmd);

    // FILE *pstr;
    // char cmd[128], buff[512], *p;
    // pid_t pID;
    // int pidnum;

    // int ret = -1;
    // memset(cmd, 0, sizeof(cmd));
    // sprintf(cmd, "sudo ps -ef|grep \"%s\" ", processname);
    // pstr = popen(cmd, "r");
    // if (pstr == NULL)
    // {
    //     return 1;
    // }
    // memset(buff, 0, sizeof(buff));
    // fgets(buff, 512, pstr);
    // p = strtok(buff, " ");
    // p = strtok(NULL, " ");
    // pclose(pstr);
    // if (p == NULL)
    // {
    //     return -1;
    // }
    // if (strlen(p) == 0)
    // {
    //     return -1;
    // }
    // if ((pidnum = atoi(p)) == 0)
    // {
    //     return -1;
    // }
    // //printf("pidnum: %d\n", pidnum);
    // pID = (pid_t)pidnum;
    // ret = kill(pID, 9);
    // printf("ret= %d \n", ret);
    // if (0 == ret)
    //     printf("process: %s kill!\n", processname);
    // else
    //     printf("process: %s not kill!\n", processname);
    return 0;
}

/**
 * @brief :  查找指定名称的进程
 * @param {char} *processname
 * @return {*}
 * @author: LR
 * @Date: 2022-01-10 12:47:22
 */
int find_process_by_name(char *processname)
{
    FILE *pstr;
    char cmd[128], buff[512] = {0};

    sprintf(cmd, "sudo ps -ef | grep %s | wc -l", processname);
    if ((pstr = popen(cmd, "r")) != NULL)
    {
        int count = 0;
        fgets(buff, 512, pstr);
        if (buff != NULL)
        {
            count = atoi(buff);
            if (count > 1)
            {
                pclose(pstr);
                return RES_OK;
            }
            else
            {
                pclose(pstr);
                return RES_ERROR;
            }
        }
        pclose(pstr);
        return RES_ERROR;
    }
    else
    {
        return RES_ERROR;
    }
}
