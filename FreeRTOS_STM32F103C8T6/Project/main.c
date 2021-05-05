/*  
*****************************************************************
*                           STM32F103C8T6                                  
*****************************************************************
*/
#include "includes.h"
u8 DC_Flage = 0;
//������
TaskHandle_t LED0Task_Handler; 
TaskHandle_t KEYTask_Handler;
TaskHandle_t CPUask_Handler;
TaskHandle_t OLEDask_Handler;
TimerHandle_t xTimers1 = {NULL};
//�źž��


//������
void led0_task(void *pvParameters);
void KEY_task(void *pvParameters);
void CPU_task(void *pvParameters);
void OLED_task(void *pvParameters);
void vTimerCallback(xTimerHandle pxTimer);

static void AppTaskCreate (void);
static void AppObjCreate (void);

int main(void)
{	
    /* �رմ󲿷��ж� */ 
	__set_PRIMASK(1);                //Ҫ�жϵĺ�����Ҫ����Bsp�����У������ж��޷�ִ��
	
	/* Ӳ����ʼ�� */
	bsp_Init();
	
	/* �������� */
	AppTaskCreate();
	
	/* ��������ͨ�Ż��� */
	 AppObjCreate();
	
	 /* �������ȣ���ʼִ������ */
   vTaskStartScheduler();
	
/* 
 ���ϵͳ���������ǲ������е�����ģ����е����Ｋ�п��������ڶ�ʱ��������߿��������
 heap �ռ䲻����ɴ���ʧ�ܣ���Ҫ�Ӵ� FreeRTOSConfig.h �ļ��ж���� heap ��С��
 #define configTOTAL_HEAP_SIZE ( ( size_t ) ( 8 * 1024 ) )
*/
   while(1);      
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AppTaskCreate (void)
{
 taskENTER_CRITICAL(); //�����ٽ���
	
	  //���� LED0 ����
	 xTaskCreate((TaskFunction_t )led0_task, 
                 (const char* )"led0_task", 
                 (uint16_t )512, 
								 (void* )NULL,
								 (UBaseType_t )5,
								 (TaskHandle_t* )&LED0Task_Handler); 
 
     //����KEY����
    xTaskCreate((TaskFunction_t )KEY_task,     
                (const char*    )"KEY_task",   
                (uint16_t       )256, 
                (void*          )NULL,
                (UBaseType_t    )1,
                (TaskHandle_t*  )&KEYTask_Handler);   
				                
	//����OLED����
    xTaskCreate((TaskFunction_t )OLED_task,     
                (const char*    )"OLED_task",   
                (uint16_t       )512, 
                (void*          )NULL,
                (UBaseType_t    )5,
                (TaskHandle_t*  )&OLEDask_Handler);  
                
	
		#if 0
	//����CPU����
    xTaskCreate((TaskFunction_t )CPU_task,     
                (const char*    )"CPU_task",   
                (uint16_t       )256, 
                (void*          )NULL,
                (UBaseType_t    )5,
                (TaskHandle_t*  )&CPUask_Handler); 
        #endif
   taskEXIT_CRITICAL(); //�˳��ٽ���
}


/*
*********************************************************************************************************
*	�� �� ��: AppObjCreate
*	����˵��: ��������ͨ�Ż���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
 void AppObjCreate (void)
 {
	//����������ʱ��
		xTimers1 = xTimerCreate("Timer", /* ��ʱ������ */
						         30, /* ��ʱ������,��λʱ�ӽ��� */
								pdTRUE, /* ������ */
								 (void *) 0, /* ��ʱ�� ID */
                            vTimerCallback); /* ��ʱ���ص����� */
													 
			if(xTimers1 == NULL)
				printf("������ʱ������ʧ��");
			else 
				printf("������ʱ�������ɹ�");
 }
 

 /*
*********************************************************************************************************
*	�� �� ��: led0_task
*	����˵��: LED�Ƶ�����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void led0_task(void *pvParameters)
{
   char temp0[TBUFF_UNIT];
    while(1)
 {

//    MQTT_Control();                             
         
    LED0 = !LED0;
    xTaskNotifyGive(OLEDask_Handler);           //֪ͨLCD��ʾ
     
         if(SubcribePack_flag == 1)  //������ĳɹ�
    {          
      sprintf(temp0,"{\"method\":\"thing.event.property.post\",\"id\":\"203302322\",\"params\":{\
                    \"U\":%f,\"I\":%f,\"Py\":%f,\"Pw\":%f,\"PowerRatio\":%f,\"Nactive_Energy\":%f},\"version\":\"1.0.0\"}",F_DC_V,F_DC_I,F_DC_P,F_DC_P,F_DC_PF,F_DC_E);  
    if(Ping_flag == 0)
    {
       xTaskNotifyGive(OLEDask_Handler);           //֪ͨLCD��ʾ
       MQTT_PublishQs0(P_TOPIC_NAME,temp0,strlen(temp0));   //�������ݣ�������������
    }
     }
     
         vTaskDelay(800);
 }
}

 /*
*********************************************************************************************************
*	�� �� ��: KEY_task
*	����˵��: ��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
  u8 key_cout;
void KEY_task(void *pvParameters)
{
      Init_HLW8110();                       //Init_HLW8110��ʼ��
	while(1)
	{                           
      key_cout = KEY_Scan(0);
      if(key_cout == 1)
      {
        DC_Flage = 1;
        Init_HLW8110();                       //Init_HLW8110��ʼ��
      }
      
      else if(key_cout == 2)
      {
       DC_Flage = 0;
       Init_HLW8110();                       //Init_HLW8110��ʼ��
      }
     else     
        Calculate_HLW8110_MeterData();    
      
      vTaskDelay(1000);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: OLED_task
*	����˵��: OLED��ʾ����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void OLED_task(void *pvParameters)
{
    char Buff_data[10];
    u8 star_x = 10,star_y = 50,ratio_y = 50;
    LCD_Clear(BLACK);
    //���ƹ̶���up
    LCD_Fill(0, 0,lcddev.width,30,LIGHTGRAY);
    Show_Str(50,1,BLUE,LGRAY,"������������",24,1);

    Show_Str(star_x,star_y+ratio_y*5,BLUE,BLACK,"������Чֵ",16,1);     
    Show_Str(star_x,star_y+ratio_y*4,BLUE,BLACK,"��ѹ��Чֵ",16,1);
    Show_Str(star_x,star_y+ratio_y*3,BLUE,BLACK,"�й�����",16,1);
    Show_Str(star_x,star_y+ratio_y*2,BLUE,BLACK,"�޹�����",16,1);
    Show_Str(star_x,star_y+ratio_y*1,BLUE,BLACK,"��������",16,1);
    Show_Str(star_x,star_y,BLUE,BLACK,"����",16,1);

    LCD_DrawLine(0,star_y+32,lcddev.width,star_y+32);
    LCD_DrawLine(0,star_y+32+ratio_y*1,lcddev.width,star_y+32+ratio_y*1);
    LCD_DrawLine(0,star_y+32+ratio_y*2,lcddev.width,star_y+32+ratio_y*2);
    LCD_DrawLine(0,star_y+32+ratio_y*3,lcddev.width,star_y+32+ratio_y*3);
    LCD_DrawLine(0,star_y+32+ratio_y*4,lcddev.width,star_y+32+ratio_y*4);
    
    LCD_DrawLine(lcddev.width/2,30,lcddev.width/2,lcddev.height);

		while(1)
	{
        //����֪ͨ�� ��ֵ�ź���
      ulTaskNotifyTake(pdTRUE,  
						 portMAX_DELAY); /* ���޵ȴ� */
     sprintf(Buff_data,"%5.3f kw.h",F_DC_E);//����������תΪָ����ʽ���ַ���   
     Show_Str(star_x+140,star_y,LIGHTGREEN,BLACK,(u8*)Buff_data,16,0);  
        
     sprintf(Buff_data,"%5.3f",F_DC_PF);//����������תΪָ����ʽ���ַ���  
     Show_Str(star_x+140,star_y+ratio_y*1,LIGHTGREEN,BLACK,(u8*)Buff_data,16,0);  
        
     sprintf(Buff_data,"%5.3f W",(double)0);//����������תΪָ����ʽ���ַ���     
     Show_Str(star_x+140,star_y+ratio_y*2,LIGHTGREEN,BLACK,(u8*)Buff_data,16,0);  
        
     sprintf(Buff_data,"%5.3f W",F_DC_P);//����������תΪָ����ʽ���ַ���   
     Show_Str(star_x+140,star_y+ratio_y*3,LIGHTGREEN,BLACK,(u8*)Buff_data,16,0); 

     sprintf(Buff_data,"%5.3f V",F_DC_V);//����������תΪָ����ʽ���ַ���           
     Show_Str(star_x+140,star_y+ratio_y*4,LIGHTGREEN,BLACK,(u8*)Buff_data,16,0);  
     
     sprintf(Buff_data,"%5.3f A",F_DC_I);//����������תΪָ����ʽ���ַ���   
     Show_Str(star_x+140,star_y+ratio_y*5,LIGHTGREEN,BLACK,(u8*)Buff_data,16,0);  
	}
}


/*
*********************************************************************************************************
*	�� �� ��: CPU_task
*	����˵��: �����ӡ
*	��    ��: ��
*	�� �� ֵ: ��               ��˵��������򿪶�ʱ��2,50us��
*********************************************************************************************************
*/
void CPU_task(void *pvParameters)
{
	uint8_t CPU_RunInfo[400];    //������������ʱ����Ϣ
    TIM2_Int(719,5);
	while(1)
	{
		 memset(CPU_RunInfo,0,400); //��Ϣ����������
		 vTaskList((char *)&CPU_RunInfo);  //��ȡ��������ʱ����Ϣ
    
    printf("---------------------------------------------\r\n");
    printf("������                         ����״̬           ���ȼ�            ʣ��ջ         �������\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n");
    
    memset(CPU_RunInfo,0,400);				//��Ϣ����������
    
    vTaskGetRunTimeStats((char *)&CPU_RunInfo);
    
    printf("������                             ���м���                             ʹ����\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n\n");
    vTaskDelay(1000);   /* ��ʱ500��tick */	
}
}
/*
*********************************************************************************************************
*	�� �� ��: vTimerCallback
*	����˵��: ��ʱ���ص�����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void vTimerCallback(xTimerHandle pxTimer)
{
	uint32_t ulTimerID;
    static u32 time_coun = 0;

	configASSERT(pxTimer);  //���ԣ����ڼ�����

//	/* ��ȡ�Ǹ���ʱ��ʱ�䵽 */
	ulTimerID = (uint32_t)pvTimerGetTimerID(pxTimer);
	
	/* ������ʱ��0���� */
	if(ulTimerID == 0)
	{
        time_coun++;
        if(time_coun>=1000)
        {
            time_coun  = 0;
        	switch(Ping_flag)
			{                               //�ж�Ping_flag��״̬
			case 0:										 //���Ping_flag����0����ʾ����״̬������Ping����                   
					MQTT_PingREQ(); 					 //����Ping���ĵ����ͻ�����  
					break;
			case 1:	               
                xTimerStop(pxTimer, 0);//����ֹͣ��ǰ�Ķ�ʱ��
                xTimerChangePeriod(pxTimer, 100, 0);//�޸Ķ�ʱ������
                xTimerReset(pxTimer, 0);//��λ��ʱ����Ϊ����ʱ��ӵ�ǰʱ�俪ʼ�����Ǵ�֮ǰ������ʱ����ʼ
                MQTT_PingREQ();  									 //���Ping_flag����2��˵����û���յ��������ظ�
                    break;
			case 2:					 
		    case 3:				                         //���Ping_flag����3��˵����û���յ��������ظ�
            case 4:	
			case 5:				                         //���Ping_flag����4��˵����û���յ��������ظ�	
					MQTT_PingREQ();  					 //����Ping���ĵ����ͻ����� 
					break;
			case 6:										 //���Ping_flag����5��˵�����Ƿ����˶��ping�����޻ظ���Ӧ�������������⣬������������
					Connect_flag = 0;                    //����״̬��0����ʾ�Ͽ���û���Ϸ�����
                   xTimerStop(pxTimer,0);			
					break;			
		}
		Ping_flag++;           		             		 //Ping_flag����1����ʾ�ַ�����һ��ping���ڴ��������Ļظ�        
	    }
	}
}