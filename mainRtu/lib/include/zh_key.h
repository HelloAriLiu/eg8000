/*
 * @Author: your name
 * @Date: 2021-12-01 17:12:25
 * @LastEditTime: 2021-12-01 17:12:25
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \Zh_Driver\include\zh_key.h
 */
#ifndef __ZH_KEY_H__
#define __ZH_KEY_H__
#include "zh_base.h"


/******************************************************************************************
 * key name
******************************************************************************************/
typedef enum
{
	KEY0 = 0,
} Enum_KeyName;

/**
 * @brief :  获取按键key值
 * @param {Enum_KeyName} keyName
 * @return {*}
 * @author: LR
 * @Date: 2021-10-29 11:32:44
 */
 int  zh_key_getValue(Enum_KeyName keyName);

#endif
