#ifndef _TIMER_H
#define _TIMER_H
#include "includes.h"

#define TIM2_INTERRUPT 1  //定时器2中断开启

void TIM2_Int(u16 arr,u16 psc);

#endif
