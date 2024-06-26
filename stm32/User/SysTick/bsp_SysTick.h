#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"
#include "stdio.h"
#define mdelay(x) udelay(100*x)

int get_tick_count(unsigned long *count);
void udelay(unsigned long nTime);
void SysTick_Init(void);

#endif /* __SYSTICK_H */
