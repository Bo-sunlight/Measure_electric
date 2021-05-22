/*=========================================================================================================
  * File Name	 : HLW8110.c
  * Describe 	 : HLW8110 UART 通讯程序,使用USATR2.(HLW8110 code,use USATE2)
  * Author	   : Tuqiang
  * Version	   : V1.3
  * Record	   : 2019/04/16，V1.2
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
//unsigned char	B_ReadReg_Time_EN;			// 串口读取寄存器数据，时间计数器标志位，1--开启计数，0--关闭计数
//unsigned char	B_Tx_Finish;
unsigned char	B_Rx_Finish;
unsigned char	B_Rx_Data_ING;					// 接收数据标志位	,		< 1:接收数据中,0:未接收到数据 >
unsigned char	B_Read_Error;							// UART读取出据校验和出错,< 1:数据读错，0:数据读取正确 >
//unsigned char	u8_ReadReg_Index;
//unsigned char	u8_ReadReg_Time;				// 串口读取寄存器数据的时间
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
unsigned int	U16_RMSIAC_RegData; 			// A通道电流转换系数
unsigned int	U16_RMSIBC_RegData; 			// B通道电流转换系数
unsigned int	U16_RMSUC_RegData; 				// 电压通道转换系数
unsigned int	U16_PowerPAC_RegData; 		// A通道功率转换系数
//unsigned int	U16_FreqAC_RegData; 		// A通道频率转换系数
unsigned int	U16_PowerPBC_RegData; 		// B通道功率转换系数
unsigned int	U16_PowerSC_RegData; 			// 视在功率转换系数,如果选择A通道，则是A通道视在功率转换系数。A和B通道只能二者选其一
unsigned int	U16_EnergyAC_RegData; 		// A通道有功电能(量)转换系数 
unsigned int	U16_EnergyBC_RegData; 		// A通道有功电能(量)转换系数

unsigned int	U16_CheckSUM_RegData; 		// 转换系数的CheckSum
unsigned int	U16_CheckSUM_Data; 				// 转换系数计算出来的CheckSum

unsigned int	U16_Check_SysconReg_Data; 						
unsigned int	U16_Check_EmuconReg_Data; 						
unsigned int	U16_Check_Emucon2Reg_Data; 				

/*---------------------------------------------------------------------------------------------------------*/			
/*---------------------------------------------------------------------------------------------------------*/
unsigned long 	U32_RMSIA_RegData;			// A通道电流有效值寄存器
unsigned long 	U32_RMSU_RegData;				// 电压有效值寄存器
unsigned long 	U32_POWERPA_RegData;		// A通道功率有效值寄存器
unsigned long 	U32_POWERSA_RegData;		// A通道功率视在功率寄存器
unsigned long 	U32_POWERPF_RegData;		// A通道功率功率因数寄存器
unsigned long 	U32_ENERGY_PA_RegData;	// A通道有功电能（量）有效值寄存器
unsigned long 	U32_Freq_RegData;		// A通道功率有效值寄存器


unsigned long 	U32_RMSIB_RegData;			// B通道电流有效值寄存器
unsigned long		U32_POWERPB_RegData;		// B通道功率有效值寄存器
unsigned long 	U32_ENERGY_PB_RegData;	// B通道有功电能（量）有效值寄存器
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
float   F_DC_V;													// 电压有效值
float   F_DC_I;													// A通道电流
float   F_DC_P;													// A通道有功功率
float   F_DC_E;													// A通道有功电能(量)
float   F_DC_BACKUP_E;									          // A通道电量备份	
float   F_DC_PF;												// 功率因素，A通道和B通道只能选其一 
float   F_DC_SA;	 //视在功率
float   F_DC_WF;	 //无功功率
float   F_Angle;												// 相角，A通道和B通道只能选其一 

float   F_AC_LINE_Freq;     						// 市电线性频率
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/


/*=========================================================================================================
 * Function : void USART2_IRQHandler(void)
 * Describe : UART2串口中断，用于接收数据,9600bps,偶校验,even
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2019/04/16
==========================================================================================================*/
void USART2_IRQ_User(void)
{
	//接收数据
	if(USART2->SR&(1<<5))	
	{	 
		USART2->SR;
		if (u8_RX_Index < u8_RX_Length )
		{
			u8_RxBuf[u8_RX_Index] = USART2->DR;		// 数据接收中
			u8_RX_Index++;
			
			B_Rx_Data_ING = 1;										// 置数据接收标志位
		}
		else
		{
			B_Rx_Finish = TRUE;										// 数据接收完毕
			u8_RX_Index = 0;

		}
	}
	
	//发送数据
	
}
/*=========================================================================================================
 * Function : void Start_Send_UartData(unsigned char len)
 * Describe : UART2串口发送数据
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
		
		while((USART2->SR&0X40) == 0);	//等待发送结束
		
		//delay_us(50);	//两个字节之间增加延时
		USART2->DR	=	u8_TxBuf[i];
		
	}

}
/*=========================================================================================================
 * Function : void Clear_RxBuf(void)
 * Describe : 在准备接收串口数据前，清空接收缓存器的数据
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
	u8_RX_Length = u8_reg_length + 1;	        // +1，是因为接收的数据长度，除了REG值，还有一个校验和数据
	
	
	Clear_RxBuf();										//清空接收缓冲区
	Start_Send_UartData(u8_TX_Length);
}
/*=========================================================================================================
 * Function : void Uart_Write_HLW8110_Reg(unsigned char ADDR_Reg,unsigned char u8_reg_length,unsigned long u32_data)
 * Describe : 写寄存器命令，u8_reg_length：写入的寄存器数据字节长度
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
			u8_TxBuf[i+2] = Temp_u32_a.byte[u8_reg_length-1-i];						//STM32，32位MCU，union定义，是低位在前
			//u8_TxBuf[i+2] = Temp_u32_a.byte[4-u8_reg_length + i];				//STM8,STC MCU,   union定义，是高位在前
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
 * Describe : 验证地址0x70-0x77地址的系数和
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
 
  //读取RmsIAC、RmsIBC、RmsUC、PowerPAC、PowerPBC、PowerSC、EnergAc、EnergBc的值
	
	Uart_Read_HLW8110_Reg(REG_RMS_IAC_ADDR,2);
	delay_ms(50);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_RMSIAC_RegData = (u8_RxBuf[0]<<8) + u8_RxBuf[1] ;
		printf("A通道电流转换系数:%x\n " ,U16_RMSIAC_RegData);
	}
	
	
	Uart_Read_HLW8110_Reg(REG_RMS_IBC_ADDR,2);
	delay_ms(50);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_RMSIBC_RegData = (u8_RxBuf[0]<<8) + u8_RxBuf[1] ;
		printf("B通道电流转换系数:%x\n " ,U16_RMSIBC_RegData);
	}
	
	
	Uart_Read_HLW8110_Reg(REG_RMS_UC_ADDR,2);
	delay_ms(50);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_RMSUC_RegData = (u8_RxBuf[0]<<8) + u8_RxBuf[1] ;
		printf("电压通道转换系数:%x\n " ,U16_RMSUC_RegData);
	}
		
	Uart_Read_HLW8110_Reg(REG_POWER_PAC_ADDR,2);
	delay_ms(50);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_PowerPAC_RegData = (u8_RxBuf[0]<<8) + u8_RxBuf[1] ;
		printf("A通道功率转换系数:%x\n " ,U16_PowerPAC_RegData);
	}
		
	Uart_Read_HLW8110_Reg(REG_POWER_PBC_ADDR,2);
	delay_ms(50);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_PowerPBC_RegData = (u8_RxBuf[0]<<8) + u8_RxBuf[1] ;
		printf("B通道功率转换系数:%x\n " ,U16_PowerPAC_RegData);
	}
	
	Uart_Read_HLW8110_Reg(REG_POWER_SC_ADDR,2);
	delay_ms(50);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_PowerSC_RegData = (u8_RxBuf[0]<<8) + u8_RxBuf[1] ;
		printf("视在功率转换系数:%x\n " ,U16_PowerSC_RegData);
	}
	
	Uart_Read_HLW8110_Reg(REG_ENERGY_AC_ADDR,2);
	delay_ms(50);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_EnergyAC_RegData = (u8_RxBuf[0]<<8) + u8_RxBuf[1] ;
		printf("A通道电量转换系数:%x\n " ,U16_EnergyAC_RegData);
	
	}
	Uart_Read_HLW8110_Reg(REG_ENERGY_BC_ADDR,2);
	delay_ms(50);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_EnergyBC_RegData = (u8_RxBuf[0]<<8) + u8_RxBuf[1] ;
		printf("B通道电量转换系数:%x\n " ,U16_EnergyBC_RegData);
	}
	
 
	Uart_Read_HLW8110_Reg(REG_CHECKSUM_ADDR,2);
	delay_ms(50);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_CheckSUM_RegData= (u8_RxBuf[0]<<8) + u8_RxBuf[1] ;
		printf("系数校验和:%x\n " ,U16_CheckSUM_RegData);
	}
	
	
	a = 0;
    a = ~(0xffff+U16_RMSIAC_RegData + U16_RMSIBC_RegData + U16_RMSUC_RegData + 
        U16_PowerPAC_RegData + U16_PowerPBC_RegData + U16_PowerSC_RegData + 
          U16_EnergyAC_RegData + U16_EnergyBC_RegData  );
  
	U16_CheckSUM_Data = a & 0xffff;
	
	printf("计算系数校验和:%x\n " ,U16_CheckSUM_Data);
  
	if ( U16_CheckSUM_Data == U16_CheckSUM_RegData)
	{
		d = 1;
		printf("校验和正确\r\n ");
    
	}
	else
	{
		d = 0;
		printf("校验和出错\r\n ");
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
	
//电流通道A设置命令，指定当前用于计算视在功率、功率因数、相角、瞬时有功功率、瞬时视在功率和有功功率过载的信号指示 的通道为通道A	
	Uart_HLW8110_Set_Channel_A();			
	delay_ms(10);
	Uart_Write_HLW8110_Reg(REG_SYSCON_ADDR,2,0x0a00);	//开启A通道，关闭B通道，电压通道PGA = 1，电流通道PGA = 1
	delay_ms(10);
    
   Uart_Write_HLW8110_Reg(RmsIAOS,2,0xf8fd);	//开启A通道，关闭B通道，电压通道PGA = 1，电流通道PGA = 1
	delay_ms(10);
	
//在这里改直流  0x0001为测交流，0x0071为测直流     
    if(DC_Flage == 1)
	       Uart_Write_HLW8110_Reg(REG_EMUCON_ADDR,2,0x0071);	//1，使能PFA 脉冲输出和有功电能寄存器累加；
    else
        Uart_Write_HLW8110_Reg(REG_EMUCON_ADDR,2, 0x0001);      
//	Uart_Write_HLW8110_Reg(REG_EMUCON_ADDR,2,0x0018);	//正向和负向过零点均发生变化，ZXD0 = 1，ZXD1 = 1
    
// 过压、过流、过载检测使能信号 过零检测、相角、电压频率测量使能信号 峰值检测使能信号  数据更新频率
	Uart_Write_HLW8110_Reg(REG_EMUCON2_ADDR,2,0x0465);	//0x0001是EMUCON2的默认值，waveEn = 1,zxEn = 1，A通道电量寄存器，读后不清0，EPA_CB = 1；打开功率因素检测
	delay_ms(10);
    
	Uart_HLW8110_WriteREG_DIS();
//	delay_ms(10);	
  //读取地址是0x6F至0x77的寄存器，验证系数是否正确
  Judge_CheckSum_HLW8110_Calfactor();	
}


/*=========================================================================================================
 * Function : void Check_WriteReg_Success(void)
 * Describe : 检验写入的REG是否正确写入
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
//		printf("写入的SysconReg寄存器:%lx\n " ,U16_Check_SysconReg_Data);
	}
	else
	{
		printf("SysconReg寄存器读取出错\r\n");
		B_Read_Error = 1;
	}
	
	Uart_Read_HLW8110_Reg(REG_EMUCON_ADDR,2);
	delay_ms(10);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_Check_EmuconReg_Data =  (unsigned int)(u8_RxBuf[0]<<8) + (unsigned int)(u8_RxBuf[1]); 
//		printf("写入的EmuconReg寄存器:%lx\n " ,U16_Check_EmuconReg_Data);
	}
	else
	{
		printf("EmuconReg寄存器读取出错\r\n");
		B_Read_Error = 1;
	}
	
	
	Uart_Read_HLW8110_Reg(REG_EMUCON2_ADDR,2);
		delay_ms(10);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_Check_Emucon2Reg_Data =  (u8_RxBuf[0]<<8) + (u8_RxBuf[1]); 
//		printf("写入的Emucon2Reg寄存器寄存器:%lx\n " ,U16_Check_Emucon2Reg_Data);
	}
	else
	{
		printf("Emucon2Reg寄存器读取出错\r\n");
		B_Read_Error = 1;
	}
	
}

/*=========================================================================================================
 * Function : void Read_HLW8112_IA(void)
 * Describe : 读取A通道电流
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
//		printf("A通道电流寄存器:%lx\n " ,U32_RMSIA_RegData);
	}
	else
	{
		printf("A通道电流寄存器读取出错\r\n");
		B_Read_Error = 1;
	}
	
	
	//A通道电流PGA = 16,电压通道PGA = 1;电流采样电阻1mR,电压采样电阻1M
    //计算公式,U16_AC_I = (U32_RMSIA_RegData * U16_RMSIAC_RegData)/(电流系数* 2^23）
	if ((U32_RMSIA_RegData & 0x800000) == 0x800000)
	 {
			F_DC_I = 0;
	 }
	 else
	 {
		a = (float)U32_RMSIA_RegData;
		a = a * U16_RMSIAC_RegData;
		a  = a/0x800000;                     //电流计算出来的浮点数单位是mA,比如5003.12 
   if(DC_Flage==1)
		a = a/490.0001;  								// 1 = 电流系数
   
   else  
        a = a/0.153285;

		a = a/1000;              //a= 5003ma,a/1000 = 5.003A,单位转换成A
		a = a * 0.95;				//D_CAL_A_I是校正系数，默认是1
		F_DC_I = a;
	 }
}
/*=========================================================================================================
 * Function : void Read_HLW8110_U(void)
 * Describe : 读取电压
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
//		printf("电压通道寄存器:%lx\n " ,U32_RMSU_RegData);
	}
	else
	{
		printf("电压通道寄存器读取出错\r\n");
		B_Read_Error = 1;
	}
	
		//电压
	//计算:U16_AC_V = (U32_RMSU_RegData * U16_RMSUC_RegData)/2^23
	
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
  a = a/20.10;  						// 1 = 电压系数
  a = a/100;    				    //计算出a = 22083.12mV,a/100表示220.8312V，电压转换成V
    
if(a>=20)
  a = a*0.995;			      	//D_CAL_U是校正系数，默认是1,

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
 * Describe : 读取A通道功率
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
//		printf("A通道功率寄存器:%lx\n " ,U32_POWERPA_RegData);
	}
	else
	{
		printf("A通道功率寄存器读取出错\r\n");
		B_Read_Error = 1;
	}
	
	
	 if (U32_POWERPA_RegData > 0x80000000)      //为负数
   {
     b = ~U32_POWERPA_RegData;
     a = (float)b;
   }
   else
     a =  (float)U32_POWERPA_RegData;
     
//功率需要分正功和负功
//计算,U16_AC_P = (U32_POWERPA_RegData * U16_PowerPAC_RegData)/(2^31*电压系数*电流系数)
//单位为W,比如算出来5000.123，表示5000.123W	
    a = a*U16_PowerPAC_RegData;
    a = a/0x80000000;
if(DC_Flage == 1)
{    
    a = a/490.0001;  										// 1 = 电流系数
    a = a/20.10;  										// 1 = 电压系数
}
else
{
    a = a/0.762;  										// 1 = 电流系数
    a = a/1.043;  										// 1 = 电压系数
}

    a = a * text_data;						//D_CAL_A_P是校正系数，默认是1
    F_DC_P = a;									//单位为W,比如算出来5000.123，表示5000.123W

}

/*=========================================================================================================
 * Function : void Read_HLW8110_PA(void)
 * Describe : 读取A通道功率因数
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2019/04/03
==========================================================================================================*/
void Read_HLW8110_PF(void)
{
	float a;
	//计算公式：PF = |PF|/0x7FFFFF;
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
 * Describe : 读取A通道视在功率
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
//		printf("A通道功率寄存器:%lx\n " ,U32_POWERPA_RegData);
	}
	else
	{
		printf("A通道功率寄存器读取出错\r\n");
		B_Read_Error = 1;
	}
    
    a = U32_POWERSA_RegData;
	F_DC_SA = F_DC_P/F_DC_PF;
    F_DC_WF= sqrt(F_DC_SA*F_DC_SA-F_DC_P*F_DC_P);

}
/*=========================================================================================================
 * Function : void void Read_HLW8112_EA(void)
 * Describe : 读取A通道有功电量
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
//		printf("A通道有功电量寄存器:%lx\n " ,U32_ENERGY_PA_RegData);
	}
	else
	{
		printf("A通道有功电量寄存器读取出错\r\n");
		B_Read_Error = 1;
	}
	
	Uart_Read_HLW8110_Reg(REG_HFCONST_ADDR,2); 
	delay_ms(10);
	if ( u8_RxBuf[u8_RX_Length-1] == HLW8110_checkSum_Read(u8_RX_Length) )
	{
		U16_HFConst_RegData = (unsigned int)(u8_RxBuf[0]<<8) + (unsigned int)(u8_RxBuf[1]);
//		printf("HFCONST常数 = :%d\n " ,U16_HFConst_RegData);
	}
	else
	{
		printf("HFCONST常数寄存器读取出错\r\n");
		B_Read_Error = 1;
	}

	//电量计算,电量 = (U32_ENERGY_PA_RegData * U16_EnergyAC_RegData * HFCONST) /(K1*K2 * 2^29 * 4096)
	//HFCONST:默认值是0x1000, HFCONST/(2^29 * 4096) = 0x20000000
	a =  (float)U32_ENERGY_PA_RegData;	
    a = a*U16_EnergyAC_RegData;
    a = a/0x20000000;             //电量单位是0.001KWH,比如算出来是2.002,表示2.002KWH    
if(DC_Flage == 1)
{    
    a = a/490.0001;  										// 1 = 电流系数
    a = a/20.10;  										// 1 = 电压系数
}
else
{
    a = a/0.762;  										// 1 = 电流系数
    a = a/1.043;  										// 1 = 电压系数
}
    a = a * D_CAL_A_E;     			//D_CAL_A_E是校正系数，默认是1
    F_DC_E = a;
	F_DC_BACKUP_E = F_DC_E;
		
}


/*=========================================================================================================
 * Function : void Read_HLW8110_PA(void)
 * Describe : 读取A通道频率
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
		printf("A通道频率寄存器读取出错\r\n");
		B_Read_Error = 1;
	}
	
    a =(float)1.0/U32_Freq_RegData;	
if(DC_Flage == 1)
{    
    a = a/100000;  										// 1 = 电流系数
    a = a/100000;  										// 1 = 电压系数
}

else
{
    a = a*3579545;  										// 1 = 电流系数
    a = a/8;  										// 1 = 电压系数
}

    F_AC_LINE_Freq = a;									//单位为W,比如算出来5000.123，表示5000.123W

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
	
	Read_HLW8110_IA();                //电流有效值
	Read_HLW8110_U();                //电压有效值
	Read_HLW8110_PA();               //有功功率
	Read_HLW8110_EA();	             //有功电量
    Read_HLW8110_Freq();             //电压频率
    Read_HLW8110_SA();               //视在功率
    Read_HLW8110_PF();              //功率因数
//	printf("\r\n");	
//	printf("\r\n");	
//   if(DC_Flage == 1)
//   {
//	   printf("直流测量,uart通讯方式\r\n");
//   }
//   else 
//       printf("交流测量,uart通讯方式\r\n");
//   
//	printf("A通道电流转换系数:%x\n " ,U16_RMSIAC_RegData);
//	printf("电压通道转换系数:%x\n " ,U16_RMSUC_RegData);
//	printf("A通道功率转换系数:%x\n " ,U16_PowerPAC_RegData);
//	printf("视在功率转换系数:%x\n " ,U16_PowerSC_RegData);
//	printf("A通道电量转换系数:%x\n " ,U16_EnergyAC_RegData);
//	printf("转换系数校验和:%x\n " ,U16_CheckSUM_RegData);
//	printf("转换系数计算出的校验和:%x\n " ,U16_CheckSUM_Data);
//	
//	printf("\r\n");//插入换行
//	printf("A通道电流寄存器:%x\n " ,U32_RMSIA_RegData);
//	printf("电压寄存器:%x\n " ,U32_RMSU_RegData);
//	printf("A通道功率寄存器:%x\n " ,U32_POWERPA_RegData);
//	printf("A通道电量寄存器:%x\n " ,U32_ENERGY_PA_RegData);
//    printf("A通道频率寄存器:%x\n " ,   U32_Freq_RegData);
//     printf("A通道视在功率寄存器:%x\n " ,U32_POWERSA_RegData);
//     printf("A通道功率因数寄存器:%x\n " ,U32_POWERPF_RegData);
//     
//	printf("\r\n");                                               //插入换行
//	printf("F_DC_I = %f A \n " ,F_DC_I);					 	//电流
//	printf("F_DC_V = %f V	\n " ,F_DC_V);					   //电压
//	printf("F_DC_P = %f W	\n " ,F_DC_P);						  //有功功率
//	printf("F_DC_BACKUP_E = %f KWH \n " ,F_DC_BACKUP_E);			//电量
//    printf("F_AC_LINE_Freq = %f Hz \n " ,F_AC_LINE_Freq);			//频率
//    printf("F_DC_PF = %f Hz \n " ,       F_DC_PF);			//功率因数
//    printf("F_DC_SA = %f W \n " ,  F_DC_SA);			//视在功率 
//    printf("F_DC_WF = %f W,\n",F_DC_WF);                           //无功功率
//    
//	
//	printf("\r\n");//插入换行
//	printf("\r\n");//插入换行
//	printf("----------------------------------------------\r\n");	
//	printf("----------------------------------------------\r\n");			
}

#endif
