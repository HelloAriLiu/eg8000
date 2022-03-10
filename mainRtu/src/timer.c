/*
 * @Description :  
 * @FilePath: /app_code/mainRtu/src/timer.c
 * @Author:  LR
 * @Date: 2021-09-28 10:36:30
 */
#include <sys/time.h>
#include <sys/select.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include "timer.h"

#include "baseInit.h"
#include "system.h"
#include "rtuIfsHandle.h"
#include "usartHandle.h"
#include "vpnHandle.h"
#include "mqtts.h"

/************************************************************************
*功能描述 ： 定时器函数中断处理
*参数 ： seconds: s;    mseconds: us
*返回值 ： NULL
*作者 ：LR
*修改日期 ：2019年03月29日 
***********************************************************************/
unsigned int timertick = 0;
void interruptTimer(int seconds, int useconds)
{
	struct timeval temp;
	temp.tv_sec = seconds;
	temp.tv_usec = useconds;
	select(0, NULL, NULL, NULL, &temp);
	timertick++;

	/* 	需要定时触发的事件:100ms */
	{
		ifs_timerHandle();	 /* rtu接口定时器 */
		sys_timerHandle();
	}
	if(timertick%10==0)
	{
		vpn_timerHandle();
		mqtt_timerHandle();
	}
	return;
}
