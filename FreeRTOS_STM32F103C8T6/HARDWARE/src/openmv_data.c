#include "openmv_data.h"

Openmv_Data Code_data;
Openmv_Data Block_data;
MCU_Data MCU_data;
unsigned char ucRxBuffer[MAX_len] ;

char dar[3]  = {1,2};


//串口中断调用此函数，串口每收到一个数据，调用一次这个函数，收到的数据作为入口参数 
void OPNMV_DataHandle(unsigned char ucData)
{	   
    static unsigned char ucRxCnt = 0;
	ucRxBuffer[ucRxCnt++] = ucData;	             //将收到的数据存入缓冲区中
	
	if ((ucRxCnt==1) && ucRxBuffer[0]!=0x00)    //数据头不对，则重新开始寻找0x55数据头
	{
		ucRxCnt=0;
		return;
	}
	
	if ((ucRxCnt==2) && ucRxBuffer[1]!=0x00)
	{
		ucRxCnt=0;
		return;
	}
	
	if (ucRxCnt<MAX_len) {return;}             //数据不满，则返回
    else 
	{
        
	if(ucRxBuffer[2]==0x00)                                       //接收到二维码数据
    {
	  memcpy(&Code_data,&ucRxBuffer[0],MAX_len);
      Code_data.Code_Data = Code_data.data[3]<<8|Code_data.data[4];
    }
    
	else if(ucRxBuffer[2]==0x01)                              //接收到色块数据  
    {      
      memcpy(&Block_data,&ucRxBuffer[0],MAX_len);	
      Block_data.X = Block_data.data[3]<<8 | Block_data.data[4];
      Block_data.Y = Block_data.data[5]<<8 | Block_data.data[6];
    }
    
    else if(ucRxBuffer[2]==0x02)                                //接收到MCU数据
     memcpy(&MCU_data,&ucRxBuffer[0],MAX_len);
	
	  ucRxCnt=0;                                                //清空缓存区
     
	}
}

