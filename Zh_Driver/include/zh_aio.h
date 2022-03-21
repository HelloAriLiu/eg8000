/*
 * @Description :  
 * @FilePath: /app_code/Zh_Driver/include/zh_aio.h
 * @Author:  LR
 * @Date: 2021-12-24 14:59:29
 */
#ifndef __ZH_AIN_H__
#define __ZH_AIN_H__


#define AI_NUM 4
#define AO_NUM 1

typedef struct{
	float ai_value[AI_NUM];
	int err_code;
}Struct_Ain;
/***************************************************
 * 功能：获取模拟量AIN�? * 参数：ain, 模拟量通道编号  one value of Struct_Ain.
 * 返回：成功RES_OK ; 错误RES_ERROR;
***************************************************/
int zh_ain_getValue(Struct_Ain *ain);




typedef struct{
	float ao_value[AO_NUM];
	int error_code;
}Struct_Ao;



/**
 * @brief :  aio的初始化
 * @param {*}
 * @return {*}
 * @author: LR
 * @Date: 2022-03-17 04:47:58
 */
int zh_aio_init(void);
/**
 * @brief :  获取AO数值
 * @param {int} aoFd
 * @param {Struct_AO} *ao
 * @return {*}
 */
int zh_aout_getValue(Struct_Ao *ao);

/**
 * @brief :  设置AO数值
 * @param {int} aoFd
 * @param {int} index
 * @param {float} value
 * @return {*}
 */
int zh_aout_setLevel(int index, float value);



#endif
