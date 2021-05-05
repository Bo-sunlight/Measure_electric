#ifndef __USART3_H
#define __USART3_H	 件
#include "includes.h"  //包含需要的头文件
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口3驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/3/29
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

#define USART3_RX_ENABLE 			1					//0,不接收;1,接收.

#define USART3_TXBUFF_SIZE   1024           //定义串口2 发送缓冲区大小 1024字节
#define USART3_RXBUFF_SIZE   1024              //定义串口2 接收缓冲区大小 1024字节
extern char Usart3_RxCompleted;               //外部声明，其他文件可以调用该变量
extern unsigned int Usart3_RxCounter;          //外部声明，其他文件可以调用该变量
extern char Usart3_RxBuff[USART3_RXBUFF_SIZE]; //外部声明，其他文件可以调用该变量

void usart3_init(u32 bound);				//串口3初始化 
void u3_printf(char* fmt, ...);
void u3_TxData(unsigned char *data);
#endif













