#include "Timer2.h"

void TIM2_Int(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  ///ʹ��TIM2ʱ��
	
    TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//��ʼ��TIM2

	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //��ʱ��2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=4; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
    
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
    TIM_Cmd(TIM2,ENABLE); //ʹ�ܶ�ʱ��3
}

