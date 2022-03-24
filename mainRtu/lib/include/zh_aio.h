/*
 * @Description :  
 * @FilePath: /app_code/Zh_Driver/include/zh_aio.h
 * @Author:  LR
 * @Date: 2021-12-24 14:59:29
 */
#ifndef __ZH_AIO_H__
#define __ZH_AIO_H__


#define AI_NUM 4
#define AO_NUM 4

typedef struct{
	float ai_value[AI_NUM];
	int err_code;
}Struct_Ain;

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
 * @brief :  获取AI
 * @param {Struct_Ain} *ain
 * @return {*}
 * @author: LR
 * @Date: 2022-03-17 04:47:52
 */
int zh_ain_getValue(Struct_Ain *ain);


/**
 * @brief :  AO设置值
 * @param {int} index
 * @param {float} value
 * @return {*}
 * @author: LR
 * @Date: 2022-03-17 04:47:55
 */
int zh_aout_setValue(int index, float value);



#endif
