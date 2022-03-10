#ifndef  __ZH_IO_H__
#define  __ZH_IO_H__

#define DO1  1
#define DO2  4
#define DO3  5
#define DO4  6
#define DI1  0
#define DI2  2 
#define DI3  3
#define DI4  12
#define  DIN_NUM  4
#define  DOUT_NUM 4
#define  AOUT_NUM 1
#define DI_UPS 24
/*********/
typedef enum{
	AOUT_0 = 0
}Enum_AoutName;

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

int zh_dout_get(Struct_DoutLevel *doutLevel);
int zh_dout_getLevel(int dout_num);

int zh_dout_setLevel(int dout_num,int dout_level);

void zh_gpio_init(void);
#endif
