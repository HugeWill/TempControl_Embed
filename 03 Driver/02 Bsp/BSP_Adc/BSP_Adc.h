/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——BSP层 ADC封装

  -------------------------------------------------------
   文 件 名   : BSP_Adc.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2020年02月17日
   功能描述   : 定义一种基于STM32的板级支持包。
				该层依赖于硬件电路。
   依赖于     : STM32F10x_StdPeriph_Lib_V3.5.0
 ********************************************************/

#ifndef _BSP_ADC_H_
#define _BSP_ADC_H_

#include "stdint.h"
#include "stdbool.h"
#include "stm32f10x.h"
#include "BSP_Gpio.h"

/*ADC枚举*/
typedef enum{
	_ADC1_ = 0,
	_ADC2_,
	_ADC3_,
	_ADC_BUTT_
} BSP_ADC_ENUM;

/*ADC通道枚举*/
typedef enum{
	_CH0_ = 0,
	_CH1_,
	_CH2_,
	_CH3_,
	_CH4_,
	_CH5_,
	_CH6_,
	_CH7_,
	_CH8_,
	_CH9_,
	_CH10_,
	_CH11_,
	_CH12_,
	_CH13_,
	_CH14_,
	_CH15_,
	_CH_BUTT_
} BSP_CH_ENUM;

/* 初始化
  ----------------------------------------------------------------------*/

/* 初始化ADC
  -----------------------------------------
  入口：ADC编号，通道号
  返回值：成功true，失败false
*/
extern bool BSP_Init_Adc(BSP_ADC_ENUM adc_number, BSP_CH_ENUM ch1_number, BSP_CH_ENUM ch2_number, BSP_CH_ENUM ch3_number, BSP_CH_ENUM ch4_number, BSP_CH_ENUM ch5_number, BSP_CH_ENUM ch6_number, BSP_CH_ENUM ch7_number, BSP_CH_ENUM ch8_number);

/* 操作
  ----------------------------------------------------------------------*/

/* 读AD值
  -----------------------------------------
  入口：ADC编号
  返回值：高电平true，低电平false
*/
extern uint32_t BSP_ReadAdc(BSP_ADC_ENUM adc_number, BSP_CH_ENUM ch_number);

#endif
