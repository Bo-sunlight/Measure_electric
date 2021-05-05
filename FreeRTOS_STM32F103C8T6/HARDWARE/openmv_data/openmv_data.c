#include "openmv_data.h"

openmv openmv_data;
#define MAX_len 8
unsigned char ucRxBuffer[MAX_len] ;
unsigned char agreement[] = {0xAA,0xAA,0xAA,       //��ͷ
	                            0x00,0x00,0x00};      //У��

//�����жϵ��ô˺���������ÿ�յ�һ�����ݣ�����һ������������յ���������Ϊ��ڲ��� 
void OPNMV_DataHandle(unsigned char ucData)
{
	static unsigned char ucRxCnt = 0;
	ucRxBuffer[ucRxCnt++] = ucData;	     //���յ������ݴ��뻺������
	
	if ((ucRxCnt==1) && ucRxBuffer[0]!=agreement[0])    //����ͷ���ԣ������¿�ʼѰ��0x55����ͷ
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
	if (ucRxCnt<MAX_len) {return;}//���ݲ���8�����򷵻�
  else 
	{
  //memcpyΪ�������Դ����ڴ濽��������������"string.h"�������ջ��������ַ����������ݽṹ�����棬�Ӷ�ʵ�����ݵĽ���
	memcpy(&openmv_data,&ucRxBuffer[4],2);
	openmv_data.datas[0] = openmv_data.data[0]<<8 | openmv_data.data[1];
	ucRxCnt=0;                        //��ջ�����
	}
}

