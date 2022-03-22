RPI_PATH := $(shell pwd)
INCLUDE=-I./include

TARGET = zh_lib_api_node.a
TARGET1 = zh_lib_api_node.so
CFLAGS = -g -w -fPIC -lwiringPi $(INCLUDE)

CC = gcc
AR = ar 

SRCS=$(wildcard ./src/*.c)
OBJS=$(patsubst %c,%o,$(SRCS))


all: $(TARGET)
	#mv -f $(TARGET)  ./obj/
	#mv -f $(TARGET)	../mainRtu/lib/
	mv -f $(OBJS) ./obj/
	cd ./test/ && make
	#cp -a include/*  ../mainRtu/lib/include/
	
$(TARGET) : $(OBJS)
	$(AR) -rc $@ $^

$(TARGET1) : $(OBJS)
	$(CC) -shared -o $@ $^

%o:%c
	$(CC) -c $^ $(INCLUDE) $(CFLAGS)

.PHONY : clean
clean:
	rm -rf $(OBJS)  ./obj/* $(TARGET)
