#ifndef __USART3_H
#define __USART3_H	 ��
#include "includes.h"  //������Ҫ��ͷ�ļ�
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����3��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/3/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

#define USART3_RX_ENABLE 			1					//0,������;1,����.

#define USART3_TXBUFF_SIZE   1024           //���崮��2 ���ͻ�������С 1024�ֽ�
#define USART3_RXBUFF_SIZE   1024              //���崮��2 ���ջ�������С 1024�ֽ�
extern char Usart3_RxCompleted;               //�ⲿ�����������ļ����Ե��øñ���
extern unsigned int Usart3_RxCounter;          //�ⲿ�����������ļ����Ե��øñ���
extern char Usart3_RxBuff[USART3_RXBUFF_SIZE]; //�ⲿ�����������ļ����Ե��øñ���

void usart3_init(u32 bound);				//����3��ʼ�� 
void u3_printf(char* fmt, ...);
void u3_TxData(unsigned char *data);
#endif













