#ifndef __HLW8110_H
#define __HLW8110_H	 
#include "includes.h"
#include "core_cm3.h"
#include "stdio.h"
extern u8 DC_Flage;
extern float   F_DC_V;													// 电压有效值
extern float   F_DC_I;													// A通道电流
extern float   F_DC_P;													// A通道有功功率
extern float   F_DC_SA;	 //视在功率
extern float   F_DC_E;													// A通道有功电能(量)
extern float   F_DC_BACKUP_E;									        // A通道电量备份	
extern float   F_DC_PF;											     	// 功率因素，A通道和B通道只能选其一 
extern float   F_Angle;												    // 相角，A通道和B通道只能选其一 
extern float   F_DC_WF;    //无功功率

//extern float   F_DC_I_B;												// B通道电流有效值
//extern float   F_DC_P_B;												// B通道有功功率
//extern float 	F_DC_E_B;												// B通道有功电能(量)
//extern float   F_DC_BACKUP_E_B;								// B通道电量备份	
extern float   F_AC_LINE_Freq;     						// 市电线性频率
//8112A通道或8110通道校正系数
#define D_CAL_U		1000/1000		//电压校正系数
#define D_CAL_A_I	1000/1000		//A通道电流校正系数
#define D_CAL_A_P	1000/1000		//A通道功率校正系数
#define D_CAL_A_E	0.813		//A通道电能校正系数


//8112 B通道校正系数
#define D_CAL_B_P	1000/1000		//B通道功率校正系数
#define D_CAL_B_I	1000/1000		//B通道电流校正系数
#define D_CAL_B_E	1000/1000		//B通道电能校正系数


//8112/8110 reg define
#define REG_SYSCON_ADDR         0x00
#define RmsIAOS                 0x0F
#define REG_EMUCON_ADDR         0x01
#define REG_HFCONST_ADDR        0x02
#define REG_EMUCON2_ADDR        0x13
#define REG_ANGLE_ADDR        	0x22			//相角寄存器
#define REG_UFREQ_ADDR          0x23     	//市电线性频率地址
#define REG_RMSIA_ADDR          0x24
#define REG_RMSIB_ADDR          0x25
#define REG_RMSU_ADDR           0x26
#define REG_PF_ADDR             0x27
#define REG_ENERGY_PA_ADDR			0x28
#define REG_ENERGY_PB_ADDR			0x29
#define REG_POWER_PA_ADDR       0x2C
#define REG_POWER_PB_ADDR       0x2D
#define REG_POWER_SA_ADDR       0x2E           //视在功率地址

#define REG_POWER_PS_ADDR       0x27          //功率因数寄存器地址
#define REG_Ufreq_ADDR       0x23



#define REG_OVLVL_ADDR          0x19

#define REG_INT_ADDR          	0x1D
#define REG_IE_ADDR          		0x40
#define REG_IF_ADDR          		0x41
#define REG_RIF_ADDR          	0x42

#define REG_RDATA_ADDR          0x44


#define REG_CHECKSUM_ADDR				0x6f
 #define REG_RMS_IAC_ADDR				0x70
#define REG_RMS_IBC_ADDR				0x71
#define REG_RMS_UC_ADDR					0x72
#define REG_POWER_PAC_ADDR			0x73
#define REG_POWER_PBC_ADDR			0x74
#define REG_POWER_SC_ADDR				0x75
#define REG_ENERGY_AC_ADDR			0x76
#define REG_ENERGY_BC_ADDR			0x77


//宏定义
#define FALSE	0
#define TRUE	0

#define D_TIME1_50MS	50


void USART2_IRQ_User(void);
void Init_HLW8110(void);
void Calculate_HLW8110_MeterData(void);
void Init_HLW8112(void);
void HLW8112_Measure(void);
void Read_HLW8110_Freq(void);
void Read_HLW8110_SA(void);
void Read_HLW8110_PF(void);

#endif
