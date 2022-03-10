#include "taskCreat.h"

//初始化一个子任务
_work Init_cwork()
{
	_work handler = NULL;
	handler = malloc(sizeof(work));
	assert(handler != NULL);
	memset(handler, ZERO, sizeof(work));
	handler->work_num = 0;
	handler->work_name = NULL;
	handler->work_handler = NULL;
	handler->next = NULL;
	return handler;
}

//创建一个子任务
_work create_cwork(s32 work_num, pchar work_name, work_fun work_fuc)
{
	_work handler = NULL;
	handler = malloc(sizeof(work));
	assert(handler != NULL);
	memset(handler, ZERO, sizeof(work));
	handler->work_num = work_num;
	handler->work_name = work_name;
	handler->work_handler = work_fuc;
	handler->next = NULL;
	return handler;
}

//注册子任务
No_return Register_work_fuc(_work __work, _work new_work)
{
	assert(__work != NULL);
	_work work_handler = __work;
	while (NULL != work_handler->next)
		work_handler = work_handler->next;
	work_handler->next = new_work;
}

//查找子任务的编号
s32 Find_Work_Num(_work headler, s32 work_num)
{
	assert(headler != NULL);
	_work temp = headler->next;
	while (NULL != temp->next)
	{
		if (temp->work_num == work_num)
			return temp->work_num;
		temp = temp->next;
	}
	return temp->work_num;
}

//查找子任务的名称
pchar Find_Work_Name(_work headler, pchar work_name)
{
	assert(headler != NULL);
	_work temp = headler->next;
	while (NULL != temp->next)
	{
		if (temp->work_name == work_name)
			return temp->work_name;
		temp = temp->next;
	}
	return temp->work_name;
}

//执行子任务----根据任务名称来执行
s32 Run_work_for_work_name(_work headler, pchar work_name)
{
	assert(headler != NULL);
	pthread_t tid;
	s32 ret;
	_work temp = headler;
	while (NULL != temp->next)
	{
		temp = temp->next;
		if (temp->work_name == work_name)
		{
			//创建线程
			ret = pthread_create(&tid, NULL, (void *)temp->work_handler, temp->work_name);
			if (ret != SUCCESS)
			{
				perror("create pthread fail");
				return ERROR;
			}
			//线程分离
			ret = pthread_detach(tid);
			return SUCCESS;
		}
	}
	if (temp->work_name == work_name)
	{
		ret = pthread_create(&tid, NULL, (void *)temp->work_handler, temp->work_name);
		if (ret != SUCCESS)
		{
			perror("create pthread fail");
			return ERROR;
		}
		ret = pthread_detach(tid);
		return SUCCESS;
	}
	printf("not this work , return ERROR!\n");
	return ERROR;
}

//执行子任务----根据任务编号来执行
static s32 Run_work_for_work_num(_work headler, s32 work_num)
{
	assert(headler != NULL);
	pthread_t tid;
	//
	pthread_attr_t attr;
	struct sched_param param;
	//
	int ret;
	_work temp = headler;
	while (NULL != temp->next)
	{
		temp = temp->next;
		if (temp->work_num == work_num)
		{
			//
			pthread_attr_init(&attr);
			param.sched_priority = work_num;			  //以任务编号为优先级，数值越大优先级越高
			pthread_attr_setschedpolicy(&attr, SCHED_RR); //设置调度策略:时间片轮转
			pthread_attr_setschedparam(&attr, &param);	//设置优先级
			ret = pthread_create(&tid, &attr, (void *)temp->work_handler, (void *)&temp->work_num);
			pthread_attr_destroy(&attr);
			//
			//创建线程
			//ret = pthread_create(&tid , NULL,(void *)temp->work_handler , (void *)&temp->work_num);
			if (ret != SUCCESS)
			{
				perror("create pthread fail");
				return ERROR;
			}
			//线程分离
			ret = pthread_detach(tid);
			return SUCCESS;
		}
	}
	if (temp->work_num == work_num)
	{
		ret = pthread_create(&tid, NULL, (void *)temp->work_handler, (void *)&temp->work_num);
		if (ret != SUCCESS)
		{
			perror("create pthread fail");
			return ERROR;
		}
		ret = pthread_detach(tid);
		return SUCCESS;
	}
	printf("not this work , return ERROR!\n");
	return ERROR;
}

static No_return Sort_work_num(s32 *buf, s32 len, int direction)
{
	s32 min;
	s32 index;
	s32 i, j, n;
	if (direction == Positive)
	{
		for (i = ZERO; i < len - 1; i++)
		{
			min = buf[i];
			index = i;
			for (j = i; j < len; j++)
			{
				if (buf[j] < min)
				{
					min = buf[j];
					index = j;
				}
			}
			buf[index] = buf[i];
			buf[i] = min;
		}
	}
	else if (direction == Reverse)
	{
		for (i = 0; i < len; i++)
		{
			for (j = 0; j < len; j++)
			{
				if (buf[i] < buf[i + 1])
				{
					n = buf[i];
					buf[i] = buf[i + 1];
					buf[i + 1] = n;
				}
			}
		}
	}
	else
	{
		return;
	}
}

//工作优先级调度执行--->工作编号小的优先级高，依次类推
s32 Run_Priority_work(_work handler, s32 direction, const s32 work_array_size)
{
	s32 count = 0;
	s32 i;
	assert(handler != NULL);
	_work temp = handler->next;
	s32 Curent_node_Array[work_array_size];

	while (temp != NULL)
	{
		Curent_node_Array[count] = temp->work_num;
		temp = temp->next;
		if (count < work_array_size)
			count++;
	}
	Sort_work_num(Curent_node_Array, NR(Curent_node_Array), direction);
	for (i = 0; i < NR(Curent_node_Array); i++)
		Run_work_for_work_num(handler, Curent_node_Array[i]);
	return SUCCESS;
}

//销毁一个子任务
s32 Destroy_cwork(_work headler, pchar work_name)
{
	assert(headler != NULL);
	_work temp = headler;
	_work temp_header_prev = NULL;
	while (NULL != temp->next)
	{
		temp_header_prev = temp;
		temp = temp->next;
		if (temp->work_name == work_name)
		{
			if (temp->next != NULL)
			{
				temp_header_prev->next = temp->next;
				free(temp);
				temp = NULL;
			}
			else
			{
				temp_header_prev->next = NULL;
				free(temp);
				temp = NULL;
			}
			return SUCCESS;
		}
	}
	printf("Not Work node\n");
	return ERROR;
}

//销毁全部任务
_work Destroy_work(_work headler, _work array)
{
	s32 i;
	assert(headler != NULL);
	_work temp = headler;
	for (i = ZERO; i < NR(array); i++)
		Destroy_cwork(headler, array[i].work_name);
	headler = NULL;
	return headler;
}
