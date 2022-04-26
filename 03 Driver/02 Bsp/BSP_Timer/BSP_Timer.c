/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——BSP层 Timer封装

  -------------------------------------------------------
   文 件 名   : BSP_Timer.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年07月17日
   功能描述   : 定义一种基于STM32的板级支持包。
				该层依赖于硬件电路。
   依赖于     : STM32F10x_StdPeriph_Lib_V3.5.0
 ********************************************************/
 
#include "BSP_Timer.h"

/*定时器中断函数注册表*/
void(* BSP_TimerIRQHandler[6])(void) = {0};	/*定时器中断函数指针列表*/

/* 初始化
  ----------------------------------------------------------------------*/

/* 初始化定时器中断
  -----------------------------------------
  入口：定时器编号、周期、预分频数
  返回值：成功true，失败false
*/
bool BSP_Init_TimerInterrupt(BSP_TIMER_ENUM timer_number, uint16_t period, uint16_t prescaler)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};
	
	/*使能定时器时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 << timer_number, ENABLE);	
	
	/*初始化TIM中断优先级*/
	switch(timer_number)
	{
		case _T2_:
			NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;		
			break;
		case _T3_:
			NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;		
			break;
		case _T4_:
			NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;		
			break;
		#ifdef STM32F10X_HD
		case _T5_:
			NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;		
			break;
		case _T6_:
			NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;		
			break;
		case _T7_:
			NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;		
			break;
		#endif
		default:
			return false;
	}	
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			/*2位抢占优先级，2位响应优先级*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	
	
	/*初始化定时器*/
    TIM_DeInit(TIMER(timer_number));										    
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);			/* Fpclk1 = 72MHz */
    TIM_TimeBaseStructure.TIM_Period        = period;		/*时钟周期*/
    TIM_TimeBaseStructure.TIM_Prescaler     = prescaler;	/*预分频数   Fpclk1 / (8+1) = 8M*/
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIMER(timer_number), &TIM_TimeBaseStructure);
    TIM_ARRPreloadConfig(TIMER(timer_number), DISABLE);					/*不用预装载，直接修改到影子寄存器*/
    TIM_SelectOutputTrigger(TIMER(timer_number), TIM_TRGOSource_Update);	/* TIMER TRGO selection */
    TIM_ClearFlag(TIMER(timer_number), TIM_FLAG_Update);					/*清定时器溢出标志位*/
    TIM_ITConfig(TIMER(timer_number), TIM_IT_Update, ENABLE);				/*使能定时器溢出中断*/
	
	return true;
}


/* 操作
  ----------------------------------------------------------------------*/

/* 设置定时器计时值
  -----------------------------------------
  入口：定时器编号、设置的定时器溢出记数值
*/
void BSP_SetTimer(BSP_TIMER_ENUM timer_number, uint32_t time)
{
	TIM_SetAutoreload(TIMER(timer_number), time);
}

/* 启动定时器工作
  -----------------------------------------
  入口：定时器编号
*/
void BSP_EnTimer(BSP_TIMER_ENUM timer_number)
{
	TIM_Cmd(TIMER(timer_number), ENABLE);
}

/* 中止定时器工作
  -----------------------------------------
  入口：定时器编号
*/
void BSP_DisTimer(BSP_TIMER_ENUM timer_number)
{
	TIM_Cmd(TIMER(timer_number), DISABLE);
	TIM_ClearITPendingBit(TIMER(timer_number), TIM_IT_Update);
	TIM_SetCounter(TIMER(timer_number), 0);	/*20200221 增加，解决STEP信号突然中断的问题*/
}

/* 中断函数
  ----------------------------------------------------------------------*/
 
/* TIM2中断
  --------------------------------
*/
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)	/*产生了更新中断*/
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);		/*清除更新中断*/
		if(BSP_TimerIRQHandler[0] != 0)
			(*BSP_TimerIRQHandler[0])();
	}
}

/* TIM3中断
  --------------------------------
*/
void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)	/*产生了更新中断*/
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);		/*清除更新中断*/
		if(BSP_TimerIRQHandler[1] != 0)
			(*BSP_TimerIRQHandler[1])();
	}
}

/* TIM4中断
  --------------------------------
*/
void TIM4_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)	/*产生了更新中断*/
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);		/*清除更新中断*/
		if(BSP_TimerIRQHandler[2] != 0)
			(*BSP_TimerIRQHandler[2])();
	}
}

/* TIM5中断
  --------------------------------
*/
void TIM5_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)	/*产生了更新中断*/
    {
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);		/*清除更新中断*/
		if(BSP_TimerIRQHandler[3] != 0)
			(*BSP_TimerIRQHandler[3])();
	}
}

/* TIM6中断
  --------------------------------
*/
void TIM6_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)	/*产生了更新中断*/
    {
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);		/*清除更新中断*/
		if(BSP_TimerIRQHandler[4] != 0)
			(*BSP_TimerIRQHandler[4])();
	}
}

/* TIM7中断
  --------------------------------
*/
void TIM7_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)	/*产生了更新中断*/
    {
        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);		/*清除更新中断*/
		if(BSP_TimerIRQHandler[5] != 0)
			(*BSP_TimerIRQHandler[5])();
	}
}
