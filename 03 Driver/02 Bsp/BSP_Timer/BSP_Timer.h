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

#ifndef _BSP_TIMER_H_
#define _BSP_TIMER_H_	

#include "stdint.h"
#include "stdbool.h"
#include "stm32f10x.h"

/*定时器枚举转义为定时器结构体指针*/
#define TIMER(n)		(TIM_TypeDef *)(TIM2_BASE + 0x0400 * n)

/*定时器枚举*/
typedef enum{
	_T2_ = 0,
	_T3_,
	_T4_,
	_T5_,
	_T6_,
	_T7_,
	_TIMER_BUTT_
} BSP_TIMER_ENUM;

/*定时器中断函数注册表*/
extern void(* BSP_TimerIRQHandler[])(void);

/* 初始化
  ----------------------------------------------------------------------*/

/* 初始化定时器中断
  -----------------------------------------
  入口：定时器编号、周期、预分频数
  返回值：成功true，失败false
*/
extern bool BSP_Init_TimerInterrupt(BSP_TIMER_ENUM timer_number, uint16_t period, uint16_t prescaler);

/* 操作
  ----------------------------------------------------------------------*/

/* 设置定时器计时值
  -----------------------------------------
  入口：定时器编号、设置的定时器溢出记数值
*/
extern void BSP_SetTimer(BSP_TIMER_ENUM timer_number, uint32_t time);

/* 启动定时器工作
  -----------------------------------------
  入口：定时器编号
*/
extern void BSP_EnTimer(BSP_TIMER_ENUM timer_number);

/* 中止定时器工作
  -----------------------------------------
  入口：定时器编号
*/
extern void BSP_DisTimer(BSP_TIMER_ENUM timer_number);

#endif
