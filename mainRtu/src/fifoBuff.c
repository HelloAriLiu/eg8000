/*
 * @Description :  
 * @FilePath: /app_code/mainRtu/src/fifoBuff.c
 * @Author:  LR
 * @Date: 2021-11-19 10:44:45
 */
#include "fifoBuff.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/**
 * @brief :  初始化链表，
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-11-19 10:51:29
 */
void fifoBuffListInit(fifoBuffList_t **fifoBuffListTemp)
{
}

/**
 * @brief :  链表释放
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2021-11-19 10:51:36
 */
void fifoBuffListFree(fifoBuffList_t **head)
{
    if (head == NULL || *head == NULL)
        return;

    fifoBuffList_t *fifoBuffListTemp = *head;

    while (fifoBuffListTemp != NULL)
    {
        *head = fifoBuffListTemp->next;
        free(fifoBuffListTemp->payload);
        free(fifoBuffListTemp);
        fifoBuffListTemp = *head;
    }
}

/**
 * @brief :  链表表尾添加数据
 * @param {fifoBuffList_t **} head
 * @param {unsigned char*} payload
 * @param {unsigned int} len
 * @return {*}
 * @author: LR
 * @Date: 2021-11-19 10:51:45
 */
void fifoBuffListAdd(fifoBuffList_t **head, unsigned char *payload, unsigned int len)
{
    unsigned char indexCnt = 0;

    if (head == NULL || payload == NULL || len == 0)
        return;
    //将数据插入链表
    fifoBuffList_t *fifoBuffListTempIndex = *head;
    if (fifoBuffListTempIndex != NULL)
    {
        while (fifoBuffListTempIndex->next)
        {
            indexCnt++;
            fifoBuffListTempIndex = fifoBuffListTempIndex->next;
        }
        if (indexCnt >= 50) //最大50个长度
        {
            printf("[WARN]--fifoBuffListAdd indexCnt >= 50\n");
            return;
        }
    }

    fifoBuffList_t *creatNewBuffList = (fifoBuffList_t *)calloc(sizeof(fifoBuffList_t), 1);
    //如果分配不到内存，返回
    if (creatNewBuffList == NULL)
    {
        printf("[ERROR]--creatNewBuffList == NULL  not calloc -return\n");
        return;
    }
    creatNewBuffList->len = len;
    creatNewBuffList->payload = calloc(len + 1, 1);
    if (creatNewBuffList->payload == NULL)
    {
        printf("[ERROR]--creatNewBuffList->payload == NULL  not calloc -return\n");
        free(creatNewBuffList);
        return;
    }
    memcpy(creatNewBuffList->payload, payload, len);
    //memset(creatNewBuffList->payload + len, 0, 1);
    creatNewBuffList->next = NULL;
    //将分配号的数据插入链表尾部
    fifoBuffList_t *fifoBuffListTemp = *head;
    if (fifoBuffListTemp == NULL)
    {
        *head = creatNewBuffList;
    }
    else
    {
        while (fifoBuffListTemp->next)
        {
            fifoBuffListTemp = fifoBuffListTemp->next;
        }
        fifoBuffListTemp->next = creatNewBuffList;
    }
}

/**
 * @brief :  表头释放取出数据
 * @param {fifoBuffList_t **} head
 * @param {unsigned int *} len
 * @return {*}
 * @author: LR
 * @Date: 2021-11-19 10:52:00
 */
unsigned char *fifoBuffListDel(fifoBuffList_t **head, unsigned int *len)
{
    unsigned char *data;

    if (head == NULL || *head == NULL)
    {
        *len = 0;
        return NULL;
    }
    fifoBuffList_t *fifoBuffListTemp = *head;
    //获取数据
    *len = fifoBuffListTemp->len;
    data = fifoBuffListTemp->payload;
    //后移链表
    *head = fifoBuffListTemp->next;
    //释放链表头
    free(fifoBuffListTemp);
    return data;
}
