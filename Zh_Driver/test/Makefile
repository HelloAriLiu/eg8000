#--------------------------------------------------------------
#            test
#--------------------------------------------------------------

ZH_SDK_PATH   ?= $(shell pwd)/../

# 可执行文件的名字
TARGET = ex_pi_exe
# 先定义编译器链接器
CC =gcc
# -I指定头文件目录
INCLUDE = -I$(ZH_SDK_PATH)/include
# 开启编译warning和设置优化等级
CFLAGS = -g -w $(INCLUDE) 
# -l指定静态库名字
LIB = -L $(ZH_SDK_PATH)/ -l:zh_lib_api_node.a  -lm -lwiringPi
# 正则表达式表示目录下所有.c文件，相当于：SRCS = main.c a.c b.c
SRCS = $(wildcard *.c )
# OBJS表示SRCS中把列表中的.c全部替换为.o，相当于：OBJS = main.o a.o b.o
OBJS = $(patsubst %c, %o, $(SRCS))
#指定 .o 文件生成的目录
#ALL_OBJ_POS = $(addprefix $(OBJ_PATH),$(OBJS))
# 要生成的目标文件
all: $(TARGET)
	rm $(OBJS)

# 第一行依赖关系：冒号后面为依赖的文件，相当于Hello: main.o a.o b.o
# 第二行规则：$@表示目标文件，$^表示所有依赖文件，$<表示第一个依赖文件
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIB)
# 上一句目标文件依赖一大堆.o文件，这句表示所有.o都由相应名字的.c文件自动生成
%o:%c
	$(CC) -c $^ $(INCLUDE) $(CFLAGS) 

.PHONY:clean  
clean:
	rm -rf $(OBJS) $(TARGET)



