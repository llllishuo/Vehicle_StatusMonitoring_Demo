//SysTick_Config��Ҫ���������ж�����������STK_VAL�Ĵ���������SysTickʱ��ΪAHB 
#include "./systick/bsp_SysTick.h"
 

volatile uint32_t g_ul_ms_ticks=0;
static volatile uint32_t TimingDelay=0;

 
/**
  * @brief  ����ϵͳ�δ�ʱ�� SysTick
  * @param  ��
  * @retval ��
  */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
//	if (SysTick_Config(SystemFrequency / 100000))	// ST3.0.0��汾

	if (SysTick_Config(SystemCoreClock/100000))	// ST3.5.0��汾SystemCoreClock/10���ܳ���16777216
	{ 

		/* Capture error */ 
		while (1);
	}

		// �رյδ�ʱ��  
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
