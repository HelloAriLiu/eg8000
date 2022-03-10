
RSAP_SDK_PATH :=$(shell pwd)

TARGET =  zh_node.app

CC = gcc

INCLUDE = -I$(RSAP_SDK_PATH)/include -I$(RSAP_SDK_PATH)/lib/include -I$(RSAP_SDK_PATH)/lib/elog/include -I$(RSAP_SDK_PATH)/lib/libmqtt/include
CFLAGS = -g -w $(INCLUDE)
LIB = -L $(RSAP_SDK_PATH)/lib/ -l:zh_lib_api_node.a  -L $(RSAP_SDK_PATH)/lib/libmqtt/  -l:libpaho-mqtt3c.a  -lwiringPi -lm -lpthread  -lcrypto -lssl

OBJS += $(patsubst %.c, %.o, $(wildcard $(RSAP_SDK_PATH)/src/*.c))
OBJS += $(patsubst %.c, %.o, $(wildcard $(RSAP_SDK_PATH)/lib/elog/src/*.c))

#SRCS = $(wildcard ./src/*.c)
#OBJS = $(patsubst %c,%o,$(SRCS))

all: $(TARGET)
	mv -f $(OBJS) ./obj/
	cp -a $(TARGET) /home/pi/edge8000/app/
	cp -a  *.sh  /home/pi/edge8000/app/
	
$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(LIB)

%o:%c
	$(CC) -c $^ $(INCLUDE) $(CFLAGS)
	#mv $@ ./obj/

.PHONY:clean
clean:
	rm -rf $(OBJS) $(TARGET)
