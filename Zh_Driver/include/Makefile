CC      = gcc
CFLAGS = -Wall -g -o -fPIC

INCLUDE = -I ../include

TARGET = libhard.so

LIBPATH = ./libs/

vpath %.h ../include

SRCS = $(wildcard  ./src/*.c)

OBJS = $(patsubst %c, %o,$(SRCS))
#OBJS = zh_ai.o zh_ao.o zh_uart.o zh_gpio.o
#SRCS = zh_ai.c zh_ao.c zh_uart.c zh_gpio.c

all:$(OBJS)
	$(CC) -shared -fPIC -o $(TARGET) $(OBJS)
	mv $(TARGET) $(LIBPATH)
$(OBJS):$(SRCS)
	$(CC) $(INCLUDE) -c $^

#all:$(OBJS)
#	$(CC) -shared -fPIC -o $(TARGET) $(OBJS)
#	mv $(TARGET) $(LIBPATH)

clean:
	rm -f *.o
	rm -f $(LIBPATH)*


