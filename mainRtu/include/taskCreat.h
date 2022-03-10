#ifndef __TASKCREAT_H
#define __TASKCREAT_H

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h> 
#include <assert.h>
#include <pthread.h>

#define NR(x) (sizeof(x)/sizeof(x[0]))

//自定义数据类型



typedef unsigned char      		  u8;
typedef unsigned short int 		u16;
typedef unsigned int        		u32;
typedef char  							  s8 ;
typedef int   								s32;
typedef short 							  s16;



typedef char  * pchar;
typedef int   * pint ;
typedef short * pshort ;
typedef void    No_return;
typedef void (*work_fun)();
 
#define SUCCESS 0
#define ZERO    0
#define ERROR  -1
#define Not_sorted -1
#define Positive 1
#define Reverse 0
 
typedef struct __Work
{
	//任务编号
	//根据任务编号决定工作任务的优先级
	//编号越小，优先级越高
	s32 work_num ;
	//任务名称 
	pchar work_name ;
	//根据相应的任务名称，处理相应的任务     
	void (*work_handler)(int);
 
	struct __Work *next ;
}work;
typedef work * _work ;
 
#define __INIT_WORK(_work)			\
	do {							\
		_work = Init_cwork(_work);		\
	} while (0)
 
#define INIT_WORK(work_node) \
	 _work work_node = NULL ; \
	__INIT_WORK(work_node); 
 
#define REGISTER_WORK(__work,new_work) \
	Register_work_fuc(__work,new_work);
		
#define SCHEDULING_WORK(work_node,direction,array_size) \
	Run_Priority_work(work_node,direction,array_size);
 
	
#define DESTROY_WORK(work_node,array) \
		work_node = Destroy_work(work_node ,array);
 
//初始化一个子任务 
_work Init_cwork();
//创建一个子任务
_work create_cwork(s32 work_num,pchar work_name ,work_fun work_fuc) ;
//注册子任务
No_return Register_work_fuc(_work __work,_work new_work);
//查找子任务的编号
s32 Find_Work_Num(_work headler,s32 work_num);
//查找子任务的名称
pchar Find_Work_Name(_work headler,pchar work_name) ;
//执行子任务----根据任务名称来执行
s32 Run_work_for_work_name(_work headler,pchar work_name);
//执行子任务----根据任务编号来执行
static s32 Run_work_for_work_num(_work headler,s32 work_num);
//销毁一个子任务
s32 Destroy_cwork(_work headler,pchar work_name);
//销毁全部任务 
_work Destroy_work(_work headler,_work array);
//工作优先级调度执行--->工作编号小的优先级高，依次类推
s32 Run_Priority_work(_work handler,s32 direction,const s32 work_array_size) ;
 
#endif //__WORK_H




