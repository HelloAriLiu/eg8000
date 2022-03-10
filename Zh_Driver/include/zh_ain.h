#ifndef __ZH_AIN_H__
#define __ZH_AIN_H__


#define AI_NUM 4

typedef struct{
	float ai_value[AI_NUM];
	int err_code;
}Struct_Ain;
/***************************************************
 * 功能：获取模拟量AIN�? * 参数：ain, 模拟量通道编号  one value of Struct_Ain.
 * 返回：成功RES_OK ; 错误RES_ERROR;
***************************************************/
int zh_ain_getValue(Struct_Ain *ain);





#endif
