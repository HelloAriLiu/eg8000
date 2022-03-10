#ifndef __ZH_AO_H__
#define __ZH_AO_H__
#define AO_NUM 1


typedef struct{
	float ao_value[AO_NUM];
	int error_code;
}Struct_Ao;

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
extern int ao_init;
#endif
