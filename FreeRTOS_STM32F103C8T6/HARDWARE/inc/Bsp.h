#ifndef _BSP_H_
#define _BSP_H_

#include "includes.h"
/* 提供给其他C文件调用的函数 */
void bsp_Init(void);
void bsp_Idle(void);
extern TimerHandle_t xTimers1;
extern TaskHandle_t OLEDask_Handler;
#endif

