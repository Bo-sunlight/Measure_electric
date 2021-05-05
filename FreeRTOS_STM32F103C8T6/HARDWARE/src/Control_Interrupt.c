#include "includes.h"
#include "timer.h"
/*
*************************************
* 函数功能：串口2中断服务函数
* 入口参数：无
* 返回  值：无
*************************************
*/
void USART2_IRQHandler(void)
{
     USART2_IRQ_User();
}

void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//接收到数据
	{	 
	 //如果USART_IT_RXNE标志置位，表示有数据到了，进入if分支
		if(Connect_flag==0){                                //如果Connect_flag等于0，当前还没有连接服务器，处于指令配置状态
			if(USART3->DR){                                 //处于指令配置状态时，非零值才保存到缓冲区	
				Usart3_RxBuff[Usart3_RxCounter]=USART3->DR; //保存到缓冲区	
				Usart3_RxCounter ++;                        //每接收1个字节的数据，Usart2_RxCounter加1，表示接收的数据总量+1 
			}		
		}
		else
			{		                                            //反之Connect_flag等于1，连接上服务器了	
			Usart3_RxBuff[Usart3_RxCounter] = USART3->DR;   //把接收到的数据保存到Usart3_RxBuff中				
			if(Usart3_RxCounter == 0)                       //如果Usart3_RxCounter等于0，表示是接收的第1个数据，进入if分支			
			{    						
				TIM_Cmd(TIM3,ENABLE); 
			}else{                        				        	//else分支，表示果Usart2_RxCounter不等于0，不是接收的第一个数据
				TIM_SetCounter(TIM3,0);  
			}	
			Usart3_RxCounter ++;         				           //每接收1个字节的数据，Usart2_RxCounter加1，表示接收的数据总量+1 
		}
 }										 
}  


/*
*************************************
* 函数功能：TIM2中断服务函数
* 入口参数：无
* 返回  值：无
*************************************
*/
#if  TIM2_INTERRUPT
volatile uint32_t CPU_RunTime = 0UL;
void TIM2_IRQHandler(void)   //TIM2中断
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
          CPU_RunTime++;
         TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
      
}
#endif

/*
*************************************
* 函数功能：TIM3中断服务函数
* 入口参数：无
* 返回  值：无
*************************************
*/
#if  TIM3_INTERRUPT
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		            //如果TIM_IT_Update置位，表示TIM3溢出中断，进入if	
		
		memcpy(&MQTT_RxDataInPtr[2],Usart3_RxBuff,Usart3_RxCounter);  //拷贝数据到接收缓冲区
		MQTT_RxDataInPtr[0] = Usart3_RxCounter/256;                   //记录数据长度高字节
		MQTT_RxDataInPtr[1] = Usart3_RxCounter%256;                   //记录数据长度低字节
		MQTT_RxDataInPtr+=RBUFF_UNIT;                                  //指针下移
		if(MQTT_RxDataInPtr==MQTT_RxDataEndPtr)                       //如果指针到缓冲区尾部了
		MQTT_RxDataInPtr = MQTT_RxDataBuf[0];                     //指针归位到缓冲区开头
		Usart3_RxCounter = 0;                                         //串口2接收数据量变量清零
		
		TIM_SetCounter(TIM2, 0);                                      //清零定时器2计数器，重新计时ping包发送时间
		TIM_Cmd(TIM3, DISABLE);                        				  //关闭TIM3定时器
		TIM_SetCounter(TIM3, 0);                        			  //清零定时器3计数器
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);     			  //清除TIM3溢出中断标志 	
	}
}
#endif

/*
*************************************
* 函数功能：TIM4中断服务函数
* 入口参数：无
* 返回  值：无
*************************************
*/
void TIM4_IRQHandler(void)
{ 		    		  			    
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{    				   				     	    	
	}				   
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
}

/*
*************************************
* 函数功能：TIM5中断服务函数
* 入口参数：无
* 返回  值：无
*************************************
*/
void TIM5_IRQHandler(void)
{ 		    		  			    
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{    				   				     	    	
	}				   
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
}




