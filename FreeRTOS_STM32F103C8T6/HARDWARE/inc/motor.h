#ifndef __MOTOR_H
#define __MOTOR_H
#include "includes.h"

#define PWMD   TIM8->CCR4  
#define PWMC   TIM8->CCR3  
#define PWMB   TIM8->CCR2 
#define PWMA   TIM8->CCR1 

#define BIN2   PBout(0)
#define BIN1   PBout(1)

#define AIN1   PCout(4)
#define AIN2   PCout(5)

#define CIN1   PDout(5)
#define CIN2   PDout(6)

#define DIN1   PDout(3)
#define DIN2   PDout(4)

void Motor_Init(void);
void Set_Pwm(int moto1,int moto2);
int  myabs(int a);
void TIM8_PWM_Init(u16 arr,u16 psc);
int Xianfu_Pwm(int target);
void Turn_Off(float angle, float voltage);

void TIM1_PWM_Init(u16 arr,u16 psc);

#endif
