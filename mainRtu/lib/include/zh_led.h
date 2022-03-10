/*
 * @Author: your name
 * @Date: 2021-12-01 16:31:40
 * @LastEditTime: 2021-12-01 16:31:40
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \Zh_Driver\include\zh_led.h
 */
#ifndef __ZH_LED_H__
#define __ZH_LED_H__

#include "zh_base.h"


/******************************************************************************************
 * led name
*******************************************************************************************/
typedef enum
{
	LED_1 = 1,
	LED_2,
	LED_3
} Enum_LedName;
/******************************************************************************************
 * led state
******************************************************************************************/
typedef enum
{
	LED_ON= 0,
	LED_OFF
} Enum_LedState;


/**
 * @brief :  设置LED状态
 * @param {Enum_LedName} ledName
 * @param {Enum_LedState} ledState
 * @return {*}
 * @author: LR
 * @Date: 2021-10-29 11:31:02
 */
int zh_led_setValue(Enum_LedName ledName, Enum_LedState ledState);





#endif