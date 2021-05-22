/*=========================================================================================================
  * File Name	 : HLW8110.c
  * Describe 	 : HLW8110 UART ͨѶ����,ʹ��USATR2.(HLW8110 code,use USATE2)
  * Author	   : Tuqiang
  * Version	   : V1.3
  * Record	   : 2019/04/16��V1.2
  * Record	   : 2020/04/02, V1.3	
==========================================================================================================*/
#include "HLW8110.h"
#include "delay.h"
#include "stdio.h"


#if 1


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
union IntData
{
	u16  inte;			
	u8 byte[2];		
};
union LongData
{
    u32  word;		
    u16  inte[2];		
    u8   byte[4];		
};
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

unsigned char	u8_TxBuf[10]; 
unsigned char	u8_RxBuf[10];
unsigned char	u8_TX_Length;
unsigned char	u8_RX_Length;
unsigned char	u8_RX_Index;
//unsigned char	B_ReadReg_Time_EN;			// ���ڶ�ȡ�Ĵ������ݣ�ʱ���������־λ��1--����������0--�رռ���
//unsigned char	B_Tx_Finish;
unsigned char	B_Rx_Finish;
unsigned char	B_Rx_Data_ING;					// �������ݱ�־λ	,		< 1:����������,0:δ���յ����� >
unsigned char	B_Read_Error;							// UART��ȡ����У��ͳ���,< 1:���ݶ���0:���ݶ�ȡ��ȷ >
//unsigned char	u8_ReadReg_Index;
//unsigned char	u8_ReadReg_Time;				// ���ڶ�ȡ�Ĵ������ݵ�ʱ��
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

unsigned int    U16_TempData;	


unsigned int    U16_IFData;
unsigned int    U16_RIFData;
unsigned int    U16_LineFData;
unsigned int    U16_AngleData;
unsigned int    U16_PFData;
unsigned int    U16_HFConst_RegData;
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
unsigned int	U16_RMSIAC_RegData; 			// Aͨ������ת��ϵ��
unsigned int	U16_RMSIBC_RegData; 			// Bͨ������ת��ϵ��
unsigned int	U16_RMSUC_RegData; 				// ��ѹͨ��ת��ϵ��
unsigned int	U16_PowerPAC_RegData; 		// Aͨ������ת��ϵ��
//unsigned int	U16_FreqAC_RegData; 		// Aͨ��Ƶ��ת��ϵ��
unsigned int	U16_PowerPBC_RegData; 		// Bͨ������ת��ϵ��
unsigned int	U16_PowerSC_RegData; 			// ���ڹ���ת��ϵ��,���ѡ��Aͨ��������Aͨ�����ڹ���ת��ϵ����A��Bͨ��ֻ�ܶ���ѡ��һ
unsigned int	U16_EnergyAC_RegData; 		// Aͨ���й�����(��)ת��ϵ�� 
unsigned int	U16_EnergyBC_RegData; 		// Aͨ���й�����(��)ת��ϵ��

unsigned int	U16_CheckSUM_RegData; 		// ת��ϵ����CheckSum
unsigned int	U16_CheckSUM_Data; 				// ת��ϵ�����������CheckSum

unsigned int	U16_Check_SysconReg_Data; 						
unsigned int	U16_Check_EmuconReg_Data; 						
unsigned int	U16_Check_Emucon2Reg_Data; 				

/*---------------------------------------------------------------------------------------------------------*/			
/*---------------------------------------------------------------------------------------------------------*/
unsigned long 	U32_RMSIA_RegData;			// Aͨ��������Чֵ�Ĵ���
unsigned long 	U32_RMSU_RegData;				// ��ѹ��Чֵ�Ĵ���
unsigned long 	U32_POWERPA_RegData;		// Aͨ��������Чֵ�Ĵ���
unsigned long 	U32_POWERSA_RegData;		// Aͨ���������ڹ��ʼĴ���
unsigned long 	U32_POWERPF_RegData;		// Aͨ�����ʹ��������Ĵ���
unsigned long 	U32_ENERGY_PA_RegData;	// Aͨ���й����ܣ�������Чֵ�Ĵ���
unsigned long 	U32_Freq_RegData;		// Aͨ��������Чֵ�Ĵ���


unsigned long 	U32_RMSIB_RegData;			// Bͨ��������Чֵ�Ĵ���
unsigned long		U32_POWERPB_RegData;		// Bͨ��������Чֵ�Ĵ���
unsigned long 	U32_ENERGY_PB_RegData;	// Bͨ���й����ܣ�������Чֵ�Ĵ���
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
float   F_DC_V;													// ��ѹ��Чֵ
float   F_DC_I;													// Aͨ������
float   F_DC_P;													// Aͨ���й�����
float   F_DC_E;													// Aͨ���й�����(��)
float   F_DC_BACKUP_E;									          // Aͨ����������	
float   F_DC_PF;												// �������أ�Aͨ����Bͨ��ֻ��ѡ��һ 
float   F_DC_SA;	 //���ڹ���
float   F_DC_WF;	 //�޹�����
float   F_Angle;												// ��ǣ�Aͨ����Bͨ��ֻ��ѡ��һ 

float   F_AC_LINE_Freq;     						// �е�����Ƶ��
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/


/*=========================================================================================================
 * Function : void USART2_IRQHandler(void)
 * Describe : UART2�����жϣ����ڽ�������,9600bps,żУ��,even
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2019/04/16
==========================================================================================================*/
void USART2_IRQ_User(void)
{
	//��������
	if(USART2->SR&(1<<5))	
	{	 
		USART2->SR;
		if (u8_RX_Index < u8_RX_Length )
		{
			u8_RxBuf[u8_RX_Index] = USART2->DR;		// ���ݽ�����
			u8_RX_Index++;
			
			B_Rx_Data_ING = 1;										// �����ݽ��ձ�־λ
		}
		else
		{
			B_Rx_Finish = TRUE;										// ���ݽ������
			u8_RX_Index = 0;

		}
	}
	
	//��������
	
}
/*=========================================================================================================
 * Function : void Start_Send_UartData(unsigned char len)
 * Describe : UART2���ڷ�������
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2019/04/16
=========================================================================================================*/

void Start_Send_UartData(unsigned char len)
{
	unsigned char i;
	for(i=0;i<len;i++)
	{
		
		while((USART2->SR&0X40) == 0);	//�ȴ����ͽ���
		
		//delay_us(50);	//�����ֽ�֮��������ʱ
		USART2->DR	=	u8_TxBuf[i];
		
	}

}
/*=========================================================================================================
 * Function : void Clear_RxBuf(void)
 * Describe : ��׼�����մ�������ǰ����ս��ջ�����������
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2019/04/03
=========================================================================================================*/
void Clear_RxBuf(void)
{
	unsigned char i;
	for(i = 0;i<10;i++)
	{
		u8_RxBuf[i] = 0x00;
	}
	
	B_Rx_Data_ING = 0;
	B_Rx_Finish = FALSE;
	u8_RX_Index = 0;
}
/*=========================================================================================================
 * Function : unsigned char HLW8110_checkSum_Write(unsigned char u8_Reg_length)
 * Describe : 
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2019/04/03
=========================================================================================================*/
unsigned char HLW8110_checkSum_Write(unsigned char u8_Reg_length)
{
	unsigned char i;
	unsigned char Temp_u8_checksum;
	unsigned int	a;

	a = 0x0000;
	Temp_u8_checksum = 0;
	for (i = 0; i< (u8_Reg_length-1); i++)
		{
			a += u8_TxBuf[i];
		}
	
	a = ~a;
	Temp_u8_checksum = a & 0xff;

	return Temp_u8_checksum;
	
}

/*=========================================================================================================
 * Function : unsigned char HLW8110_checkSum_Read(void)
 * Describe : 
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2019/04/03
==========================================================================================================*/
unsigned char HLW8110_checkSum_Read(unsigned char u8_Reg_length)
{
	unsigned char i;
	unsigned char Temp_u8_checksum;
	unsigned int a;

	a = 0x0000;
	Temp_u8_checksum = 0;
	for (i = 0; i< (u8_Reg_length-1); i++)
		{
			a += u8_RxBuf[i];
		}
		
	a = a + u8_TxBuf[0] + u8_TxBuf[1];
	a = ~a;
		
	Temp_u8_checksum = a & 0xff;

	return Temp_u8_checksum;
	
}
/*=========================================================================================================
 * Function : void Uart_HLW8110_WriteREG_EN(void)
 * Describe : 
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2019/04/03
==========================================================================================================*/
void Uart_HLW8110_WriteREG_EN(void)
{

	u8_TX_Length = 4;
	u8_RX_Length = 0;
	
	u8_TxBuf[0] = 0xa5;
	u8_TxBuf[1] = 0xea;
	u8_TxBuf[2] = 0xe5;
//	u8_TxBuf[3] = 0x8b;  //checksum
	u8_TxBuf[3] = HLW8110_checkSum_Write(u8_TX_Length);

	Start_Send_UartData(u8_TX_Length);
}

/*=========================================================================================================
 * Function : void Uart_HLW8110_WriteREG_DIS(void)
 * Describe : 
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2019/04/03
==========================================================================================================*/

void Uart_HLW8110_WriteREG_DIS(void)
{
	
	u8_TX_Length = 4;
	u8_RX_Length = 0;
	
	u8_TxBuf[0] = 0xa5;
	u8_TxBuf[1] = 0xea;
	u8_TxBuf[2] = 0xdc;
//	u8_TxBuf[3] = 0x94;  //checksum
	u8_TxBuf[3] = HLW8110_checkSum_Write(u8_TX_Length);
        
	Start_Send_UartData(u8_TX_Length);
}


/*=========================================================================================================
 * Function : void Uart_HLW8110_Set_Channel_A(void)
 * Describe : 
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2019/04/03
==========================================================================================================*/
void Uart_HLW8110_Set_Channel_A(void)
{
	u8_TX_Length = 4;
	u8_RX_Length = 0;
	
	u8_TxBuf[0] = 0xa5;
	u8_TxBuf[1] = 0xea;
	u8_TxBuf[2] = 0x5a;
//	u8_TxBuf[3] = 0x16;  //checksum
	u8_TxBuf[3] = HLW8110_checkSum_Write(u8_TX_Length);

	Start_Send_UartData(u8_TX_Length);
}


/*=========================================================================================================
 * Function : void Uart_Read_HLW8110_Reg(unsigned char ADDR_Reg,unsigned char u8_reg_length)
 * Describe :
 * Input    : 
 * Output   : 
 * Return   : 
 * Record   : 2019/04/04
==========================================================================================================*/
void Uart_Read_HLW8110_Reg(unsigned char ADDR_Reg,unsigned char u8_reg_length)
{
	u8_TxBuf[0] = 0xa5;
	u8_TxBuf[1] = ADDR_Reg;
	u8_TX_Length =  2;
	u8_RX_Length = u8_reg_length + 1;	        // +1������Ϊ���յ����ݳ��ȣ�����REGֵ������һ��У�������
	
	
	Clear_RxBuf();										//��ս��ջ�����
	Start_Send_UartData(u8_TX_Length);
}
/*=========================================================================================================
 * Function : void Uart_Write_HLW8110_Reg(unsigned char ADDR_Reg,unsigned char u8_reg_length,unsigned long u32_data)
 * Describe : д�Ĵ������u8_reg_length��д��ļĴ��������ֽڳ���
 * Input    : 
 * Output   : 
 * Return   : 
 * Record   : 2019/04/03
==========================================================================================================*/
void Uart_Write_HLW8110_Reg(unsigned char ADDR_Reg,unsigned char u8_reg_length,unsigned long u32_data)
{
	unsigned char i;
	union LongData Temp_u32_a;

	
	u8_TxBuf[0] = 0xa5;
	u8_TxBuf[1] = ADDR_Reg|0x80;

	Temp_u32_a.word = u32_data;
	for (i = 0; i< u8_reg_length; i++)
		{
			u8_TxBuf[i+2] = Temp_u32_a.byte[u8_reg_length-1-i];						//STM32��32λMCU��union���壬�ǵ�λ��ǰ
			//u8_TxBuf[i+2] = Temp_u32_a.byte[4-u8_reg_length + i];				//STM8,STC MCU,   union���壬�Ǹ�λ��ǰ
		}


	u8_TX_Length = 3 + u8_reg_length ;
	u8_RX_Length = 0;
	
	u8_TxBuf[u8_TX_Length-1] = HLW8110_checkSum_Write(u8_TX_Length);


	Start_Send_UartData(u8_TX_Length);
}

/*=========================================================================================================
 * Function : void Uart_HLW8110_Reset(void)
 * Describe : 
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2019/04/03
==========================================================================================================*/
void Uart_HLW8110_Reset(void)
{
	u8_TX_Length = 4;
	u8_RX_Length = 0;
	
	u8_TxBuf[0] = 0xa5;
	u8_TxBuf[1] = 0xea;
	u8_TxBuf[2] = 0x96;
//	u8_TxBuf[3] = 0xda;  //checksum
	
	u8_TxBuf[3] = HLW8110_checkSum_Write(u8_TX_Length);

	Start_Send_UartData(u8_TX_Length);
}




/*=========================================================================================================
 * Function : void Judge_CheckSum_HLW8110_Calfactor(void)
 * Describe : ��֤��ַ0x70-0x77��ַ��ϵ����
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2019/03/18
==========================================================================================================*/
unsigned char Judge_CheckSum_HLW8110_Calfactor(void)
{
	unsigned long a;
	//unsigned int b;
	//unsigned int c;
	unsigned char d;
 
  //��ȡRmsIAC��RmsIBC��RmsUC��PowerPAC��PowerPBC��PowerSC��EnergAc��EnergBc��ֵ
	
	Uart_Read_HLW8110_Reg(REG_RMS_IAC_ADDR,2);
	delay_ms(50);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_RMSIAC_RegData = (u8_RxBuf[0]<<8) + u8_RxBuf[1] ;
		printf("Aͨ������ת��ϵ��:%x\n " ,U16_RMSIAC_RegData);
	}
	
	
	Uart_Read_HLW8110_Reg(REG_RMS_IBC_ADDR,2);
	delay_ms(50);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_RMSIBC_RegData = (u8_RxBuf[0]<<8) + u8_RxBuf[1] ;
		printf("Bͨ������ת��ϵ��:%x\n " ,U16_RMSIBC_RegData);
	}
	
	
	Uart_Read_HLW8110_Reg(REG_RMS_UC_ADDR,2);
	delay_ms(50);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_RMSUC_RegData = (u8_RxBuf[0]<<8) + u8_RxBuf[1] ;
		printf("��ѹͨ��ת��ϵ��:%x\n " ,U16_RMSUC_RegData);
	}
		
	Uart_Read_HLW8110_Reg(REG_POWER_PAC_ADDR,2);
	delay_ms(50);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_PowerPAC_RegData = (u8_RxBuf[0]<<8) + u8_RxBuf[1] ;
		printf("Aͨ������ת��ϵ��:%x\n " ,U16_PowerPAC_RegData);
	}
		
	Uart_Read_HLW8110_Reg(REG_POWER_PBC_ADDR,2);
	delay_ms(50);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_PowerPBC_RegData = (u8_RxBuf[0]<<8) + u8_RxBuf[1] ;
		printf("Bͨ������ת��ϵ��:%x\n " ,U16_PowerPAC_RegData);
	}
	
	Uart_Read_HLW8110_Reg(REG_POWER_SC_ADDR,2);
	delay_ms(50);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_PowerSC_RegData = (u8_RxBuf[0]<<8) + u8_RxBuf[1] ;
		printf("���ڹ���ת��ϵ��:%x\n " ,U16_PowerSC_RegData);
	}
	
	Uart_Read_HLW8110_Reg(REG_ENERGY_AC_ADDR,2);
	delay_ms(50);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_EnergyAC_RegData = (u8_RxBuf[0]<<8) + u8_RxBuf[1] ;
		printf("Aͨ������ת��ϵ��:%x\n " ,U16_EnergyAC_RegData);
	
	}
	Uart_Read_HLW8110_Reg(REG_ENERGY_BC_ADDR,2);
	delay_ms(50);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_EnergyBC_RegData = (u8_RxBuf[0]<<8) + u8_RxBuf[1] ;
		printf("Bͨ������ת��ϵ��:%x\n " ,U16_EnergyBC_RegData);
	}
	
 
	Uart_Read_HLW8110_Reg(REG_CHECKSUM_ADDR,2);
	delay_ms(50);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_CheckSUM_RegData= (u8_RxBuf[0]<<8) + u8_RxBuf[1] ;
		printf("ϵ��У���:%x\n " ,U16_CheckSUM_RegData);
	}
	
	
	a = 0;
    a = ~(0xffff+U16_RMSIAC_RegData + U16_RMSIBC_RegData + U16_RMSUC_RegData + 
        U16_PowerPAC_RegData + U16_PowerPBC_RegData + U16_PowerSC_RegData + 
          U16_EnergyAC_RegData + U16_EnergyBC_RegData  );
  
	U16_CheckSUM_Data = a & 0xffff;
	
	printf("����ϵ��У���:%x\n " ,U16_CheckSUM_Data);
  
	if ( U16_CheckSUM_Data == U16_CheckSUM_RegData)
	{
		d = 1;
		printf("У�����ȷ\r\n ");
    
	}
	else
	{
		d = 0;
		printf("У��ͳ���\r\n ");
	}
  
	return d;
  
}

/*=========================================================================================================
 * Function : void Init_HLW8110(void)
 * Describe : 
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2019/04/03
==========================================================================================================*/
void Init_HLW8110(void)
{
	   	 	
	Uart_HLW8110_WriteREG_EN();
	delay_ms(10);
	
//����ͨ��A�������ָ����ǰ���ڼ������ڹ��ʡ�������������ǡ�˲ʱ�й����ʡ�˲ʱ���ڹ��ʺ��й����ʹ��ص��ź�ָʾ ��ͨ��Ϊͨ��A	
	Uart_HLW8110_Set_Channel_A();			
	delay_ms(10);
	Uart_Write_HLW8110_Reg(REG_SYSCON_ADDR,2,0x0a00);	//����Aͨ�����ر�Bͨ������ѹͨ��PGA = 1������ͨ��PGA = 1
	delay_ms(10);
    
   Uart_Write_HLW8110_Reg(RmsIAOS,2,0xf8fd);	//����Aͨ�����ر�Bͨ������ѹͨ��PGA = 1������ͨ��PGA = 1
	delay_ms(10);
	
//�������ֱ��  0x0001Ϊ�⽻����0x0071Ϊ��ֱ��     
    if(DC_Flage == 1)
	       Uart_Write_HLW8110_Reg(REG_EMUCON_ADDR,2,0x0071);	//1��ʹ��PFA ����������й����ܼĴ����ۼӣ�
    else
        Uart_Write_HLW8110_Reg(REG_EMUCON_ADDR,2, 0x0001);      
//	Uart_Write_HLW8110_Reg(REG_EMUCON_ADDR,2,0x0018);	//����͸�������������仯��ZXD0 = 1��ZXD1 = 1
    
// ��ѹ�����������ؼ��ʹ���ź� �����⡢��ǡ���ѹƵ�ʲ���ʹ���ź� ��ֵ���ʹ���ź�  ���ݸ���Ƶ��
	Uart_Write_HLW8110_Reg(REG_EMUCON2_ADDR,2,0x0465);	//0x0001��EMUCON2��Ĭ��ֵ��waveEn = 1,zxEn = 1��Aͨ�������Ĵ�����������0��EPA_CB = 1���򿪹������ؼ��
	delay_ms(10);
    
	Uart_HLW8110_WriteREG_DIS();
//	delay_ms(10);	
  //��ȡ��ַ��0x6F��0x77�ļĴ�������֤ϵ���Ƿ���ȷ
  Judge_CheckSum_HLW8110_Calfactor();	
}


/*=========================================================================================================
 * Function : void Check_WriteReg_Success(void)
 * Describe : ����д���REG�Ƿ���ȷд��
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2020/04/02
==========================================================================================================*/
void Check_WriteReg_Success(void)
{
	Uart_Read_HLW8110_Reg(REG_SYSCON_ADDR,2);
	delay_ms(10);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_Check_SysconReg_Data =  (unsigned int)(u8_RxBuf[0]<<8) + (unsigned int)(u8_RxBuf[1]); 
//		printf("д���SysconReg�Ĵ���:%lx\n " ,U16_Check_SysconReg_Data);
	}
	else
	{
		printf("SysconReg�Ĵ�����ȡ����\r\n");
		B_Read_Error = 1;
	}
	
	Uart_Read_HLW8110_Reg(REG_EMUCON_ADDR,2);
	delay_ms(10);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_Check_EmuconReg_Data =  (unsigned int)(u8_RxBuf[0]<<8) + (unsigned int)(u8_RxBuf[1]); 
//		printf("д���EmuconReg�Ĵ���:%lx\n " ,U16_Check_EmuconReg_Data);
	}
	else
	{
		printf("EmuconReg�Ĵ�����ȡ����\r\n");
		B_Read_Error = 1;
	}
	
	
	Uart_Read_HLW8110_Reg(REG_EMUCON2_ADDR,2);
		delay_ms(10);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_Check_Emucon2Reg_Data =  (u8_RxBuf[0]<<8) + (u8_RxBuf[1]); 
//		printf("д���Emucon2Reg�Ĵ����Ĵ���:%lx\n " ,U16_Check_Emucon2Reg_Data);
	}
	else
	{
		printf("Emucon2Reg�Ĵ�����ȡ����\r\n");
		B_Read_Error = 1;
	}
	
}

/*=========================================================================================================
 * Function : void Read_HLW8112_IA(void)
 * Describe : ��ȡAͨ������
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2019/04/03
==========================================================================================================*/
void Read_HLW8110_IA(void)
{	
	float a;
	
	Uart_Read_HLW8110_Reg(REG_RMSIA_ADDR,3);
		delay_ms(10);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U32_RMSIA_RegData = (unsigned long)(u8_RxBuf[0]<<16) + (unsigned long)(u8_RxBuf[1]<<8) + (unsigned long)(u8_RxBuf[2]); 
//		printf("Aͨ�������Ĵ���:%lx\n " ,U32_RMSIA_RegData);
	}
	else
	{
		printf("Aͨ�������Ĵ�����ȡ����\r\n");
		B_Read_Error = 1;
	}
	
	
	//Aͨ������PGA = 16,��ѹͨ��PGA = 1;������������1mR,��ѹ��������1M
    //���㹫ʽ,U16_AC_I = (U32_RMSIA_RegData * U16_RMSIAC_RegData)/(����ϵ��* 2^23��
	if ((U32_RMSIA_RegData & 0x800000) == 0x800000)
	 {
			F_DC_I = 0;
	 }
	 else
	 {
		a = (float)U32_RMSIA_RegData;
		a = a * U16_RMSIAC_RegData;
		a  = a/0x800000;                     //������������ĸ�������λ��mA,����5003.12 
   if(DC_Flage==1)
		a = a/490.0001;  								// 1 = ����ϵ��
   
   else  
        a = a/0.153285;

		a = a/1000;              //a= 5003ma,a/1000 = 5.003A,��λת����A
		a = a * 0.95;				//D_CAL_A_I��У��ϵ����Ĭ����1
		F_DC_I = a;
	 }
}
/*=========================================================================================================
 * Function : void Read_HLW8110_U(void)
 * Describe : ��ȡ��ѹ
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2019/04/03
==========================================================================================================*/
void Read_HLW8110_U(void)
{
	float a;
	
	Uart_Read_HLW8110_Reg(REG_RMSU_ADDR,3);
		delay_ms(10);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U32_RMSU_RegData = (unsigned long)(u8_RxBuf[0]<<16) + (unsigned long)(u8_RxBuf[1]<<8) + (unsigned long)(u8_RxBuf[2]);
//		printf("��ѹͨ���Ĵ���:%lx\n " ,U32_RMSU_RegData);
	}
	else
	{
		printf("��ѹͨ���Ĵ�����ȡ����\r\n");
		B_Read_Error = 1;
	}
	
		//��ѹ
	//����:U16_AC_V = (U32_RMSU_RegData * U16_RMSUC_RegData)/2^23
	
	 if ((U32_RMSU_RegData &0x800000) == 0x800000)
	 {
			F_DC_V = 0;
	 }
  else
	{
  a =  (float)U32_RMSU_RegData;
  a = a*U16_RMSUC_RegData;  
  a = a/0x400000;        
if(DC_Flage == 1)  
{
  a = a/20.10;  						// 1 = ��ѹϵ��
  a = a/100;    				    //�����a = 22083.12mV,a/100��ʾ220.8312V����ѹת����V
    
if(a>=20)
  a = a*0.995;			      	//D_CAL_U��У��ϵ����Ĭ����1,

else if(a<10&&a>5)
     a = a*1.004;

//else if(a<5&&a>=3)
//  a = a*0.97;

//else if(a<3)
//  a = a*0.93;      
}
else 
{
    
  a = a/1.063; 
}

  F_DC_V = a;
	}
}
/*=========================================================================================================
 * Function : void Read_HLW8110_PA(void)
 * Describe : ��ȡAͨ������
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2019/04/03
==========================================================================================================*/
float text_data = 0.94;
void Read_HLW8110_PA(void)
{
	float a;
	float b;
	
	Uart_Read_HLW8110_Reg(REG_POWER_PA_ADDR,4);
		delay_ms(10);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U32_POWERPA_RegData = (unsigned long)(u8_RxBuf[0]<<24) + (unsigned long)(u8_RxBuf[1]<<16) + (unsigned long)(u8_RxBuf[2]<<8) + (unsigned long)(u8_RxBuf[3]);
//		printf("Aͨ�����ʼĴ���:%lx\n " ,U32_POWERPA_RegData);
	}
	else
	{
		printf("Aͨ�����ʼĴ�����ȡ����\r\n");
		B_Read_Error = 1;
	}
	
	
	 if (U32_POWERPA_RegData > 0x80000000)      //Ϊ����
   {
     b = ~U32_POWERPA_RegData;
     a = (float)b;
   }
   else
     a =  (float)U32_POWERPA_RegData;
     
//������Ҫ�������͸���
//����,U16_AC_P = (U32_POWERPA_RegData * U16_PowerPAC_RegData)/(2^31*��ѹϵ��*����ϵ��)
//��λΪW,���������5000.123����ʾ5000.123W	
    a = a*U16_PowerPAC_RegData;
    a = a/0x80000000;
if(DC_Flage == 1)
{    
    a = a/490.0001;  										// 1 = ����ϵ��
    a = a/20.10;  										// 1 = ��ѹϵ��
}
else
{
    a = a/0.762;  										// 1 = ����ϵ��
    a = a/1.043;  										// 1 = ��ѹϵ��
}

    a = a * text_data;						//D_CAL_A_P��У��ϵ����Ĭ����1
    F_DC_P = a;									//��λΪW,���������5000.123����ʾ5000.123W

}

/*=========================================================================================================
 * Function : void Read_HLW8110_PA(void)
 * Describe : ��ȡAͨ����������
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2019/04/03
==========================================================================================================*/
void Read_HLW8110_PF(void)
{
	float a;
	//���㹫ʽ��PF = |PF|/0x7FFFFF;
	Uart_Read_HLW8110_Reg(REG_POWER_PS_ADDR,3);
		delay_ms(10);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U32_POWERPF_RegData = (unsigned long)(u8_RxBuf[0]<<16) + (unsigned long)(u8_RxBuf[1]<<8) + (unsigned long)(u8_RxBuf[2]);
	}
	else
	{
		B_Read_Error = 1;
	}
    if(U32_POWERPF_RegData>0x7ffd00)
        a = 1;
    else    
        a = U32_POWERPF_RegData/0x7FFFFF;
    F_DC_PF = a;
}



/*=========================================================================================================
 * Function : void Read_HLW8110_PA(void)
 * Describe : ��ȡAͨ�����ڹ���
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2019/04/03
==========================================================================================================*/
void Read_HLW8110_SA(void)
{
	float a;
	
	Uart_Read_HLW8110_Reg(REG_POWER_SA_ADDR,4);
		delay_ms(10);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U32_POWERSA_RegData = (unsigned long)(u8_RxBuf[0]<<24) + (unsigned long)(u8_RxBuf[1]<<16) + (unsigned long)(u8_RxBuf[2]<<8) + (unsigned long)(u8_RxBuf[3]);
//		printf("Aͨ�����ʼĴ���:%lx\n " ,U32_POWERPA_RegData);
	}
	else
	{
		printf("Aͨ�����ʼĴ�����ȡ����\r\n");
		B_Read_Error = 1;
	}
    
    a = U32_POWERSA_RegData;
	F_DC_SA = F_DC_P/F_DC_PF;
    F_DC_WF= sqrt(F_DC_SA*F_DC_SA-F_DC_P*F_DC_P);

}
/*=========================================================================================================
 * Function : void void Read_HLW8112_EA(void)
 * Describe : ��ȡAͨ���й�����
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2019/04/03
==========================================================================================================*/
void Read_HLW8110_EA(void)
{
	float a;
	Uart_Read_HLW8110_Reg(REG_ENERGY_PA_ADDR,3); 
	delay_ms(10);
	
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U32_ENERGY_PA_RegData = (unsigned long)(u8_RxBuf[0]<<16) + (unsigned long)(u8_RxBuf[1]<<8) + (unsigned long)(u8_RxBuf[2]);
//		printf("Aͨ���й������Ĵ���:%lx\n " ,U32_ENERGY_PA_RegData);
	}
	else
	{
		printf("Aͨ���й������Ĵ�����ȡ����\r\n");
		B_Read_Error = 1;
	}
	
	Uart_Read_HLW8110_Reg(REG_HFCONST_ADDR,2); 
	delay_ms(10);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_HFConst_RegData = (unsigned int)(u8_RxBuf[0]<<8) + (unsigned int)(u8_RxBuf[1]);
//		printf("HFCONST���� = :%d\n " ,U16_HFConst_RegData);
	}
	else
	{
		printf("HFCONST�����Ĵ�����ȡ����\r\n");
		B_Read_Error = 1;
	}

	//��������,���� = (U32_ENERGY_PA_RegData * U16_EnergyAC_RegData * HFCONST) /(K1*K2 * 2^29 * 4096)
	//HFCONST:Ĭ��ֵ��0x1000, HFCONST/(2^29 * 4096) = 0x20000000
	a =  (float)U32_ENERGY_PA_RegData;	
    a = a*U16_EnergyAC_RegData;
    a = a/0x20000000;             //������λ��0.001KWH,�����������2.002,��ʾ2.002KWH    
if(DC_Flage == 1)
{    
    a = a/490.0001;  										// 1 = ����ϵ��
    a = a/20.10;  										// 1 = ��ѹϵ��
}
else
{
    a = a/0.762;  										// 1 = ����ϵ��
    a = a/1.043;  										// 1 = ��ѹϵ��
}
    a = a * D_CAL_A_E;     			//D_CAL_A_E��У��ϵ����Ĭ����1
    F_DC_E = a;
	F_DC_BACKUP_E = F_DC_E;
		
}


/*=========================================================================================================
 * Function : void Read_HLW8110_PA(void)
 * Describe : ��ȡAͨ��Ƶ��
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2019/04/03
==========================================================================================================*/
void Read_HLW8110_Freq(void)
{
	float a;
	
	Uart_Read_HLW8110_Reg(REG_Ufreq_ADDR,2);
		delay_ms(10);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U32_Freq_RegData = (unsigned int)(u8_RxBuf[0]<<8) + (unsigned int)(u8_RxBuf[1]);
	}
	else
	{
		printf("Aͨ��Ƶ�ʼĴ�����ȡ����\r\n");
		B_Read_Error = 1;
	}
	
    a =(float)1.0/U32_Freq_RegData;	
if(DC_Flage == 1)
{    
    a = a/100000;  										// 1 = ����ϵ��
    a = a/100000;  										// 1 = ��ѹϵ��
}

else
{
    a = a*3579545;  										// 1 = ����ϵ��
    a = a/8;  										// 1 = ��ѹϵ��
}

    F_AC_LINE_Freq = a;									//��λΪW,���������5000.123����ʾ5000.123W

}

/*=========================================================================================================
 * Function : void Calculate_HLW8110_MeterData(void);
 * Describe : 
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2018/05/10
==========================================================================================================*/
void Calculate_HLW8110_MeterData(void)
{	    
   
	Check_WriteReg_Success();
	
	Read_HLW8110_IA();                //������Чֵ
	Read_HLW8110_U();                //��ѹ��Чֵ
	Read_HLW8110_PA();               //�й�����
	Read_HLW8110_EA();	             //�й�����
    Read_HLW8110_Freq();             //��ѹƵ��
    Read_HLW8110_SA();               //���ڹ���
    Read_HLW8110_PF();              //��������
//	printf("\r\n");	
//	printf("\r\n");	
//   if(DC_Flage == 1)
//   {
//	   printf("ֱ������,uartͨѶ��ʽ\r\n");
//   }
//   else 
//       printf("��������,uartͨѶ��ʽ\r\n");
//   
//	printf("Aͨ������ת��ϵ��:%x\n " ,U16_RMSIAC_RegData);
//	printf("��ѹͨ��ת��ϵ��:%x\n " ,U16_RMSUC_RegData);
//	printf("Aͨ������ת��ϵ��:%x\n " ,U16_PowerPAC_RegData);
//	printf("���ڹ���ת��ϵ��:%x\n " ,U16_PowerSC_RegData);
//	printf("Aͨ������ת��ϵ��:%x\n " ,U16_EnergyAC_RegData);
//	printf("ת��ϵ��У���:%x\n " ,U16_CheckSUM_RegData);
//	printf("ת��ϵ���������У���:%x\n " ,U16_CheckSUM_Data);
//	
//	printf("\r\n");//���뻻��
//	printf("Aͨ�������Ĵ���:%x\n " ,U32_RMSIA_RegData);
//	printf("��ѹ�Ĵ���:%x\n " ,U32_RMSU_RegData);
//	printf("Aͨ�����ʼĴ���:%x\n " ,U32_POWERPA_RegData);
//	printf("Aͨ�������Ĵ���:%x\n " ,U32_ENERGY_PA_RegData);
//    printf("Aͨ��Ƶ�ʼĴ���:%x\n " ,   U32_Freq_RegData);
//     printf("Aͨ�����ڹ��ʼĴ���:%x\n " ,U32_POWERSA_RegData);
//     printf("Aͨ�����������Ĵ���:%x\n " ,U32_POWERPF_RegData);
//     
//	printf("\r\n");                                               //���뻻��
//	printf("F_DC_I = %f A \n " ,F_DC_I);					 	//����
//	printf("F_DC_V = %f V	\n " ,F_DC_V);					   //��ѹ
//	printf("F_DC_P = %f W	\n " ,F_DC_P);						  //�й�����
//	printf("F_DC_BACKUP_E = %f KWH \n " ,F_DC_BACKUP_E);			//����
//    printf("F_AC_LINE_Freq = %f Hz \n " ,F_AC_LINE_Freq);			//Ƶ��
//    printf("F_DC_PF = %f Hz \n " ,       F_DC_PF);			//��������
//    printf("F_DC_SA = %f W \n " ,  F_DC_SA);			//���ڹ��� 
//    printf("F_DC_WF = %f W,\n",F_DC_WF);                           //�޹�����
//    
//	
//	printf("\r\n");//���뻻��
//	printf("\r\n");//���뻻��
//	printf("----------------------------------------------\r\n");	
//	printf("----------------------------------------------\r\n");			
}

#endif
