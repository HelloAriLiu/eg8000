#include "zh_usart.h"
#include <termios.h>

#define USART_1_DEVPATH "/dev/ttyAMA0"
#define USART_2_DEVPATH "/dev/ttyAMA1"  //uart3
#define USART_3_DEVPATH "/dev/ttyAMA2" //uart4
#define USART_4_DEVPATH "/dev/ttyAMA3" //uart5

#define USART_AT_DEVPATH "/dev/ttyUSB2"//AT



/***************************************************
 * 功能：打开串口
 * 参数：usartName ,串口名称 one value of Enum_UsartName.
 * 返回：文件描述符 >0; RES_ERROR -1;
 * BY：LR
 * Data:2020-07-06
***************************************************/
int zh_usart_open(Enum_UsartName usartName)
{
	int usart_fd = -1;
	switch (usartName)
	{
	case USART_1:
	{
		usart_fd = open(USART_1_DEVPATH, O_RDWR);
		if (usart_fd < 0)
		{
			return RES_ERROR;
		}
	}
	break;
	case USART_2:
	{
		usart_fd = open(USART_2_DEVPATH, O_RDWR);
		if (usart_fd < 0)
		{
			return RES_ERROR;
		}
	}
	break;
	case USART_3:
	{
		usart_fd = open(USART_3_DEVPATH, O_RDWR);
		if (usart_fd < 0)
		{
			return RES_ERROR;
		}
	}
	break;
	case USART_4:
	{
		usart_fd = open(USART_4_DEVPATH, O_RDWR);
		if (usart_fd < 0)
		{
			return RES_ERROR;
		}
	}
	break;
	case USART_AT:
	{
		usart_fd = open(USART_AT_DEVPATH, O_RDWR);
		if (usart_fd < 0)
		{
			return RES_ERROR;
		}
	}
	break;
	default:
		return RES_ERROR;
	}

	/*清除串口非阻塞标志*/
	if (fcntl(usart_fd, F_SETFL, 0) < 0)
	{
		return RES_ERROR;
	}

	return usart_fd;
}

/***************************************************
 * 功能：初始化串口
 * 参数：usartFd ,串口文件描述符
                baudRate，波特率 one value of Enum_BaudRate.
				dataBit，数据位one value of Enum_DataBit.
				stopBit，停止位 one value of Enum_StopBit.
				parityBit，校验位 one value of Enum_ParityBit.
 * 返回：RES_OK 0; RES_ERROR -1;
***************************************************/
int zh_usart_init(int usartFd, Enum_BaudRate baudRate, Enum_DataBit dataBit, Enum_StopBit stopBit, Enum_ParityBit parityBit)
{
	if (usartFd <= 0)
		return RES_ERROR;

	struct termios newtio, oldtio;
	/*获取终端属性*/
	if (tcgetattr(usartFd, &oldtio) != 0)
	{
		return RES_ERROR;
	}

	bzero(&newtio, sizeof(newtio));
	tcflush(usartFd, TCIFLUSH);
	/*设置控制模式*/
	newtio.c_cflag |= CLOCAL | CREAD; //保证程序不占用串口
	newtio.c_cflag &= ~CSIZE; //保证程序可以从串口中读取数据
	/*设置等待时间和最小接受字符*/
	newtio.c_cc[VTIME] = 0; //可以在select中设置
	newtio.c_cc[VMIN] = 0;	//最少读取0个字符

	/*设置输入输出波特率，两者保持一致*/
	switch (baudRate)
	{
	case B_1200:
		cfsetispeed(&newtio, B1200);
		cfsetospeed(&newtio, B1200);
		break;
	case B_2400:
		cfsetispeed(&newtio, B2400);
		cfsetospeed(&newtio, B2400);
		break;
	case B_4800:
		cfsetispeed(&newtio, B4800);
		cfsetospeed(&newtio, B4800);
		break;
	case B_9600:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	case B_19200:
		cfsetispeed(&newtio, B19200);
		cfsetospeed(&newtio, B19200);
		break;
	case B_38400:
		cfsetispeed(&newtio, B38400);
		cfsetospeed(&newtio, B38400);
		break;
	case B_57600:
		cfsetospeed(&newtio, B57600);
		cfsetispeed(&newtio, B57600);
		break;
	case B_230400:
		cfsetospeed(&newtio, B230400);
		cfsetispeed(&newtio, B230400);
		break;
	case B_460800:
		cfsetispeed(&newtio, B460800);
		cfsetospeed(&newtio, B460800);
		break;
	case B_921600:
		cfsetispeed(&newtio, B921600);
		cfsetospeed(&newtio, B921600);
		break;
	default:
	case B_115200:
		cfsetispeed(&newtio, B115200);
		cfsetospeed(&newtio, B115200);
		break;
	}

	/*设置数据位，如果不在范围内按 8 处理*/
	switch (dataBit)
	{
	case DB_CS7:
		newtio.c_cflag &= ~CSIZE; //屏蔽其它标志位
		newtio.c_cflag |= CS7;
		break;
	default:
	case DB_CS8:
		newtio.c_cflag &= ~CSIZE; //屏蔽其它标志位
		newtio.c_cflag |= CS8;
		break;
	}

	/*设置校验位*/
	switch (parityBit)
	{
	/*无奇偶校验位*/
	default:
	case PB_NONE:
	{
		newtio.c_cflag &= ~PARENB; //PARENB：产生奇偶位，执行奇偶校验
		newtio.c_iflag &= ~INPCK;  //INPCK：使奇偶校验起作用
	}
	break;
	/*设置奇校验*/
	case PB_ODD:
	{
		newtio.c_cflag |= PARENB; //PARENB：产生奇偶位，执行奇偶校验
		newtio.c_cflag |= PARODD; //PARODD：若设置则为奇校验,否则为偶校验
		newtio.c_cflag |= INPCK;  //INPCK：使奇偶校验起作用
		newtio.c_cflag |= ISTRIP; //ISTRIP：若设置则有效输入数字被剥离7个字节，否则保留全部8位
	}
	break;
	/*设置偶校验*/
	case PB_EVEN:
	{
		newtio.c_cflag |= PARENB;  //PARENB：产生奇偶位，执行奇偶校验
		newtio.c_cflag &= ~PARODD; //PARODD：若设置则为奇校验,否则为偶校验
		newtio.c_cflag |= INPCK;   //INPCK：使奇偶校验起作用
		newtio.c_cflag |= ISTRIP;  //ISTRIP：若设置则有效输入数字被剥离7个字节，否则保留全部8位
	}
	break;
	}
	/*设置停止位*/
	switch (stopBit)
	{
	default:
	case SB_1:
	{
		newtio.c_cflag &= ~CSTOPB;
	}
	break;
	case SB_2:
	{
		newtio.c_cflag |= CSTOPB;
	}
	break;
	}
	newtio.c_cflag |= (CLOCAL | CREAD);
	newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    newtio.c_oflag &= ~OPOST;
    newtio.c_oflag &= ~(ONLCR | OCRNL);
    newtio.c_iflag &= ~(ICRNL | INLCR);    
    newtio.c_iflag &= ~(IXON | IXOFF | IXANY);
	/*如果发生数据溢出，只接受数据，但是不进行读操作*/
	tcflush(usartFd, TCIFLUSH);
	/*激活配置*/
	if ((tcsetattr(usartFd, TCSANOW, &newtio)) != 0)
	{
		return RES_ERROR;
	}

	return RES_OK;
}
/***************************************************
 * 功能：串口数据发送
 * 参数：usartFd ,串口文件描述符
                *sendBuff，发送数据内容.
				dataLen，发送数据长度.
 * 返回：成功返回发送长度 ; RES_ERROR -1;
 * 作者 ：LR
 * 修改日期 ：2020年03月27日 
***************************************************/
int zh_usart_send(int usartFd, unsigned char *sendBuff, int dataLen)
{
	if (dataLen <= 0)
	{
		return dataLen;
	}
	int len = -1;
	if (usartFd > 0)
	{
		if ((len = write(usartFd, sendBuff, dataLen)) < 0)
		{
			return RES_ERROR;
		}
	}
	return len;
}

/***************************************************
 * 功能：串口数据接收
 * 参数：usartFd ,串口文件描述符
                *readBuff，接受数据地址.
 * 返回：成功返回接收长度 ; RES_ERROR -1;
 * 作者 ：LR
 * 修改日期 ：2020年03月27日 
***************************************************/
int zh_usart_read(int usartFd, unsigned char *readBuff)
{
	int ReadByteAll = 0;
	if (usartFd > 0)
	{
		int ReadByte = 0;
		ReadByte = read(usartFd, readBuff, USART_READ_MAX_BUFF);
		ReadByteAll += ReadByte;
		while(ReadByte!=0 && ReadByteAll < USART_READ_MAX_BUFF)
		{
			usleep(100000);
			ReadByte= read(usartFd, readBuff+ReadByteAll, USART_READ_MAX_BUFF-ReadByteAll); //读出n个字符
			ReadByteAll += ReadByte;
		}		
		return ReadByteAll;
	}
	return -1;
}
/***************************************************
 * 功能：关闭串口文件
 * 参数：usartFd ,串口文件描述符
 * 返回：RES_OK  0; RES_ERROR -1;
 * 作者 ：LR
 * 修改日期 ：2020年03月27日 
***************************************************/
int zh_usart_close(int usartFd)
{
	int res = close(usartFd);
	return res;
}
