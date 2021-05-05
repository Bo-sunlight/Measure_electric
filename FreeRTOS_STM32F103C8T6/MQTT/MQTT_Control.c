#include "MQTT_Control.h"
#include "includes.h"
#include "led.h"
#include "timer.h"
#include "wifi.h"	    //������Ҫ��ͷ�ļ�
#include "mqtt.h"         //������Ҫ��ͷ�ļ�

/*-------------------------------------------------------------*/
/*                     �����ͻ���������                      */
/*-------------------------------------------------------------*/		
void Deal_TxData(void)
{
		if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr)              //if�����Ļ���˵�����ͻ�������������
			{  	
	 /*
		3������ɽ���if
	 *��1�֣�0x10 ���ӱ���
	 *��2�֣�0x82 ���ı��ģ���ConnectPack_flag��λ����ʾ���ӱ��ĳɹ�
	 *��3�֣�SubcribePack_flag��λ��˵�����ӺͶ��ľ��ɹ����������Ŀɷ�
  */ 
	  if((MQTT_TxDataOutPtr[2]==0x10)||((MQTT_TxDataOutPtr[2]==0x82)&&(ConnectPack_flag==1))||(SubcribePack_flag==1))
		  {    
	   	printf("��������:0x%x\r\n",MQTT_TxDataOutPtr[2]);     //������ʾ��Ϣ
		  MQTT_TxData(MQTT_TxDataOutPtr);                       //��������
		  MQTT_TxDataOutPtr += TBUFF_UNIT;                      //ָ������
		  if(MQTT_TxDataOutPtr==MQTT_TxDataEndPtr)              //���ָ�뵽������β����
          {
              	
			MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];                //ָ���λ����������ͷ
          }
	    } 				
     }
}	
/*-------------------------------------------------------------*/
/*                     ������ջ���������                      */
/*-------------------------------------------------------------*/
void Deal_RxData(void)
{
		if(MQTT_RxDataOutPtr != MQTT_RxDataInPtr)             //if�����Ļ���˵�����ջ�������������		
			{  												
	          //printf("���յ�����:"); 
				/*                    ����CONNACK����                   */
				/*       �����һ���ֽ���0x20����ʾ�յ�����CONNACK����  */
				/*       �жϵ�4���ֽڣ�����CONNECT�����Ƿ�ɹ�         */
				if(MQTT_RxDataOutPtr[2]==0x20)
				{ 
						 switch(MQTT_RxDataOutPtr[5])
						{					
						case 0x00 : printf("CONNECT���ĳɹ�\r\n");                            //���������Ϣ	
								    ConnectPack_flag = 1;                                        //CONNECT���ĳɹ������ı��Ŀɷ�
									break;                                                       //������֧case 0x00                                              
						case 0x01 : printf("�����Ѿܾ�����֧�ֵ�Э��汾��׼������\r\n");     //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x01   
						case 0x02 : printf("�����Ѿܾ������ϸ�Ŀͻ��˱�ʶ����׼������\r\n"); //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x02 
						case 0x03 : printf("�����Ѿܾ�������˲����ã�׼������\r\n");         //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x03
						case 0x04 : printf("�����Ѿܾ�����Ч���û��������룬׼������\r\n");   //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������						
									break;                                                       //������֧case 0x04
						case 0x05 : printf("�����Ѿܾ���δ��Ȩ��׼������\r\n");               //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������						
									break;                                                       //������֧case 0x05 		
						default   : printf("�����Ѿܾ���δ֪״̬��׼������\r\n");             //���������Ϣ 
									Connect_flag = 0;                                            //Connect_flag���㣬��������					
									break;                                                       //������֧case default 								
					}								
				 }
				/*                    ����SUBACK����                    */
				/*       �����һ���ֽ���0x90����ʾ�յ����Ƕ��Ļظ�����   */
				else if(MQTT_RxDataOutPtr[2]==0x90)			
				{ 
			     	switch(MQTT_RxDataOutPtr[6])
					{					
						case 0x00 :
						case 0x01 : printf("���ĳɹ�\r\n");            //���������Ϣ
							          SubcribePack_flag = 1;                //SubcribePack_flag��1����ʾ���ı��ĳɹ����������Ŀɷ���
									      Ping_flag = 0;                        //Ping_flag����
                                      xTimerStart(xTimers1, 100);
//   								      TIM2_Int_Init(65535,65535);                    //����30s��PING��ʱ��
									      LED1_State();                         //�жϿ���״̬����������������  
									      break;                                //������֧                                             
						default   : printf("����ʧ�ܣ�׼������\r\n");  //���������Ϣ 
									      Connect_flag = 0;                     //Connect_flag���㣬��������
									      break;                                //������֧
					}						
				 }		 
				/*   ��һ���ֽ���0xD0����ʾ�յ�����PINGRESP����*/
				else if(MQTT_RxDataOutPtr[2]==0xD0)
					{ 
					  printf("PING���Ļظ�\r\n"); 		        //���������Ϣ 
					    if(Ping_flag==1)                     //���Ping_flag=1����ʾ��һ�η���
						{ 
							Ping_flag = 0;    				          //Ҫ���Ping_flag��־
						}
						else if(Ping_flag>1)              //���Ping_flag>1����ʾ�Ƕ�η����ˣ�������2s����Ŀ��ٷ���
						{ 				  
						  Ping_flag = 0;     				          //Ҫ���Ping_flag��־
                          xTimerStop(xTimers1, 0);//����ֹͣ��ǰ�Ķ�ʱ��
                          xTimerChangePeriod(xTimers1, 30, 0);//�޸Ķ�ʱ������,PING��ʱ���ػ�30s��ʱ��
                          xTimerReset(xTimers1, 0);//��λ��ʱ����Ϊ����ʱ��ӵ�ǰʱ�俪ʼ�����Ǵ�֮ǰ������ʱ����ʼ            		 
					    }				
				  }	
					
				/*�����һ���ֽ���0x30����ʾ�յ����Ƿ�������������������*/
				else if(MQTT_RxDataOutPtr[2]==0x30)
					{ 
					printf("�������ȼ�0����\r\n"); 		   //���������Ϣ 
					MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);  //����ȼ�0��������
				  }				
								
				MQTT_RxDataOutPtr +=RBUFF_UNIT;                     //ָ������
				if(MQTT_RxDataOutPtr==MQTT_RxDataEndPtr)            //���ָ�뵽������β����
					MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];          //ָ���λ����������ͷ                        
			 }
}
/*-------------------------------------------------------------*/
/*                     ���������������                      */
/*-------------------------------------------------------------*/
void Deal_TxCmd(void)
{ 
	char temp2[TBUFF_UNIT];                   //����һ����ʱ������
	char temp3[TBUFF_UNIT];                   //����һ����ʱ������
		if(MQTT_CMDOutPtr != MQTT_CMDInPtr)                                               //if�����Ļ���˵�����������������	
		{                            		       
				printf("����:%s\r\n",&MQTT_CMDOutPtr[2]);      
              sprintf(temp2,"\"params\":{\"%s\":1}",Identifier);	
			 sprintf(temp3,"\"params\":{\"%s\":0}",Identifier);	
				if(strstr((char *)MQTT_CMDOutPtr+2,temp2))          //���������"params":{"PowerSwitch":1}˵���������·��򿪿���1
					{	       	
					LED0 = 0;                                                                   //��LED1
					LED1_State();                                                               //�жϿ���״̬����������������  
				  }
					
					else if(strstr((char *)MQTT_CMDOutPtr+2,temp3)) //���������"params":{"PowerSwitch":0}˵���������·��رտ���1
					{   
					LED0 = 1;                                                                  //�ر�LED1
					LED1_State();                                                              //�жϿ���״̬����������������  
				 }
				MQTT_CMDOutPtr += CBUFF_UNIT;                             	                  //ָ������
				if(MQTT_CMDOutPtr==MQTT_CMDEndPtr)           	                               //���ָ�뵽������β����
					MQTT_CMDOutPtr = MQTT_CMDBuf[0];          	                               //ָ���λ����������ͷ				
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
/*      Connect_flag=0ͬ�������Ͽ�������,����Ҫ�������ӷ�����         */
/*--------------------------------------------------------------------*/
void Connet_Wifi(void)
{
  		printf("��Ҫ���ӷ�����\r\n");                    //���������Ϣ
			TIM_Cmd(TIM2,DISABLE);                           //�ر�TIM4 
			TIM_Cmd(TIM3,DISABLE);                           //�ر�TIM3  
			WiFi_RxCounter=0;                                //WiFi������������������                        
			memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);          //���WiFi���ջ����� 
			if(WiFi_Connect_IoTServer()==0)
				{   			                                     //���WiFi�����Ʒ�������������0����ʾ��ȷ������if
				printf("����TCP���ӳɹ�\r\n");                 //���������Ϣ
				Connect_flag = 1;                             //Connect_flag��1����ʾ���ӳɹ�	
				WiFi_RxCounter=0;                             //WiFi������������������                        
				memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);       //���WiFi���ջ����� 
                    
 	MQTT_Buff_Init();                             //��ʼ�����ͻ�����        
                    
				}		
}
	

