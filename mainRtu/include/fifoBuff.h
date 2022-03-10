/*
 * @Description :  
 * @FilePath: /Code/include/fifoBuff.h
 * @Author:  LR
 * @Date: 2021-11-19 10:45:57
 */
#ifndef __FIFOBUFF_H
#define __FIFOBUFF_H

/**********************************************
len:数据长度
*payload:数据指针
*next:下一个数据指针
************************************************/
typedef struct fifoBuffList
{
    unsigned int len;
    unsigned char *payload;
    struct fifoBuffList *next;
} fifoBuffList_t;

void fifoBuffListInit(fifoBuffList_t **fifoBuffListTemp);
void fifoBuffListFree(fifoBuffList_t **fifoBuffListTemp);
void fifoBuffListAdd(fifoBuffList_t **head, unsigned char *payload, unsigned int len);
unsigned char *fifoBuffListDel(fifoBuffList_t **head, unsigned int *len);
#endif
