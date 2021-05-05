#ifndef __USART2_H
#define __USART2_H	 

#define PA2 PAout(2)	
#define PA3 PAin(3)		
#include "includes.h"  //包含需要的头文件

void uart2_init(u32 pclk2,u32 bound);

void usart2_init(u32 bound);				//串口3初始化 

#endif

