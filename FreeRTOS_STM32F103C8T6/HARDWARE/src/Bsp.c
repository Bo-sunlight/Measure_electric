#include "bsp.h" 
#include "lcd.h"
#include "encoder.h"
#include "includes.h"

void bsp_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);   //设置系统中断优先级分组 4
	delay_init();	                                  //延时函数初始化	  
	LED_Init();		                               	//初始化与LED连接的硬件接口
	LCD_Init();	                                    //液晶屏初始化	
    uart_init(115200);                    	       //初始化串口
    usart2_init(9600);
    usart3_init(115200);
    KEY_Init();
 
    TIM3_Int_Init(500-1,8400-1);            //TIM3初始化，定时时间 500*8400*1000/84000000 = 50ms
	AliIoT_Parameter_Init();	    //初始化连接阿里云IoT平台MQTT服务器的参数	
	MQTT_Buff_Init();
	WiFi_RxCounter=0;                                //WiFi接收数据量变量清零                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);          //清空WiFi接收缓冲区 

//	Timer2_Init(300,7200);                        //TIM2初始化，定时时间 300*7200*1000/72000000 = 30ms                   				  //关闭TIM4定时器
}


