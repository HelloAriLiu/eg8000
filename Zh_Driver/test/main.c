/*
 * @Description :  
 * @FilePath: /app_code/Zh_Driver/test/main.c
 * @Author:  LR
 * @Date: 2022-03-22 09:21:06
 */

#include "zh_base.h"
#include "zh_gpio.h"
#include "zh_rtc.h"
#include "zh_aio.h"
#include "zh_network.h"

int main(void)
{
    zh_board_init();
    int i = 0;

    // Struct_Ain ain;

    // for(i=0;i<4;i++)
    // {
    //     if(zh_aout_setValue(i, 8.0+i*4)!=RES_OK)
    //     return;
    // }
    // printf(" aout set [8.0    12.0     16.0     20.0]\n");

//    if( zh_dataCall_init("","","")==RES_OK)
//    {
//        printf("zh_dataCall_init  ok\n");
//    }

    while (1)
    {
        // if (zh_ain_getValue(&ain) == RES_OK)
        // {
        //     printf("AI_0=%0.3f     AI_1=%0.3f      AI_2=%0.3f     AI_3=%0.3f \n", ain.ai_value[0], ain.ai_value[1], ain.ai_value[2], ain.ai_value[3]);
        // }
        // else
        // {

        //     printf("ain get error\n");
        // }

        zh_wdg_feed();
        printf("UPS=%d \n", zh_vin_getValue());
        sleep(1);
    }

    // Struct_DinLevel dinLevel;

    // char strTime[128];
    //set RTC  time
    // struct tm src_tm;
    // struct tm *p;
    // p = &src_tm;
    // p->tm_year = 122; /* Year- 1900  :  2020*/
    // p->tm_mon = 2;    /* Month.	[0-11]   :7 */
    // p->tm_mday = 22;  /* Day.		[1-31]    :8*/
    // p->tm_wday = 2;   /* Day of week.	[0-6] :  3*/
    // p->tm_hour = 14;  /* Hours.	[0-23] : 14*/
    // p->tm_min = 49;   /* Minutes.	[0-59] : 35*/
    // p->tm_sec = 30;   /* Seconds.	[0-60] (1 leap second) :30*/
    // if (zh_setTm_rtc(p) < 0)
    // {
    //     printf("zh_setTm_rtc error!\n");
    //     return -1;
    // }
    // sprintf(strTime, "%04d-%02d-%02d %02d:%02d:%02d", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday, (p->tm_hour) % 24, p->tm_min, p->tm_sec);
    // printf("set rtc datatime : %s \n \n", strTime);
    //read RTC  time
    // struct tm des_tm;
    // struct tm *p1;
    // p1 = &des_tm;

    // int usart_fd[4];
    // for (int i = 0; i < 4; i++)
    // {

    //     usart_fd[i] = zh_usart_open(i);
    //     if (usart_fd[i] < 0)
    //     {
    //         printf("串口%d  打开失败，测试不通过ＸＸＸ\n", i);
    //         return;
    //     }
    //     if (zh_usart_init(usart_fd[i], B_9600, DB_CS8, SB_1, PB_NONE) != RES_OK)
    //     {
    //         printf("串口%d  init失败，测试不通过ＸＸＸ\n", i);
    //         return;
    //     }
    // }

    // char sendbuf[1024];
    // char readbuf[1024];
    // int readlen = 0;

    // while (1)
    // {
    //     for (i = 0; i < 4; i++)
    //     {
    //         readlen = zh_usart_read(usart_fd[i], readbuf);
    //         if (readlen > 0)
    //         {

    //             sprintf(sendbuf, "usrat[%d]  REV [%d]: %s \n", i, readlen, readbuf);
    //             printf("%s",sendbuf);
    //             zh_usart_send(usart_fd[i], sendbuf, strlen(sendbuf));
    //         }
    //         usleep(500);
    //     }

    // zh_led_setValue(LED_1, LED_ON);
    // zh_led_setValue(LED_2, LED_ON);
    // sleep(1);
    // zh_led_setValue(LED_1, LED_OFF);
    // zh_led_setValue(LED_2, LED_OFF);
    // sleep(1);

    // printf("key is:%d \n", zh_key_getValue(KEY0));
    // sleep(1);

    // for (i = 0; i < DOUT_NUM; i++)
    // {
    //     zh_dout_setLevel(i, 0);
    //     sleep(1);
    // }
    // for (i = 0; i < DOUT_NUM; i++)
    // {
    //     zh_dout_setLevel(i, 1);
    //     sleep(1);
    // }

    // if (zh_din_get(&dinLevel) == RES_OK)
    // {
    //     printf("DI_1=%d      DI_2=%d      DI_3=%d      DI_4=%d \n", dinLevel.DIN_Level[0], dinLevel.DIN_Level[1], dinLevel.DIN_Level[2], dinLevel.DIN_Level[3]);
    // }
    // else
    // {
    //     printf("error\n");
    // }
    // sleep(1);

    //     if (zh_getTm_rtc(p1) < 0)
    //     {
    //         printf("zh_getTm_rtc error !\n");
    //         return -1;
    //     }
    //     sprintf(strTime, "%04d-%02d-%02d %02d:%02d:%02d", (1900 + p1->tm_year), (1 + p1->tm_mon), p1->tm_mday, (p1->tm_hour) % 24, p1->tm_min, p1->tm_sec);
    //    printf("get rtc datatime : %s  \n", strTime);
    //     sleep(1);
    // }

    return 0;
}