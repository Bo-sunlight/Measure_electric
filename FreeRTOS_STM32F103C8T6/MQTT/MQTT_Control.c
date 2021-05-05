#include "MQTT_Control.h"
#include "includes.h"
#include "led.h"
#include "timer.h"
#include "wifi.h"	    //包含需要的头文件
#include "mqtt.h"         //包含需要的头文件

/*-------------------------------------------------------------*/
/*                     处理发送缓冲区数据                      */
/*-------------------------------------------------------------*/		
void Deal_TxData(void)
{
		if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr)              //if成立的话，说明发送缓冲区有数据了
			{  	
	 /*
		3种情况可进入if
	 *第1种：0x10 连接报文
	 *第2种：0x82 订阅报文，且ConnectPack_flag置位，表示连接报文成功
	 *第3种：SubcribePack_flag置位，说明连接和订阅均成功，其他报文可发
  */ 
	  if((MQTT_TxDataOutPtr[2]==0x10)||((MQTT_TxDataOutPtr[2]==0x82)&&(ConnectPack_flag==1))||(SubcribePack_flag==1))
		  {    
	   	printf("发送数据:0x%x\r\n",MQTT_TxDataOutPtr[2]);     //串口提示信息
		  MQTT_TxData(MQTT_TxDataOutPtr);                       //发送数据
		  MQTT_TxDataOutPtr += TBUFF_UNIT;                      //指针下移
		  if(MQTT_TxDataOutPtr==MQTT_TxDataEndPtr)              //如果指针到缓冲区尾部了
          {
              	
			MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];                //指针归位到缓冲区开头
          }
	    } 				
     }
}	
/*-------------------------------------------------------------*/
/*                     处理接收缓冲区数据                      */
/*-------------------------------------------------------------*/
void Deal_RxData(void)
{
		if(MQTT_RxDataOutPtr != MQTT_RxDataInPtr)             //if成立的话，说明接收缓冲区有数据了		
			{  												
	          //printf("接收到数据:"); 
				/*                    处理CONNACK报文                   */
				/*       如果第一个字节是0x20，表示收到的是CONNACK报文  */
				/*       判断第4个字节，看看CONNECT报文是否成功         */
				if(MQTT_RxDataOutPtr[2]==0x20)
				{ 
						 switch(MQTT_RxDataOutPtr[5])
						{					
						case 0x00 : printf("CONNECT报文成功\r\n");                            //串口输出信息	
								    ConnectPack_flag = 1;                                        //CONNECT报文成功，订阅报文可发
									break;                                                       //跳出分支case 0x00                                              
						case 0x01 : printf("连接已拒绝，不支持的协议版本，准备重启\r\n");     //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接
									break;                                                       //跳出分支case 0x01   
						case 0x02 : printf("连接已拒绝，不合格的客户端标识符，准备重启\r\n"); //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接
									break;                                                       //跳出分支case 0x02 
						case 0x03 : printf("连接已拒绝，服务端不可用，准备重启\r\n");         //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接
									break;                                                       //跳出分支case 0x03
						case 0x04 : printf("连接已拒绝，无效的用户名或密码，准备重启\r\n");   //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接						
									break;                                                       //跳出分支case 0x04
						case 0x05 : printf("连接已拒绝，未授权，准备重启\r\n");               //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接						
									break;                                                       //跳出分支case 0x05 		
						default   : printf("连接已拒绝，未知状态，准备重启\r\n");             //串口输出信息 
									Connect_flag = 0;                                            //Connect_flag置零，重启连接					
									break;                                                       //跳出分支case default 								
					}								
				 }
				/*                    处理SUBACK报文                    */
				/*       如果第一个字节是0x90，表示收到的是订阅回复报文   */
				else if(MQTT_RxDataOutPtr[2]==0x90)			
				{ 
			     	switch(MQTT_RxDataOutPtr[6])
					{					
						case 0x00 :
						case 0x01 : printf("订阅成功\r\n");            //串口输出信息
							          SubcribePack_flag = 1;                //SubcribePack_flag置1，表示订阅报文成功，其他报文可发送
									      Ping_flag = 0;                        //Ping_flag清零
                                      xTimerStart(xTimers1, 100);
//   								      TIM2_Int_Init(65535,65535);                    //启动30s的PING定时器
									      LED1_State();                         //判断开关状态，并发布给服务器  
									      break;                                //跳出分支                                             
						default   : printf("订阅失败，准备重启\r\n");  //串口输出信息 
									      Connect_flag = 0;                     //Connect_flag置零，重启连接
									      break;                                //跳出分支
					}						
				 }		 
				/*   第一个字节是0xD0，表示收到的是PINGRESP报文*/
				else if(MQTT_RxDataOutPtr[2]==0xD0)
					{ 
					  printf("PING报文回复\r\n"); 		        //串口输出信息 
					    if(Ping_flag==1)                     //如果Ping_flag=1，表示第一次发送
						{ 
							Ping_flag = 0;    				          //要清除Ping_flag标志
						}
						else if(Ping_flag>1)              //如果Ping_flag>1，表示是多次发送了，而且是2s间隔的快速发送
						{ 				  
						  Ping_flag = 0;     				          //要清除Ping_flag标志
                          xTimerStop(xTimers1, 0);//首先停止当前的定时器
                          xTimerChangePeriod(xTimers1, 30, 0);//修改定时器周期,PING定时器重回30s的时间
                          xTimerReset(xTimers1, 0);//复位定时器是为了让时间从当前时间开始而不是从之前启动定时器开始            		 
					    }				
				  }	
					
				/*如果第一个字节是0x30，表示收到的是服务器发来的推送数据*/
				else if(MQTT_RxDataOutPtr[2]==0x30)
					{ 
					printf("服务器等级0推送\r\n"); 		   //串口输出信息 
					MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);  //处理等级0推送数据
				  }				
								
				MQTT_RxDataOutPtr +=RBUFF_UNIT;                     //指针下移
				if(MQTT_RxDataOutPtr==MQTT_RxDataEndPtr)            //如果指针到缓冲区尾部了
					MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];          //指针归位到缓冲区开头                        
			 }
}
/*-------------------------------------------------------------*/
/*                     处理命令缓冲区数据                      */
/*-------------------------------------------------------------*/
void Deal_TxCmd(void)
{ 
	char temp2[TBUFF_UNIT];                   //定义一个临时缓冲区
	char temp3[TBUFF_UNIT];                   //定义一个临时缓冲区
		if(MQTT_CMDOutPtr != MQTT_CMDInPtr)                                               //if成立的话，说明命令缓冲区有数据了	
		{                            		       
				printf("命令:%s\r\n",&MQTT_CMDOutPtr[2]);      
              sprintf(temp2,"\"params\":{\"%s\":1}",Identifier);	
			 sprintf(temp3,"\"params\":{\"%s\":0}",Identifier);	
				if(strstr((char *)MQTT_CMDOutPtr+2,temp2))          //如果搜索到"params":{"PowerSwitch":1}说明服务器下发打开开关1
					{	       	
					LED0 = 0;                                                                   //打开LED1
					LED1_State();                                                               //判断开关状态，并发布给服务器  
				  }
					
					else if(strstr((char *)MQTT_CMDOutPtr+2,temp3)) //如果搜索到"params":{"PowerSwitch":0}说明服务器下发关闭开关1
					{   
					LED0 = 1;                                                                  //关闭LED1
					LED1_State();                                                              //判断开关状态，并发布给服务器  
				 }
				MQTT_CMDOutPtr += CBUFF_UNIT;                             	                  //指针下移
				if(MQTT_CMDOutPtr==MQTT_CMDEndPtr)           	                               //如果指针到缓冲区尾部了
					MQTT_CMDOutPtr = MQTT_CMDBuf[0];          	                               //指针归位到缓冲区开头				
		}
}

void MQTT_Control(void)
{
	if(Connect_flag==1)
	{
		Deal_TxData();
	    Deal_RxData();
		Deal_TxCmd();
	}		
  else 
	 Connet_Wifi();
}

/*--------------------------------------------------------------------*/
/*      Connect_flag=0同服务器断开了连接,我们要重启连接服务器         */
/*--------------------------------------------------------------------*/
void Connet_Wifi(void)
{
  		printf("需要连接服务器\r\n");                    //串口输出信息
			TIM_Cmd(TIM2,DISABLE);                           //关闭TIM4 
			TIM_Cmd(TIM3,DISABLE);                           //关闭TIM3  
			WiFi_RxCounter=0;                                //WiFi接收数据量变量清零                        
			memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);          //清空WiFi接收缓冲区 
			if(WiFi_Connect_IoTServer()==0)
				{   			                                     //如果WiFi连接云服务器函数返回0，表示正确，进入if
				printf("建立TCP连接成功\r\n");                 //串口输出信息
				Connect_flag = 1;                             //Connect_flag置1，表示连接成功	
				WiFi_RxCounter=0;                             //WiFi接收数据量变量清零                        
				memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);       //清空WiFi接收缓冲区 
                    
 	MQTT_Buff_Init();                             //初始化发送缓冲区        
                    
				}		
}
	

