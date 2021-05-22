/*  
*****************************************************************
*                           STM32F103C8T6                                  
*****************************************************************
*/
#include "includes.h"
u8 DC_Flage = 1;
u8 star_x = 10,star_y = 120,ratio_y = 25;
#define BIT_0	(1 << 0)
#define BIT_1	(1 << 1)
//任务句柄
TaskHandle_t LED0Task_Handler; 
TaskHandle_t KEYTask_Handler;
TaskHandle_t CPUask_Handler;
TaskHandle_t OLEDask_Handler;
TaskHandle_t HLW_Handler;
TimerHandle_t xTimers1 = {NULL};
//信号句柄


//任务函数
void led0_task(void *pvParameters);
void KEY_task(void *pvParameters);
void CPU_task(void *pvParameters);
void OLED_task(void *pvParameters);
void HLW_task(void *pvParameters);
void vTimerCallback(xTimerHandle pxTimer);

static void AppTaskCreate (void);
static void AppObjCreate (void);

int main(void)
{	
    /* 关闭大部分中断 */ 
	__set_PRIMASK(1);                //要中断的函数不要放在Bsp函数中，否则中断无法执行
	
	/* 硬件初始化 */
	bsp_Init();
	
	/* 创建任务 */
	AppTaskCreate();
	
	/* 创建任务通信机制 */
	 AppObjCreate();
	
	 /* 启动调度，开始执行任务 */
   vTaskStartScheduler();
	
/* 
 如果系统正常启动是不会运行到这里的，运行到这里极有可能是用于定时器任务或者空闲任务的
 heap 空间不足造成创建失败，此要加大 FreeRTOSConfig.h 文件中定义的 heap 大小：
 #define configTOTAL_HEAP_SIZE ( ( size_t ) ( 8 * 1024 ) )
*/
   while(1);      
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void AppTaskCreate (void)
{
 taskENTER_CRITICAL(); //进入临界区
	
	  //创建 LED0 任务
	 xTaskCreate((TaskFunction_t )led0_task, 
                 (const char* )"led0_task", 
                 (uint16_t )512, 
								 (void* )NULL,
								 (UBaseType_t )5,
								 (TaskHandle_t* )&LED0Task_Handler); 
 
     //创建KEY任务
    xTaskCreate((TaskFunction_t )KEY_task,     
                (const char*    )"KEY_task",   
                (uint16_t       )126, 
                (void*          )NULL,
                (UBaseType_t    )1,
                (TaskHandle_t*  )&KEYTask_Handler);   
				                
	//创建OLED任务
    xTaskCreate((TaskFunction_t )OLED_task,     
                (const char*    )"OLED_task",   
                (uint16_t       )512, 
                (void*          )NULL,
                (UBaseType_t    )1,
                (TaskHandle_t*  )&OLEDask_Handler);  
                
    xTaskCreate((TaskFunction_t )HLW_task,     
                (const char*    )"HLW_task",   
                (uint16_t       )125, 
                (void*          )NULL,
                (UBaseType_t    )2,
                (TaskHandle_t*  )&HLW_Handler); 
                
	
		#if 0
	//创建CPU任务
    xTaskCreate((TaskFunction_t )CPU_task,     
                (const char*    )"CPU_task",   
                (uint16_t       )256, 
                (void*          )NULL,
                (UBaseType_t    )5,
                (TaskHandle_t*  )&CPUask_Handler); 
        #endif
   taskEXIT_CRITICAL(); //退出临界区
}


/*
*********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通信机制
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
 void AppObjCreate (void)
 {
	//创建软件定时器
		xTimers1 = xTimerCreate("Timer", /* 定时器名字 */
						         30, /* 定时器周期,单位时钟节拍 */
								pdTRUE, /* 周期性 */
								 (void *) 0, /* 定时器 ID */                                           
                            vTimerCallback); /* 定时器回调函数 */
													 
			if(xTimers1 == NULL)
				printf("软件定时器创建失败");
			else 
				printf("软件定时器创建成功");
 }
 

 /*
*********************************************************************************************************
*	函 数 名: led0_task
*	功能说明: LED灯的任务
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void led0_task(void *pvParameters)
{
    char temp0[TBUFF_UNIT];    
    while(1)
 {

//    MQTT_Control();                             
    LED0 = !LED0;
//    xTaskNotifyGive(OLEDask_Handler);           //通知LCD显示

         if(SubcribePack_flag == 1)  //如果订阅成功
    {          
      sprintf(temp0,"{\"method\":\"thing.event.property.post\",\"id\":\"203302322\",\"params\":{\
                    \"U\":%f,\"I\":%f,\"Py\":%f,\"Pw\":%f,\"PowerRatio\":%f,\"Nactive_Energy\":%f},\"version\":\"1.0.0\"}",F_DC_V,F_DC_I,F_DC_P,F_DC_P,F_DC_PF,F_DC_E);  
    if(Ping_flag == 0)
    {
       xTaskNotifyGive(OLEDask_Handler);           //通知LCD显示
       MQTT_PublishQs0(P_TOPIC_NAME,temp0,strlen(temp0));   //添加数据，发布给服务器
    }
     }
     
    vTaskDelay(800);
 }
}

 /*
*********************************************************************************************************
*	函 数 名: KEY_task
*	功能说明: 按键任务
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
  u8 key_cout;
void KEY_task(void *pvParameters)
{
	while(1)
	{                           
      key_cout = KEY_Scan(0);
      if(key_cout == 1)
      {
        xTaskNotify(HLW_Handler,BIT_0,eSetBits);
      }
      
      else if(key_cout == 3)
      {
         xTaskNotify(HLW_Handler,BIT_1,eSetBits);
      }
		tp_dev.scan(0); 	  
      	if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
		{            
          if(tp_dev.x<lcddev.width&&tp_dev.y<lcddev.height)
			{	
                 if((tp_dev.x>(star_x+15)&&tp_dev.x<(star_x+80))&&(tp_dev.y>(star_y-80)&&tp_dev.y<(star_y-45)))
				{
                   xTaskNotify(HLW_Handler,BIT_0,eSetBits);
                }
                
                 else if((tp_dev.x>(star_x+130)&&tp_dev.x<(star_x+195))&&(tp_dev.y>(star_y-80)&&tp_dev.y<(star_y-45)))
				{
           xTaskNotify(HLW_Handler,BIT_1,eSetBits);
                }               
            }
        }   
            vTaskDelay(5);
    }
}

void HLW_task(void *pvParameters)
{
   BaseType_t xResult;
   uint32_t ulValue;
   Init_HLW8110();                       //Init_HLW8110初始化
    while(1)
    {
      xResult = xTaskNotifyWait(0x00000000,      
						          0xFFFFFFFF,      
						          &ulValue,        /* 保存ulNotifiedValue到变量ulValue中 */
						          portMAX_DELAY);  /* 最大允许延迟时间 */
        
      if( xResult == pdPASS )
      {
          /* 接收到消息，检测那个位被按下 */
        if((ulValue & BIT_0) != 0)
        {
        taskENTER_CRITICAL(); //进入临界区
        GPIO_ResetBits(GPIOB,GPIO_Pin_5);
        DC_Flage = 1;
        Init_HLW8110();                       //Init_HLW8110初始化
         taskEXIT_CRITICAL(); //退出临界区
        }

        if((ulValue & BIT_1) != 0)
        {
             taskENTER_CRITICAL(); //进入临界区
         GPIO_SetBits(GPIOB,GPIO_Pin_5);
         DC_Flage = 0;
         Init_HLW8110();                       //Init_HLW8110初始化
           taskEXIT_CRITICAL(); //退出临界区
        }
		}
    }
}
/*
*********************************************************************************************************
*	函 数 名: OLED_task
*	功能说明: OLED显示任务
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void OLED_task(void *pvParameters)
{
    char Buff_data[10],parrel_data = 20;

    LCD_Clear(BLACK);
    
    Show_Str(star_x+25,star_y-70,BLUE,BLACK,"DC测量",16,1);
     Show_Str(star_x+140,star_y-70,BLUE,BLACK,"AC测量",16,1);
    //绘制固定栏up
    LCD_Fill(0, 0,lcddev.width,30,LIGHTGRAY);
    Show_Str(50,1,BLUE,LGRAY,"电力参数测量",24,1);

    Show_Str(star_x,star_y+ratio_y*6,BLUE,BLACK,"电压Frequency",16,1);    
    Show_Str(star_x,star_y+ratio_y*5,BLUE,BLACK,"电流有效值",16,1);     
    Show_Str(star_x,star_y+ratio_y*4,BLUE,BLACK,"电压有效值",16,1);
    Show_Str(star_x,star_y+ratio_y*3,BLUE,BLACK,"有功功率",16,1);
    Show_Str(star_x,star_y+ratio_y*2,BLUE,BLACK,"无功功率",16,1);
    Show_Str(star_x,star_y+ratio_y*1,BLUE,BLACK,"功率因数",16,1);
    Show_Str(star_x,star_y,BLUE,BLACK,"电能",16,1);
    
    Show_Str(star_x+190,star_y,LIGHTGREEN,BLACK,"kw.h",16,1);
    Show_Str(star_x+190,star_y+ratio_y*2,LIGHTGREEN,BLACK,"W",16,1);
    Show_Str(star_x+190,star_y+ratio_y*3,LIGHTGREEN,BLACK,"W",16,1);
    Show_Str(star_x+190,star_y+ratio_y*4,LIGHTGREEN,BLACK,"V",16,1);
    Show_Str(star_x+190,star_y+ratio_y*5,LIGHTGREEN,BLACK,"A",16,1);

    LCD_DrawLine(0,star_y+parrel_data,lcddev.width,star_y+parrel_data);                          //画线必须在写文字之后
    LCD_DrawLine(0,star_y+parrel_data+ratio_y*1,lcddev.width,star_y+parrel_data+ratio_y*1);
    LCD_DrawLine(0,star_y+parrel_data+ratio_y*2,lcddev.width,star_y+parrel_data+ratio_y*2);
    LCD_DrawLine(0,star_y+parrel_data+ratio_y*3,lcddev.width,star_y+parrel_data+ratio_y*3);
    LCD_DrawLine(0,star_y+parrel_data+ratio_y*4,lcddev.width,star_y+parrel_data+ratio_y*4);
    LCD_DrawLine(0,star_y+parrel_data+ratio_y*5,lcddev.width,star_y+parrel_data+ratio_y*5);
    LCD_DrawLine(0,star_y+parrel_data+ratio_y*6,lcddev.width,star_y+parrel_data+ratio_y*6);  
    LCD_DrawLine(0,star_y+parrel_data+ratio_y*7,lcddev.width,star_y+parrel_data+ratio_y*7);
        
    LCD_DrawLine(lcddev.width/2,30,lcddev.width/2,lcddev.height);
    LCD_DrawRectangle(star_x+15,star_y-80,star_x+80,star_y-45);
    
    LCD_DrawRectangle(star_x+130,star_y-80,star_x+195,star_y-45);
		while(1)
	{
    
        
//        //任务通知的 二值信号量
//      ulTaskNotifyTake(pdTRUE,  
//						 portMAX_DELAY); /* 无限等待 */
                Calculate_HLW8110_MeterData();     	
     sprintf(Buff_data,"%5.3f",F_DC_E);//浮点型数据转为指定格式的字符串   
     Show_Str(star_x+140,star_y,LIGHTGREEN,BLACK,(u8*)Buff_data,16,0);  
        
     sprintf(Buff_data,"%5.3f",F_DC_PF);//浮点型数据转为指定格式的字符串  
     Show_Str(star_x+140,star_y+ratio_y*1,LIGHTGREEN,BLACK,(u8*)Buff_data,16,0);  
        
     sprintf(Buff_data,"%5.3f",(double)0);//浮点型数据转为指定格式的字符串     
     Show_Str(star_x+140,star_y+ratio_y*2,LIGHTGREEN,BLACK,(u8*)Buff_data,16,0);  
        
     sprintf(Buff_data,"%5.3f",F_DC_P);//浮点型数据转为指定格式的字符串   
     Show_Str(star_x+140,star_y+ratio_y*3,LIGHTGREEN,BLACK,(u8*)Buff_data,16,0); 

     sprintf(Buff_data,"%4.3f",F_DC_V);//浮点型数据转为指定格式的字符串           
     Show_Str(star_x+140,star_y+ratio_y*4,LIGHTGREEN,BLACK,(u8*)Buff_data,16,0);  
     
     sprintf(Buff_data,"%5.4f",F_DC_I);//浮点型数据转为指定格式的字符串   
     Show_Str(star_x+140,star_y+ratio_y*5,LIGHTGREEN,BLACK,(u8*)Buff_data,16,0);  
        
     sprintf(Buff_data,"%5.3f",F_AC_LINE_Freq);//浮点型数据转为指定格式的字符串   
     Show_Str(star_x+140,star_y+ratio_y*6,LIGHTGREEN,BLACK,(u8*)Buff_data,16,0);  
     vTaskDelay(500);
	}
}

/*
*********************************************************************************************************
*	函 数 名: CPU_task
*	功能说明: 任务打印
*	形    参: 无
*	返 回 值: 无               （说明：必须打开定时器2,50us）
*********************************************************************************************************
*/
void CPU_task(void *pvParameters)
{
	uint8_t CPU_RunInfo[400];    //保存任务运行时间信息
    TIM2_Int(719,5);
	while(1)
	{
		 memset(CPU_RunInfo,0,400); //信息缓冲区清零
		 vTaskList((char *)&CPU_RunInfo);  //获取任务运行时间信息
    
    printf("---------------------------------------------\r\n");
    printf("任务名                         任务状态           优先级            剩余栈         任务序号\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n");
    
    memset(CPU_RunInfo,0,400);				//信息缓冲区清零
    
    vTaskGetRunTimeStats((char *)&CPU_RunInfo);
    
    printf("任务名                             运行计数                             使用率\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n\n");
    vTaskDelay(1000);   /* 延时500个tick */	
}
}
/*
*********************************************************************************************************
*	函 数 名: vTimerCallback
*	功能说明: 定时器回调函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void vTimerCallback(xTimerHandle pxTimer)
{
	uint32_t ulTimerID;
    static u32 time_coun = 0;

	configASSERT(pxTimer);  //断言，用于检查错误

//	/* 获取那个定时器时间到 */
	ulTimerID = (uint32_t)pvTimerGetTimerID(pxTimer);
	
	/* 处理定时器0任务 */
	if(ulTimerID == 0)
	{
        time_coun++;
        if(time_coun>=1000)
        {
            time_coun  = 0;
        	switch(Ping_flag)
			{                               //判断Ping_flag的状态
			case 0:										 //如果Ping_flag等于0，表示正常状态，发送Ping报文                   
					MQTT_PingREQ(); 					 //添加Ping报文到发送缓冲区  
					break;
			case 1:	               
                xTimerStop(pxTimer, 0);//首先停止当前的定时器
                xTimerChangePeriod(pxTimer, 100, 0);//修改定时器周期
                xTimerReset(pxTimer, 0);//复位定时器是为了让时间从当前时间开始而不是从之前启动定时器开始
                MQTT_PingREQ();  									 //如果Ping_flag等于2，说明还没有收到服务器回复
                    break;
			case 2:					 
		    case 3:				                         //如果Ping_flag等于3，说明还没有收到服务器回复
            case 4:	
			case 5:				                         //如果Ping_flag等于4，说明还没有收到服务器回复	
					MQTT_PingREQ();  					 //添加Ping报文到发送缓冲区 
					break;
			case 6:										 //如果Ping_flag等于5，说明我们发送了多次ping，均无回复，应该是连接有问题，我们重启连接
					Connect_flag = 0;                    //连接状态置0，表示断开，没连上服务器
                   xTimerStop(pxTimer,0);			
					break;			
		}
		Ping_flag++;           		             		 //Ping_flag自增1，表示又发送了一次ping，期待服务器的回复        
	    }
	}
}
