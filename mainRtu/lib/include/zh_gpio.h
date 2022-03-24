/*
 * @Description :  
 * @FilePath: /app_code/Zh_Driver/include/zh_gpio.h
 * @Author:  LR
 * @Date: 2016-02-11 16:34:02
 */
#ifndef  __ZH_GPIO_H__
#define  __ZH_GPIO_H__



#define  DIN_NUM  4
#define  DOUT_NUM 4

typedef enum{
	PINLEVEL_LOW = 1,   //low   
	PINLEVEL_HIGH = 0,  //high
	PINLEVEL_X    = 255 
}Enum_PinLevel;

typedef struct{
	unsigned int DIN_Level[DIN_NUM];
	int error_code;
}Struct_DinLevel;


int zh_din_get(Struct_DinLevel *dinLevel);
int zh_din_getLevel(int din_num);
typedef enum{
	DOUT_0 = 0,
	DOUT_1,
	DOUT_2,
	DOUT_3
}Enum_DoutName;


typedef struct{
	unsigned int DOUT_Level[DOUT_NUM];
	int error_code;
}Struct_DoutLevel;

void zh_io_init(void);

int zh_dout_get(Struct_DoutLevel *doutLevel);
int zh_dout_getLevel(int dout_num);
int zh_dout_setLevel(int dout_num,int dout_level);



#endif
