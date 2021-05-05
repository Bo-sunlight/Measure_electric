#ifndef __TIMEE_H
#define __TIMEE_H
#include "includes.h" 


#define TIM3_INTERRUPT 1  //定时器2中断开启

void TIM3_Int_Init(u16 arr,u16 psc);

#endif

