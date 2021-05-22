#include "bsp.h" 
#include "includes.h"
extern void TIM1_Int(u16 arr,u16 psc);
void bsp_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);   //����ϵͳ�ж����ȼ����� 4
	delay_init();	                                  //��ʱ������ʼ��	  
	LED_Init();		                      	    //��ʱ��3pwm�������rgb���������ã�����Ĳ���Ч
	LCD_Init();	                                    //Һ������ʼ��	
    uart_init(115200);                    	       //��ʼ������
    KEY_Init();
    usart2_init(9600);
    usart3_init(115200);
    TIM3_Int_Init(500-1,8400-1);            //TIM3��ʼ������ʱʱ�� 500*8400*1000/84000000 = 50ms
//    TP_Init();
    
	AliIoT_Parameter_Init();	    //��ʼ�����Ӱ�����IoTƽ̨MQTT�������Ĳ���	
	MQTT_Buff_Init();
	WiFi_RxCounter=0;                                //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);          //���WiFi���ջ�����

}


