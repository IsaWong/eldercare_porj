#ifndef __SYSTICK_H
#define __SYSTICK_H


#include "stm32f10x.h"

#define TASK_ENABLE 0
#define NumOfTask 3

void SysTick_Init(void);
void Delay_us(__IO u32 nTime);         // ��λ1us


#define Delay_ms(x) Delay_us(1000*x)	 //��λms


#endif /* __SYSTICK_H */
