#include "includes.h"
#include "timer.h"
/*
*************************************
* �������ܣ�����2�жϷ�����
* ��ڲ�������
* ����  ֵ����
*************************************
*/
void USART2_IRQHandler(void)
{
     USART2_IRQ_User();
}

void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//���յ�����
	{	 
	 //���USART_IT_RXNE��־��λ����ʾ�����ݵ��ˣ�����if��֧
		if(Connect_flag==0){                                //���Connect_flag����0����ǰ��û�����ӷ�����������ָ������״̬
			if(USART3->DR){                                 //����ָ������״̬ʱ������ֵ�ű��浽������	
				Usart3_RxBuff[Usart3_RxCounter]=USART3->DR; //���浽������	
				Usart3_RxCounter ++;                        //ÿ����1���ֽڵ����ݣ�Usart2_RxCounter��1����ʾ���յ���������+1 
			}		
		}
		else
			{		                                            //��֮Connect_flag����1�������Ϸ�������	
			Usart3_RxBuff[Usart3_RxCounter] = USART3->DR;   //�ѽ��յ������ݱ��浽Usart3_RxBuff��				
			if(Usart3_RxCounter == 0)                       //���Usart3_RxCounter����0����ʾ�ǽ��յĵ�1�����ݣ�����if��֧			
			{    						
				TIM_Cmd(TIM3,ENABLE); 
			}else{                        				        	//else��֧����ʾ��Usart2_RxCounter������0�����ǽ��յĵ�һ������
				TIM_SetCounter(TIM3,0);  
			}	
			Usart3_RxCounter ++;         				           //ÿ����1���ֽڵ����ݣ�Usart2_RxCounter��1����ʾ���յ���������+1 
		}
 }										 
}  


/*
*************************************
* �������ܣ�TIM2�жϷ�����
* ��ڲ�������
* ����  ֵ����
*************************************
*/
#if  TIM2_INTERRUPT
volatile uint32_t CPU_RunTime = 0UL;
void TIM2_IRQHandler(void)   //TIM2�ж�
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
          CPU_RunTime++;
         TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
      
}
#endif

/*
*************************************
* �������ܣ�TIM3�жϷ�����
* ��ڲ�������
* ����  ֵ����
*************************************
*/
#if  TIM3_INTERRUPT
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
		            //���TIM_IT_Update��λ����ʾTIM3����жϣ�����if	
		
		memcpy(&MQTT_RxDataInPtr[2],Usart3_RxBuff,Usart3_RxCounter);  //�������ݵ����ջ�����
		MQTT_RxDataInPtr[0] = Usart3_RxCounter/256;                   //��¼���ݳ��ȸ��ֽ�
		MQTT_RxDataInPtr[1] = Usart3_RxCounter%256;                   //��¼���ݳ��ȵ��ֽ�
		MQTT_RxDataInPtr+=RBUFF_UNIT;                                  //ָ������
		if(MQTT_RxDataInPtr==MQTT_RxDataEndPtr)                       //���ָ�뵽������β����
		MQTT_RxDataInPtr = MQTT_RxDataBuf[0];                     //ָ���λ����������ͷ
		Usart3_RxCounter = 0;                                         //����2������������������
		
		TIM_SetCounter(TIM2, 0);                                      //���㶨ʱ��2�����������¼�ʱping������ʱ��
		TIM_Cmd(TIM3, DISABLE);                        				  //�ر�TIM3��ʱ��
		TIM_SetCounter(TIM3, 0);                        			  //���㶨ʱ��3������
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);     			  //���TIM3����жϱ�־ 	
	}
}
#endif

/*
*************************************
* �������ܣ�TIM4�жϷ�����
* ��ڲ�������
* ����  ֵ����
*************************************
*/
void TIM4_IRQHandler(void)
{ 		    		  			    
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{    				   				     	    	
	}				   
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
}

/*
*************************************
* �������ܣ�TIM5�жϷ�����
* ��ڲ�������
* ����  ֵ����
*************************************
*/
void TIM5_IRQHandler(void)
{ 		    		  			    
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{    				   				     	    	
	}				   
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
}




