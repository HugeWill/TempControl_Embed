/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——BSP层 系统封装

  -------------------------------------------------------
   文 件 名   : BSP_Sys.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年07月17日
   功能描述   : 定义一种基于STM32的板级支持包。
				该层依赖于硬件电路。
   依赖于     : STM32F10x_StdPeriph_Lib_V3.5.0
 ********************************************************/
 
#include "BSP_Sys.h"

/* 初始化
  ----------------------------------------------------------------------*/

/* 初始化BSP
  -----------------------------------------
  返回值：成功true，失败false
*/
bool BSP_Init(void)
{
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); 
	
	return true;
}

/* 初始化SysTick
  -----------------------------------------
  入口参数：滴答时钟频率、每秒滴答次数
  返回值：成功true，失败false
*/
bool BSP_InitSysTick(uint8_t clk, uint16_t ticks_per_sec)
{
	uint32_t reload;
	
 	SysTick->CTRL &= ~(1 << 2);				/*SYSTICK使用外部时钟源*/
	    
	reload = clk / 8;						/*每秒钟的计数次数 单位为K*/
	reload *= 1000000 / ticks_per_sec;		/*设定溢出时间*/
											/*reload为24位寄存器,最大值:16777216,在72M下,约合1.86s左右*/
	SysTick->CTRL |= 1 << 1;				/*开启SYSTICK中断*/
	SysTick->LOAD  = reload;				/*每1/OS_TICKS_PER_SEC秒中断一次*/
	SysTick->CTRL |= 1 << 0;				/*开启SYSTICK*/
	
	return true;
}

/* 软复位
  -----------------------------------------
  返回值：无
*/
void BSP_Reboot(void)
{
	__set_FAULTMASK(1);		/*关闭中断*/
	NVIC_SystemReset();		/*软复位*/
}
