#include "led.h"
void LED_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	    		 //LED1-->PE.5 �˿�����, �������
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 GPIO_SetBits(GPIOC,GPIO_Pin_13); 						 //PE.5 ����� 
}
 
/*-------------------------------------------------*/
/*���������жϿ���1״̬����������������            */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void LED1_State(void)
{
	char temp[TBUFF_UNIT];                   //����һ����ʱ������
		
	if(LED0==0)      
		sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302322\",\"params\":{\"%s\":1},\"version\":\"1.0.0\"}",Identifier);  //���LED1�Ǹߵ�ƽ��˵����Ϩ��״̬����Ҫ�ظ��ر�״̬��������
	else            
		sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302322\",\"params\":{\"%s\":0},\"version\":\"1.0.0\"}",Identifier);  //���LED1�ǵ͵�ƽ��˵���ǵ���״̬����Ҫ�ظ���״̬��������
//	MQTT_PublishQs0(P_TOPIC_NAME,temp,strlen(temp));   //������ݣ�������������	
}





