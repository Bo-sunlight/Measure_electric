#include "bsp.h" 
#include "lcd.h"
#include "encoder.h"
#include "includes.h"

void bsp_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);   //����ϵͳ�ж����ȼ����� 4
	delay_init();	                                  //��ʱ������ʼ��	  
	LED_Init();		                               	//��ʼ����LED���ӵ�Ӳ���ӿ�
	LCD_Init();	                                    //Һ������ʼ��	
    uart_init(115200);                    	       //��ʼ������
    usart2_init(9600);
    usart3_init(115200);
    KEY_Init();
 
    TIM3_Int_Init(500-1,8400-1);            //TIM3��ʼ������ʱʱ�� 500*8400*1000/84000000 = 50ms
	AliIoT_Parameter_Init();	    //��ʼ�����Ӱ�����IoTƽ̨MQTT�������Ĳ���	
	MQTT_Buff_Init();
	WiFi_RxCounter=0;                                //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);          //���WiFi���ջ����� 

//	Timer2_Init(300,7200);                        //TIM2��ʼ������ʱʱ�� 300*7200*1000/72000000 = 30ms                   				  //�ر�TIM4��ʱ��
}


