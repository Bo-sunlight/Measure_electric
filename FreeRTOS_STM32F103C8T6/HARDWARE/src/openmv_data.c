#include "openmv_data.h"

Openmv_Data Code_data;
Openmv_Data Block_data;
MCU_Data MCU_data;
unsigned char ucRxBuffer[MAX_len] ;

char dar[3]  = {1,2};


//�����жϵ��ô˺���������ÿ�յ�һ�����ݣ�����һ������������յ���������Ϊ��ڲ��� 
void OPNMV_DataHandle(unsigned char ucData)
{	   
    static unsigned char ucRxCnt = 0;
	ucRxBuffer[ucRxCnt++] = ucData;	             //���յ������ݴ��뻺������
	
	if ((ucRxCnt==1) && ucRxBuffer[0]!=0x00)    //����ͷ���ԣ������¿�ʼѰ��0x55����ͷ
	{
		ucRxCnt=0;
		return;
	}
	
	if ((ucRxCnt==2) && ucRxBuffer[1]!=0x00)
	{
		ucRxCnt=0;
		return;
	}
	
	if (ucRxCnt<MAX_len) {return;}             //���ݲ������򷵻�
    else 
	{
        
	if(ucRxBuffer[2]==0x00)                                       //���յ���ά������
    {
	  memcpy(&Code_data,&ucRxBuffer[0],MAX_len);
      Code_data.Code_Data = Code_data.data[3]<<8|Code_data.data[4];
    }
    
	else if(ucRxBuffer[2]==0x01)                              //���յ�ɫ������  
    {      
      memcpy(&Block_data,&ucRxBuffer[0],MAX_len);	
      Block_data.X = Block_data.data[3]<<8 | Block_data.data[4];
      Block_data.Y = Block_data.data[5]<<8 | Block_data.data[6];
    }
    
    else if(ucRxBuffer[2]==0x02)                                //���յ�MCU����
     memcpy(&MCU_data,&ucRxBuffer[0],MAX_len);
	
	  ucRxCnt=0;                                                //��ջ�����
     
	}
}

