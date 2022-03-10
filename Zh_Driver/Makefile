RPI_PATH := $(shell pwd)
INCLUDE=-I./include
TARGET = zh_lib_api_node.a
TARGET1 = libhard_api.so
CFLAGS = -g -w -fPIC $(INCLUDE)

CC = gcc
AR = ar 

SRCS=$(wildcard ./src/*.c)
OBJS=$(patsubst %c,%o,$(SRCS))


all: $(TARGET)
	#mv -f $(TARGET)  ./obj/
	mv -f $(TARGET)	../mainRtu/lib/
	mv -f $(OBJS) ./obj/
	cp -a include/*  ../mainRtu/lib/include/
#	echo $(RPI_PATH)
	
$(TARGET) : $(OBJS)
	$(AR) -rc $@ $^

$(TARGET1) : $(OBJS)
	$(CC) -shared -o $@ $^

%o:%c
	$(CC) -c $^ $(INCLUDE) $(CFLAGS)


#all:$(OBJS)
#	$(CC) $(OBJS) -o all

#zh_ai.o:zh_ai.c
#	$(CC) -c zh_ai.c $(INCLUDE)

#zh_ao.o:zh_ao.c
#	$(CC) -c zh_ao.c $(INCLUDE)

#zh_uart.o:zh_uart.c
#	$(CC) -c zh_uart.c $(INCLUDE)


#zh_gpio.o:zh_gpio.c
#	$(CC) -c zh_gpio.c $(INCLUDE)

.PHONY : clean
clean:
	rm -rf $(OBJS)  ./obj/* $(TARGET)
