#include "ds1302.h"
#include "delay.h"

//��ʼ��ʱ�䶨��
u8 time_buf[8] = {0x20,0x17,0x01,0x01,0x23,0x59,0x55,0x02}; //��ʼʱ��2010��6��1��23��59��55�� ���ڶ�

//DS1302��ʼ��
void DS1302_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_1);                 //PB1����
	GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_10); //PB0��PB10�õ�
}

//��DS1302д��һ���ֽ�����
void DS1302_Write_Byte(u8 addr, u8 data)
{
  u8 i;
	DS1302_RST = 0;      //ֹͣDS1302����
	delay_us(10);
	DS1302_RST = 1;      //����DS1302����
	addr = addr & 0xFE;  //���λ���㣬д����
	DS1302_IO_OUT();
	for(i = 0; i < 8; i ++)  //д��ַ
	{
	  if (addr & 0x01)
			DS1302_DATA_OUT = 1;
		else
			DS1302_DATA_OUT = 0;
		
		DS1302_SCK = 1;    //����ʱ��
		delay_us(10);
		DS1302_SCK = 0;
		addr = addr>>1;
	}
	for (i = 0; i < 8; i ++) //д����
	{
	  if(data & 0x01)
			DS1302_DATA_OUT = 1;
		else
			DS1302_DATA_OUT = 0;
		
		DS1302_SCK = 1;   //����ʱ��
		delay_us(10);
		DS1302_SCK = 0;
	  data = data>>1;
	}
	DS1302_RST = 0;      //ֹͣDS1302����
}

//��DS1302����һ���ֽ�����
u8 DS1302_Read_Byte(u8 addr)
{
  u8 i,temp;
	DS1302_RST = 0;      //ֹͣDS1302����
	delay_us(10);
	DS1302_RST = 1;      //����DS1302����
	addr = addr | 0x01;  //���λ�øߣ�������
	DS1302_IO_OUT();
	for(i = 0; i < 8; i ++)  //д��ַ
	{
	  if (addr & 0x01)
			DS1302_DATA_OUT = 1;
		else
			DS1302_DATA_OUT = 0;
		
		DS1302_SCK = 1;    //����ʱ��
		delay_us(10);
		DS1302_SCK = 0;
		addr = addr>>1;
	}
	DS1302_IO_IN();
	for (i = 0; i < 8; i ++) //������
	{
	  temp = temp >> 1;
		if(DS1302_DATA_IN)
			temp |= 0x80;
		else
			temp &= 0x7F;
		
		DS1302_SCK = 1;   //����ʱ��
		delay_us(10);
		DS1302_SCK = 0;
	}
	DS1302_RST = 0;      //ֹͣDS1302����
	return temp;
}

//��DS1302д��ʱ������
void DS1302_Write_Time(void)
{
  DS1302_Write_Byte(ds1302_control_add, 0x00);      //�ر�д����
  DS1302_Write_Byte(ds1302_sec_add, 0x80);          //��ͣʱ��
	//DS1302_Write_Byte(ds1302_charger_add, 0xA9);      //������
	DS1302_Write_Byte(ds1302_year_add,time_buf[1]);		//�� 
	DS1302_Write_Byte(ds1302_month_add,time_buf[2]);	//�� 
	DS1302_Write_Byte(ds1302_date_add,time_buf[3]);		//�� 
	DS1302_Write_Byte(ds1302_hr_add,time_buf[4]);		  //ʱ 
	DS1302_Write_Byte(ds1302_min_add,time_buf[5]);		//��
	DS1302_Write_Byte(ds1302_sec_add,time_buf[6]);		//��
	DS1302_Write_Byte(ds1302_day_add,time_buf[7]);		//�� 
	DS1302_Write_Byte(ds1302_control_add,0x80);		    //��д���� 
}

//��DS302����ʱ������
void DS1302_Read_Time(void)  
{
	time_buf[1] = DS1302_Read_Byte(ds1302_year_add);		   //�� 
	time_buf[2] = DS1302_Read_Byte(ds1302_month_add);		   //�� 
	time_buf[3] = DS1302_Read_Byte(ds1302_date_add);		   //�� 
	time_buf[4] = DS1302_Read_Byte(ds1302_hr_add);		     //ʱ 
	time_buf[5] = DS1302_Read_Byte(ds1302_min_add);		     //�� 
	time_buf[6] = (DS1302_Read_Byte(ds1302_sec_add))&0x7f; //�룬������ĵ�7λ�����ⳬ��59
	time_buf[7] = DS1302_Read_Byte(ds1302_day_add);		     //�� 	
}

//DS1302���ϲ㷵��ʱ������
void DS1302_Get_Time(u8 *time)
{
	DS1302_Read_Time();
	time[0]=(time_buf[0]>>4);   //��   
  time[1]=(time_buf[0]&0x0f);
   
  time[2]=(time_buf[1]>>4);   
  time[3]=(time_buf[1]&0x0f);
  
  time[4]=(time_buf[2]>>4);   //��  
  time[5]=(time_buf[2]&0x0f);

  time[6]=(time_buf[3]>>4);   //��   
  time[7]=(time_buf[3]&0x0f);
   
  time[8]=(time_buf[7]&0x07); //����
   
  time[9]=(time_buf[4]>>4);   //ʱ   
  time[10]=(time_buf[4]&0x0f);   

  time[11]=(time_buf[5]>>4);  //��   
  time[12]=(time_buf[5]&0x0f);   

  time[13]=(time_buf[6]>>4);  //��
  time[14]=(time_buf[6]&0x0f);
}









