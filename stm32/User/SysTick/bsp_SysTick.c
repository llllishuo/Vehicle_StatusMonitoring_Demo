//SysTick_Config主要用来配置中端向量，重置STK_VAL寄存器，配置SysTick时钟为AHB 
#include "./systick/bsp_SysTick.h"
 

volatile uint32_t g_ul_ms_ticks=0;
static volatile uint32_t TimingDelay=0;

 
/**
  * @brief  启动系统滴答定时器 SysTick
  * @param  无
  * @retval 无
  */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
//	if (SysTick_Config(SystemFrequency / 100000))	// ST3.0.0库版本

	if (SysTick_Config(SystemCoreClock/100000))	// ST3.5.0库版本SystemCoreClock/10不能超过16777216
	{ 

		/* Capture error */ 
		while (1);
	}

		// 关闭滴答定时器  
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}



void udelay(unsigned long nTime)
{
	TimingDelay = nTime;
	SysTick->CTRL |= SysTick_CALIB_NOREF_Msk;
	while(TimingDelay != 0);
}

int get_tick_count(unsigned long *count)
{
  count[0] = g_ul_ms_ticks;
	return 0;
}

void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
		TimingDelay--;
}

void TimeStamp_Increment(void)
{
	g_ul_ms_ticks++;

}


/*********************************************END OF FILE**********************/
