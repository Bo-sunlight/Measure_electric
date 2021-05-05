#include "openmv_data.h"

openmv openmv_data;
#define MAX_len 8
unsigned char ucRxBuffer[MAX_len] ;
unsigned char agreement[] = {0xAA,0xAA,0xAA,       //贞头
	                            0x00,0x00,0x00};      //校验

//串口中断调用此函数，串口每收到一个数据，调用一次这个函数，收到的数据作为入口参数 
void OPNMV_DataHandle(unsigned char ucData)
{
	static unsigned char ucRxCnt = 0;
	ucRxBuffer[ucRxCnt++] = ucData;	     //将收到的数据存入缓冲区中
	
	if ((ucRxCnt==1) && ucRxBuffer[0]!=agreement[0])    //数据头不对，则重新开始寻找0x55数据头
	{
		ucRxCnt=0;
		return;
	}
	if ((ucRxCnt==2) && ucRxBuffer[1]!=agreement[1])
	{
		ucRxCnt=0;
		return;
	}
		if ((ucRxCnt==3) && ucRxBuffer[2]!=agreement[2])
	{
		ucRxCnt=0;
		return;
	}
	
		if ((ucRxCnt==8) && ucRxBuffer[7]!=agreement[5])
	{
		ucRxCnt=0;
		return;
	}
	if (ucRxCnt<MAX_len) {return;}//数据不满8个，则返回
  else 
	{
  //memcpy为编译器自带的内存拷贝函数，需引用"string.h"，将接收缓冲区的字符拷贝到数据结构体里面，从而实现数据的解析
	memcpy(&openmv_data,&ucRxBuffer[4],2);
	openmv_data.datas[0] = openmv_data.data[0]<<8 | openmv_data.data[1];
	ucRxCnt=0;                        //清空缓存区
	}
}

