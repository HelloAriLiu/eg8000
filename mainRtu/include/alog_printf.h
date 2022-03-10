/*
 * @Description :  Ari-Log
 * @FilePath: /mainRtu/include/alog_printf.h
 * @Author:  LR
 * @Date: 
 * *Support thread safety
 */

#ifndef _ALOG_PRINTF_
#define _ALOG_PRINTF_
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <pthread.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    static pthread_mutex_t alogMutex = PTHREAD_MUTEX_INITIALIZER;

    /* output log's level */
    // #define ALOG_LVL_ASSERT 0 //断言-最高日志级别，依次后推
    // #define ALOG_LVL_ERROR 1
    // #define ALOG_LVL_WARN 2
    // #define ALOG_LVL_DEBUG 3
    // #define ALOG_LVL_INFO 4
    // #define ALOG_LVL_CLOSE 255

#define ALOG_COLOR_RED "\x1b[31m"
#define ALOG_COLOR_GREEN "\x1b[32m"
#define ALOG_COLOR_YELLOW "\x1b[33m"
#define ALOG_COLOR_BLUE "\x1b[34m"
#define ALOG_COLOR_MAGENTA "\x1b[35m"
#define ALOG_COLOR_CYAN "\x1b[36m"
#define ALOG_COLOR_RESET "\x1b[0m"

/* output log's level 日志级别，依次后推*/
#define ALOG_LVL_INFO 1
#define ALOG_LVL_DEBUG 2
#define ALOG_LVL_WARN 3
#define ALOG_LVL_ERROR 4
#define ALOG_LVL_ASSERT 5

#define ALOG_LVL_CLOSE 255

/* set log-output level */
#define ALOG_LVL_SET ALOG_LVL_INFO
/*  set log-output File path*/
#define ALOG_PATH "./log/"
/*  set log-output  max mum=10*/
#define ALOG_MAX_NUM 10
/*  set log-output  Single maximum size=1M*/
#define ALOG_MAX_SIZE 1024 * 1024
/* buffer size for every line's log */
#define ALOG_BUF_MAX_SIZE 1024 * 5
/*  log-output enable*/
#define ALOG_OUTPUT_ENABLE true

#define alog_printf(level, outputEnable, ...) log_printf(level, outputEnable, __FILE__, __LINE__, __VA_ARGS__)

/*  */
#define ZEROPAD 1  /* pad with zero */
#define SIGN 2     /* unsigned/signed long */
#define PLUS 4     /* show plus */
#define SPACE 8    /* space if plus */
#define LEFT 16    /* left justified */
#define SMALL 32   /* Must be 32 == 0x20 */
#define SPECIAL 64 /* 0x */

#define __do_div(n, base) ({ \
int __res; \
__res = ((unsigned long) n) % (unsigned) base; \
n = ((unsigned long) n) / (unsigned) base; \
__res; })

    static int skip_atoi(const char **s)
    {
        int i = 0;

        while (isdigit(**s))
            i = i * 10 + *((*s)++) - '0';
        return i;
    }

    static char *number(char *str, long num, int base, int size, int precision, int type)
    {
        /* we are called with base 8, 10 or 16, only, thus don't need "G..." */
        static const char digits[17] = "0123456789ABCDEF"; /* "GHIJKLMNOPQRSTUVWXYZ"; */

        char tmp[66];
        char c, sign, locase;
        int i;

        /* locase = 0 or 0x20. ORing digits or letters with 'locase'
     * produces same digits or (maybe lowercased) letters */
        locase = (type & SMALL);
        if (type & LEFT)
            type &= ~ZEROPAD;
        if (base < 2 || base > 36)
            return NULL;
        c = (type & ZEROPAD) ? '0' : ' ';
        sign = 0;
        if (type & SIGN)
        {
            if (num < 0)
            {
                sign = '-';
                num = -num;
                size--;
            }
            else if (type & PLUS)
            {
                sign = '+';
                size--;
            }
            else if (type & SPACE)
            {
                sign = ' ';
                size--;
            }
        }
        if (type & SPECIAL)
        {
            if (base == 16)
                size -= 2;
            else if (base == 8)
                size--;
        }
        i = 0;
        if (num == 0)
            tmp[i++] = '0';
        else
            while (num != 0)
                tmp[i++] = (digits[__do_div(num, base)] | locase);
        if (i > precision)
            precision = i;
        size -= precision;
        if (!(type & (ZEROPAD + LEFT)))
            while (size-- > 0)
                *str++ = ' ';
        if (sign)
            *str++ = sign;
        if (type & SPECIAL)
        {
            if (base == 8)
                *str++ = '0';
            else if (base == 16)
            {
                *str++ = '0';
                *str++ = ('X' | locase);
            }
        }
        if (!(type & LEFT))
            while (size-- > 0)
                *str++ = c;
        while (i < precision--)
            *str++ = '0';
        while (i-- > 0)
            *str++ = tmp[i];
        while (size-- > 0)
            *str++ = ' ';
        return str;
    }

    /**
     * @brief :  Ari日志打印
     * @param {unsigned char} level
     * @param {_BOOL} outputEnable
     * @param {const char} *func
     * @param {const long} line
     * @param {const char} *fmt
     * @return {*}
     * @author: LR
     * @Date: 2021-06-25 12:11:49
     */
    static void log_printf(unsigned char level, bool outputEnable, const char *file, const long line, const char *fmt, ...)
    {
        if (level < ALOG_LVL_SET)
            return;



        char printf_buf[ALOG_BUF_MAX_SIZE] = {0};
        va_list args;
        int printed;
        va_start(args, fmt);
        printed = vsnprintf(printf_buf, ALOG_BUF_MAX_SIZE, fmt, args);
        va_end(args);

        char buf[ALOG_BUF_MAX_SIZE + 200] = {0};
        int g_Count = 0;

        char temp[256];
        char logFileName[50] = {0};
        char logFileNameTemp[50] = {0};
        char logFileNameLast[50] = {"1580-05-05_00-00-00.log"};
        char logFileNameMix[50] = {"3000-05-05_00-00-00.log"};

        int iMax = ALOG_MAX_SIZE;

        FILE *fp = NULL;
        time_t timep;
        struct tm *p;
        time(&timep);
        p = localtime(&timep); //获取当前系统时间

        //将要保存的日志信息和时间戳信息整合
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "▶[%d-%02d-%02d %02d:%02d:%02d -> %s:%ld]  :  ", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday, (p->tm_hour + 8) % 24, p->tm_min, p->tm_sec, file, line); //星期p->tm_wday
        strcat(buf, printf_buf);
        //strcat(buf, "\0");

        if (outputEnable == true)
            puts(buf);

        return;

        //pthread_mutex_lock(&alogMutex); /* 日志部分上锁 */
        DIR *dir;
        struct dirent *ptr;
        if ((dir = opendir(ALOG_PATH)) == NULL) //打开日志的文件目录，如果没有则建立
        {
            sprintf(temp, "mkdir -p %s", ALOG_PATH);
            system(temp);
            dir = opendir(ALOG_PATH);
        }
        while ((ptr = readdir(dir)) != NULL) //循环读取当前目录下到所有文件
        {
            if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
                continue;
            strcpy(logFileNameTemp, ptr->d_name);
            if (strlen(logFileNameTemp) < 18)
            {
                //g_Count++; //统计符合要求的文件个数
                continue;
            }
            if (strcmp(logFileNameTemp, logFileNameLast) > 0) //找到最新的文件
            {
                strcpy(logFileNameLast, logFileNameTemp); //将最新日期的文件名字保存在logFileNameLast
            }
            if (strcmp(logFileNameTemp, logFileNameMix) < 0) //找到最老的文件
            {
                strcpy(logFileNameMix, logFileNameTemp); //将最老日期的文件名字保存在logFileNameMix
            }
            g_Count++; //统计符合要求的文件个数
        }
        closedir(dir);
        sprintf(logFileName, "%s%s", ALOG_PATH, logFileNameLast); //将最新的文件名字匹配到路径中去，如果没有用默认文件名

        fp = fopen(logFileName, "r+");
        if (fp == NULL)
        {
            //进入这个里面证明没有日志文件产生过，第一次创建+
            sprintf(logFileName, "%s%d-%02d-%02d_%02d-%02d-%02d.log", ALOG_PATH, (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday, (p->tm_hour + 8) % 24, p->tm_min, p->tm_sec);
            fp = fopen(logFileName, "w+");
        }
        else
        {
            fseek(fp, 0, 2);       //SEEK_END值为2
            if (ftell(fp) >= iMax) //如果大小已经超出限制
            {
                fclose(fp);
                if (g_Count >= ALOG_MAX_NUM) //如果日志文件的个数达到路限制10个，则按日期进行循环覆盖
                {
                    sprintf(logFileName, "%s%s", ALOG_PATH, logFileNameMix);
                    remove(logFileName); //删除最老的一个日志文件
                    sprintf(logFileName, "%s%d-%02d-%02d_%02d-%02d-%02d.log", ALOG_PATH, (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday, (p->tm_hour + 8) % 24, p->tm_min, p->tm_sec);
                }
                else
                {
                    sprintf(logFileName, "%s%d-%02d-%02d_%02d-%02d-%02d.log", ALOG_PATH, (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday, (p->tm_hour + 8) % 24, p->tm_min, p->tm_sec);
                }
                fp = fopen(logFileName, "w+");
            }
            else
            {
                fclose(fp);
                fp = fopen(logFileName, "a+"); //以追加的方式打开文件
            }
        }
        fwrite(buf, 1, strlen(buf), fp);
        fflush(fp); //立即刷新缓存区到指定文件流中
        //fsync(fileno(fp)); //将缓存区数据写入磁盘，并等待操作结束
        //fdatasync(fileno(fp));
        fclose(fp);

        pthread_mutex_unlock(&alogMutex); /*写日志部分解锁 */
    }

#ifdef __cplusplus
}
#endif

#endif
