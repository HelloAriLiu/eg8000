#include <pthread.h>
#include <sys/syscall.h>
#include <sys/prctl.h>
#include "taskCreat.h"
#include "timer.h"
#include "baseInit.h"
#include "system.h"
#include "localServer_do.h"
#include "localServer_di.h"
#include "localServer_ai.h"
#include "localServer_ao.h"
#include "localServer_ups.h"
#include "localServer_led.h"
#include "localServer_sys.h"
#include "localServer_usart.h"
#include "rtuIfsHandle.h"
#include "usartHandle.h"
#include "vpnHandle.h"
#include "mqtts.h"

/* 任务函数声明*/
void localserver_do_task(int work_num);
void localserver_di_task(int work_num);
void localserver_ai_task(int work_num);
void localserver_ao_task(int work_num);
void localserver_led_task(int work_num);
void localserver_ups_task(int work_num);
void localserver_sys_task(int work_num);
void localserver_usart_0_task(int work_num);
void localserver_usart_1_task(int work_num);
void localserver_usart_2_task(int work_num);
void localserver_usart_3_task(int work_num);
void usart_work_task(int work_num);
void ifs_work_task(int work_num);
void sys_work_task(int work_num);
void mqtt_work_task(int work_num);

/* 	{1,"Test1",Test1},    1 表示任务编号,编号越小优先级越高；"Test1" 表示任务名称；Test1 表示任务执行函数 */
work work_Register[] =
	{
		{1, "ifs_work_task", ifs_work_task},
		{2, "usart_work_task", usart_work_task},
		{3, "localserver_led_task", localserver_led_task},
		{4, "localserver_do_task", localserver_do_task},
		{5, "localserver_di_task", localserver_di_task},
		{6, "localserver_ao_task", localserver_ao_task},
		{7, "localserver_ai_task", localserver_ai_task},
		{8, "localserver_ups_task", localserver_ups_task},
		{9, "localserver_sys_task", localserver_sys_task},
		{10, "sys_work_task", sys_work_task},
		{11, "localserver_usart_0_task", localserver_usart_0_task},
		{12, "localserver_usart_1_task", localserver_usart_1_task},
		{13, "mqtt_work_task", mqtt_work_task},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{
	//rtc_systime_sync(); /* 同步RTC->>系统时间 */
	init_dev(); //设备初始化

	rtuIfs_init(); /* 接口初始化 */
	usart_init();  /* 串口初始化 */

	/* 	1、定义头指针，初始化头节点 */
	int i = 0;
	INIT_WORK(work_node);
	for (i = ZERO; i < NR(work_Register); i++)
	{
		/* 2、实现工作任务的注册 ，串联成结构体链表*/
		REGISTER_WORK(work_node, create_cwork(work_Register[i].work_num, work_Register[i].work_name, work_Register[i].work_handler));
	}
	/* 3、调度工作任务，编号从小到大排序 ，注册线程*/
	SCHEDULING_WORK(work_node, Positive, NR(work_Register));
	while (1)
	{
		/* 定时器 让主进程一直不退出  定时100ms */
		interruptTimer(0, 100 * 999);
		//usleep(10);
	}
	return SUCCESS;
}

/************************************************************
*功能描述 ： 本地TCP服务器任务
************************************************************/
void localserver_do_task(int work_num)
{
	prctl(PR_SET_NAME, "doS_work");
	init_localServer_do_service();
	while (1)
	{
		create_localServer_do_service();
		usleep(50000);
	}
}

/************************************************************
*功能描述 ： 本地TCP服务器任务
************************************************************/
void localserver_di_task(int work_num)
{
	prctl(PR_SET_NAME, "diS_work");
	init_localServer_di_service();
	while (1)
	{
		create_localServer_di_service();
		usleep(50000);
	}
}
/************************************************************
*功能描述 ： 本地TCP服务器任务
************************************************************/
void localserver_ao_task(int work_num)
{
	prctl(PR_SET_NAME, "aoS_work");
	init_localServer_ao_service();
	while (1)
	{
		create_localServer_ao_service();
		usleep(50000);
	}
}
/************************************************************
*功能描述 ： 本地TCP服务器任务
************************************************************/
void localserver_ai_task(int work_num)
{
	prctl(PR_SET_NAME, "aiS_work");
	init_localServer_ai_service();
	while (1)
	{
		create_localServer_ai_service();
		usleep(50000);
	}
}
/************************************************************
*功能描述 ： 本地TCP服务器任务
************************************************************/
void localserver_usart_0_task(int work_num)
{
	prctl(PR_SET_NAME, "usart0S_work");
	init_localServer_usart_service(0);
	while (1)
	{
		create_localServer_usart_service(0);
		usleep(50000);
	}
}

void localserver_usart_1_task(int work_num)
{
	prctl(PR_SET_NAME, "usart1S_work");
	init_localServer_usart_service(1);
	while (1)
	{
		create_localServer_usart_service(1);
		usleep(50000);
	}
}

/************************************************************
*功能描述 ： 本地TCP服务器任务
************************************************************/
void localserver_led_task(int work_num)
{
	prctl(PR_SET_NAME, "ledS_work");
	init_localServer_led_service();
	while (1)
	{
		create_localServer_led_service();
		usleep(50000);
	}
}
/************************************************************
*功能描述 ： 本地TCP服务器任务
************************************************************/
void localserver_ups_task(int work_num)
{
	prctl(PR_SET_NAME, "ups_work");
	init_localServer_ups_service();
	while (1)
	{
		create_localServer_ups_service();
		usleep(50000);
	}
}
/************************************************************
*功能描述 ： 本地TCP服务器任务
************************************************************/
void localserver_sys_task(int work_num)
{
	prctl(PR_SET_NAME, "sysS_work");
	init_localServer_sys_service();
	while (1)
	{
		create_localServer_sys_service();
		usleep(50000);
	}
}
/************************************************************
*功能描述 ： 串口-任务
************************************************************/
void usart_work_task(int work_num)
{
	prctl(PR_SET_NAME, "usart_work");
	while (1)
	{
		usart_listen_handle();
		usleep(5000);
	}
}

/************************************************************
*功能描述 ：杂项-任务
************************************************************/
void ifs_work_task(int work_num)
{
	prctl(PR_SET_NAME, "ifs_work");
	while (1)
	{
		ainHandle();
		aoHandle();
		dinHandle();
		doutHandle();
		usleep(50000);
	}
}

/************************************************************
*功能描述 ：mqtt-任务
************************************************************/
void mqtt_work_task(int work_num)
{
	prctl(PR_SET_NAME, "mqtt_work");
	while (1)
	{
		mqtt_state_proc();
		frp_connect_check();
		usleep(50000);
	}
}

/************************************************************
*功能描述 ：杂项-任务
************************************************************/
void sys_work_task(int work_num)
{
	prctl(PR_SET_NAME, "sys_work");
	while (1)
	{
		upsHandle();
		led_display();
		sysCmdMonitor();
		//wdg_handle();
		usleep(50000);
	}
}
