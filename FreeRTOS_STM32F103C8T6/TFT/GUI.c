#include "lcd.h"
#include "string.h"
#include "font.h" 
#include "delay.h"
#include "gui.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//����Ӳ������Ƭ��STM32F103RBT6,����ԭ��MiniSTM32������,��Ƶ72MHZ
//QDtech-TFTҺ������ for STM32 IOģ��
//xiao��@ShenZhen QDtech co.,LTD
//��˾��վ:www.qdtft.com
//�Ա���վ��http://qdtech.taobao.com
//��˾�ṩ����֧�֣��κμ������⻶ӭ��ʱ����ѧϰ
//�̻�(����) :+86 0755-23594567 
//�ֻ�:15989313508���빤�� 
//����:goodtft@126.com 
//Skype:QDtech2008
//����֧��QQ:3002773612
//��������QQȺ:324828016
//��������:2013/5/13
//�汾��V2.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������ȫ�����Ӽ������޹�˾ 2013-2019
//All rights reserved

//******************************************************************
//��������  GUI_DrawPoint
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    GUI���һ����
//���������x:���λ��x����
//        	y:���λ��y����
//			color:Ҫ������ɫ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void GUI_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_SetCursor(x,y);//���ù��λ�� 
	LCD_DrawPoint_16Bit(color); 
}

//******************************************************************
//��������  LCD_Fill
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ��ָ�������������ɫ
//���������sx:ָ������ʼ��x����
//        	sy:ָ������ʼ��y����
//			ex:ָ�����������x����
//			ey:ָ�����������y����
//        	color:Ҫ������ɫ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{  	

	u16 i,j;			
	u16 width=ex-sx+1; 		//�õ����Ŀ��
	u16 height=ey-sy+1;		//�߶�
	LCD_SetWindows(sx,sy,ex-1,ey-1);//������ʾ����

	LCD_CS_CLR;	
	LCD_RS_SET;//д����    
	for(i=0;i<width;i++)
	{
		for(j=0;j<height;j++)
		{
#if USE_HARDWARE_SPI   
   SPI_WriteByte(SPI1,color>>8);
	 SPI_WriteByte(SPI1,color);
#else
	 SPIv_WriteData(color>>8);
	 SPIv_WriteData(color);
#endif   
		}
	}
	LCD_CS_SET;	
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ���������Ϊȫ��
}

//******************************************************************
//��������  LCD_DrawLine
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    GUI����
//���������x1,y1:�������
//        	x2,y2:�յ����� 
//����ֵ��  ��
//�޸ļ�¼����
//****************************************************************** 
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		LCD_DrawPoint(uRow,uCol);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 

//******************************************************************
//��������  LCD_DrawRectangle
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    GUI������(�����)
//���������(x1,y1),(x2,y2):���εĶԽ�����
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************  
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}  

//******************************************************************
//��������  LCD_DrawFillRectangle
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    GUI������(���)
//���������(x1,y1),(x2,y2):���εĶԽ�����
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************   
void LCD_DrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_Fill(x1,y1,x2,y2,POINT_COLOR);

}
 
//******************************************************************
//��������  _draw_circle_8
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    8�Գ��Ի�Բ�㷨(�ڲ�����)
//���������(xc,yc) :Բ��������
// 			(x,y):��������Բ�ĵ�����
//         	c:������ɫ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************  
void _draw_circle_8(int xc, int yc, int x, int y, u16 c)
{
	GUI_DrawPoint(xc + x, yc + y, c);

	GUI_DrawPoint(xc - x, yc + y, c);

	GUI_DrawPoint(xc + x, yc - y, c);

	GUI_DrawPoint(xc - x, yc - y, c);

	GUI_DrawPoint(xc + y, yc + x, c);

	GUI_DrawPoint(xc - y, yc + x, c);

	GUI_DrawPoint(xc + y, yc - x, c);

	GUI_DrawPoint(xc - y, yc - x, c);
}

//******************************************************************
//��������  gui_circle
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ��ָ��λ�û�һ��ָ����С��Բ(���)
//���������(xc,yc) :Բ��������
//         	c:������ɫ
//		 	r:Բ�뾶
//		 	fill:����жϱ�־��1-��䣬0-�����
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************  
void gui_circle(int xc, int yc,u16 c,int r, int fill)
{
	int x = 0, y = r, yi, d;

	d = 3 - 2 * r;


	if (fill) 
	{
		// �����䣨��ʵ��Բ��
		while (x <= y) {
			for (yi = x; yi <= y; yi++)
				_draw_circle_8(xc, yc, x, yi, c);

			if (d < 0) {
				d = d + 4 * x + 6;
			} else {
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	} else 
	{
		// �������䣨������Բ��
		while (x <= y) {
			_draw_circle_8(xc, yc, x, y, c);
			if (d < 0) {
				d = d + 4 * x + 6;
			} else {
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	}
}

//******************************************************************
//��������  LCD_ShowChar
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ��ʾ����Ӣ���ַ�
//���������(x,y):�ַ���ʾλ����ʼ����
//        	fc:ǰ�û�����ɫ
//			bc:������ɫ
//			num:��ֵ��0-94��
//			size:�����С
//			mode:ģʽ  0,���ģʽ;1,����ģʽ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************  
void LCD_ShowChar(u16 x,u16 y,u16 fc, u16 bc, u8 num,u8 size,u8 mode)
{  
    u8 temp;
    u8 pos,t;
	u16 colortemp=POINT_COLOR;      
		   
	num=num-' ';//�õ�ƫ�ƺ��ֵ
	LCD_SetWindows(x,y,x+size/2-1,y+size-1);//���õ���������ʾ����
	if(!mode) //�ǵ��ӷ�ʽ
	{
		
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//����1206����
			else temp=asc2_1608[num][pos];		 //����1608����
			for(t=0;t<size/2;t++)
		    {                 
		        if(temp&0x01)LCD_DrawPoint_16Bit(fc); 
				else LCD_DrawPoint_16Bit(bc); 
				temp>>=1; 
				
		    }
			
		}	
	}else//���ӷ�ʽ
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//����1206����
			else temp=asc2_1608[num][pos];		 //����1608����
			for(t=0;t<size/2;t++)
		    {   
				POINT_COLOR=fc;              
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos);//��һ����    
		        temp>>=1; 
		    }
		}
	}
	POINT_COLOR=colortemp;	
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ�����Ϊȫ��    	   	 	  
} 
//******************************************************************
//��������  LCD_ShowString
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ��ʾӢ���ַ���
//���������x,y :�������	 
//			size:�����С
//			*p:�ַ�����ʼ��ַ
//			mode:ģʽ	0,���ģʽ;1,����ģʽ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************  	  
void LCD_ShowString(u16 x,u16 y,u8 size,u8 *p,u8 mode)
{         
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {   
		if(x>(lcddev.width-1)||y>(lcddev.height-1)) 
		return;     
        LCD_ShowChar(x,y,POINT_COLOR,BACK_COLOR,*p,size,mode);
        x+=size/2;
        p++;
    }  
} 

//******************************************************************
//��������  mypow
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ��m��n�η�(gui�ڲ�����)
//���������m:����
//	        n:��
//����ֵ��  m��n�η�
//�޸ļ�¼����
//******************************************************************  
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//******************************************************************
//��������  LCD_ShowNum
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ��ʾ�������ֱ���ֵ
//���������x,y :�������	 
//			len :ָ����ʾ���ֵ�λ��
//			size:�����С(12,16)
//			color:��ɫ
//			num:��ֵ(0~4294967295)
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************  			 
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,POINT_COLOR,BACK_COLOR,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,POINT_COLOR,BACK_COLOR,temp+'0',size,0); 
	}
} 

//******************************************************************
//��������  GUI_DrawFont16
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ��ʾ����16X16��������
//���������x,y :�������
//			fc:ǰ�û�����ɫ
//			bc:������ɫ	 
//			s:�ַ�����ַ
//			mode:ģʽ	0,���ģʽ;1,����ģʽ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void GUI_DrawFont16(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;
	u16 x0=x;
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//�Զ�ͳ�ƺ�����Ŀ
	
			
	for (k=0;k<HZnum;k++) 
	{
	  if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
	  { 	LCD_SetWindows(x,y,x+16-1,y+16-1);
		    for(i=0;i<16*2;i++)
		    {
				for(j=0;j<8;j++)
		    	{	
					if(!mode) //�ǵ��ӷ�ʽ
					{
						if(tfont16[k].Msk[i]&(0x80>>j))	LCD_DrawPoint_16Bit(fc);
						else LCD_DrawPoint_16Bit(bc);
					}
					else
					{
						POINT_COLOR=fc;
						if(tfont16[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//��һ����
						x++;
						if((x-x0)==16)
						{
							x=x0;
							y++;
							break;
						}
					}

				}
				
			}
			
			
		}				  	
		continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
	}

	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ�����Ϊȫ��  
} 

//******************************************************************
//��������  GUI_DrawFont24
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ��ʾ����24X24��������
//���������x,y :�������
//			fc:ǰ�û�����ɫ
//			bc:������ɫ	 
//			s:�ַ�����ַ
//			mode:ģʽ	0,���ģʽ;1,����ģʽ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void GUI_DrawFont24(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;
	u16 x0=x;
	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//�Զ�ͳ�ƺ�����Ŀ
		
			for (k=0;k<HZnum;k++) 
			{
			  if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
			  { 	LCD_SetWindows(x,y,x+24-1,y+24-1);
				    for(i=0;i<24*3;i++)
				    {
							for(j=0;j<8;j++)
							{
								if(!mode) //�ǵ��ӷ�ʽ
								{
									if(tfont24[k].Msk[i]&(0x80>>j))	LCD_DrawPoint_16Bit(fc);
									else LCD_DrawPoint_16Bit(bc);
								}
							else
							{
								POINT_COLOR=fc;
								if(tfont24[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//��һ����
								x++;
								if((x-x0)==24)
								{
									x=x0;
									y++;
									break;
								}
							}
						}
					}
					
					
				}				  	
				continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
			}

	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ�����Ϊȫ��  
}

//******************************************************************
//��������  GUI_DrawFont32
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ��ʾ����32X32��������
//���������x,y :�������
//			fc:ǰ�û�����ɫ
//			bc:������ɫ	 
//			s:�ַ�����ַ
//			mode:ģʽ	0,���ģʽ;1,����ģʽ
//����ֵ��  ��
//�޸ļ�¼����
//****************************************************************** 
void GUI_DrawFont32(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;
	u16 x0=x;
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//�Զ�ͳ�ƺ�����Ŀ
	for (k=0;k<HZnum;k++) 
			{
			  if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
			  { 	LCD_SetWindows(x,y,x+32-1,y+32-1);
				    for(i=0;i<32*4;i++)
				    {
						for(j=0;j<8;j++)
				    	{
							if(!mode) //�ǵ��ӷ�ʽ
							{
								if(tfont32[k].Msk[i]&(0x80>>j))	LCD_DrawPoint_16Bit(fc);
								else LCD_DrawPoint_16Bit(bc);
							}
							else
							{
								POINT_COLOR=fc;
								if(tfont32[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//��һ����
								x++;
								if((x-x0)==32)
								{
									x=x0;
									y++;
									break;
								}
							}
						}
					}
					
					
				}				  	
				continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
			}
	
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ�����Ϊȫ��  
} 

//******************************************************************
//��������  Show_Str
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ��ʾһ���ַ���,������Ӣ����ʾ
//���������x,y :�������
// 			fc:ǰ�û�����ɫ
//			bc:������ɫ
//			str :�ַ���	 
//			size:�����С
//			mode:ģʽ	0,���ģʽ;1,����ģʽ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************    	   		   
void Show_Str(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode)
{					
	u16 x0=x;							  	  
  	u8 bHz=0;     //�ַ��������� 
    while(*str!=0)//����δ����
    { 
        if(!bHz)
        {
			if(x>(lcddev.width-size/2)||y>(lcddev.height-size)) 
			return; 
	        if(*str>0x80)bHz=1;//���� 
	        else              //�ַ�
	        {          
		        if(*str==0x0D)//���з���
		        {         
		            y+=size;
					x=x0;
		            str++; 
		        }  
		        else
				{
					if(size>16)//�ֿ���û�м���12X24 16X32��Ӣ������,��8X16����
					{  
					LCD_ShowChar(x,y,fc,bc,*str,16,mode);
					x+=8; //�ַ�,Ϊȫ�ֵ�һ�� 
					}
					else
					{
					LCD_ShowChar(x,y,fc,bc,*str,size,mode);
					x+=size/2; //�ַ�,Ϊȫ�ֵ�һ�� 
					}
				} 
				str++; 
		        
	        }
        }
				
				else//���� 
        {   
			if(x>(lcddev.width-size)||y>(lcddev.height-size)) 
			return;  
            bHz=0;//�к��ֿ�    
			if(size==32)
			GUI_DrawFont32(x,y,fc,bc,str,mode);	 	
			else if(size==24)
			GUI_DrawFont24(x,y,fc,bc,str,mode);	
			else
			GUI_DrawFont16(x,y,fc,bc,str,mode);
				
	        str+=2; 
	        x+=size;//��һ������ƫ��	    
        }						 
    }   
}

//******************************************************************
//��������  Gui_StrCenter
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ������ʾһ���ַ���,������Ӣ����ʾ
//���������x,y :�������
// 			fc:ǰ�û�����ɫ
//			bc:������ɫ
//			str :�ַ���	 
//			size:�����С
//			mode:ģʽ	0,���ģʽ;1,����ģʽ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************   
void Gui_StrCenter(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode)
{
	u16 len=strlen((const char *)str);
	u16 x1=(lcddev.width-len*8)/2;
	Show_Str(x+x1,y,fc,bc,str,size,mode);
} 
 
//******************************************************************
//��������  Gui_Drawbmp16
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ��ʾһ��16λBMPͼ��
//���������x,y :�������
// 			*p :ͼ��������ʼ��ַ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************  
void Gui_Drawbmp16(u16 x,u16 y,const unsigned char *p) 
{
  	int i; 
	unsigned char picH,picL; 
	LCD_SetWindows(x,y,x+bmp_size_X-1,y+bmp_size_Y-1);//��������
    for(i=0;i<bmp_size_X*bmp_size_Y;i++)
	{	
	 	picL=*(p+i*2);	//���ݵ�λ��ǰ
		picH=*(p+i*2+1);				
		LCD_DrawPoint_16Bit(picH<<8|picL);  						
	}	
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ���ʾ����Ϊȫ��	

}
