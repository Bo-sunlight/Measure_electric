#ifndef __HLW8110_H
#define __HLW8110_H	 
#include "includes.h"
#include "core_cm3.h"
#include "stdio.h"
extern u8 DC_Flage;
extern float   F_DC_V;													// ��ѹ��Чֵ
extern float   F_DC_I;													// Aͨ������
extern float   F_DC_P;													// Aͨ���й�����
extern float   F_DC_SA;	 //���ڹ���
extern float   F_DC_E;													// Aͨ���й�����(��)
extern float   F_DC_BACKUP_E;									        // Aͨ����������	
extern float   F_DC_PF;											     	// �������أ�Aͨ����Bͨ��ֻ��ѡ��һ 
extern float   F_Angle;												    // ��ǣ�Aͨ����Bͨ��ֻ��ѡ��һ 
extern float   F_DC_WF;    //�޹�����

//extern float   F_DC_I_B;												// Bͨ��������Чֵ
//extern float   F_DC_P_B;												// Bͨ���й�����
//extern float 	F_DC_E_B;												// Bͨ���й�����(��)
//extern float   F_DC_BACKUP_E_B;								// Bͨ����������	
extern float   F_AC_LINE_Freq;     						// �е�����Ƶ��
//8112Aͨ����8110ͨ��У��ϵ��
#define D_CAL_U		1000/1000		//��ѹУ��ϵ��
#define D_CAL_A_I	1000/1000		//Aͨ������У��ϵ��
#define D_CAL_A_P	1000/1000		//Aͨ������У��ϵ��
#define D_CAL_A_E	0.813		//Aͨ������У��ϵ��


//8112 Bͨ��У��ϵ��
#define D_CAL_B_P	1000/1000		//Bͨ������У��ϵ��
#define D_CAL_B_I	1000/1000		//Bͨ������У��ϵ��
#define D_CAL_B_E	1000/1000		//Bͨ������У��ϵ��


//8112/8110 reg define
#define REG_SYSCON_ADDR         0x00
#define RmsIAOS                 0x0F
#define REG_EMUCON_ADDR         0x01
#define REG_HFCONST_ADDR        0x02
#define REG_EMUCON2_ADDR        0x13
#define REG_ANGLE_ADDR        	0x22			//��ǼĴ���
#define REG_UFREQ_ADDR          0x23     	//�е�����Ƶ�ʵ�ַ
#define REG_RMSIA_ADDR          0x24
#define REG_RMSIB_ADDR          0x25
#define REG_RMSU_ADDR           0x26
#define REG_PF_ADDR             0x27
#define REG_ENERGY_PA_ADDR			0x28
#define REG_ENERGY_PB_ADDR			0x29
#define REG_POWER_PA_ADDR       0x2C
#define REG_POWER_PB_ADDR       0x2D
#define REG_POWER_SA_ADDR       0x2E           //���ڹ��ʵ�ַ

#define REG_POWER_PS_ADDR       0x27          //���������Ĵ�����ַ
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


//�궨��
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
