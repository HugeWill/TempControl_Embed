/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——BSP层 GPIO封装

  -------------------------------------------------------
   文 件 名   : BSP_Gpio.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年07月17日
   功能描述   : 定义一种基于STM32的板级支持包。
				该层依赖于硬件电路。
   依赖于     : STM32F10x_StdPeriph_Lib_V3.5.0
 ********************************************************/

#ifndef _BSP_GPIO_H_
#define _BSP_GPIO_H_

#include "stdint.h"
#include "stdbool.h"
#include "stm32f10x.h"

/*端口及引脚枚举转换为端口及引脚结构体指针*/
#define PORT(n)			(GPIO_TypeDef *)(GPIOA_BASE + 0x0400 * n)
#define PIN(n)			(GPIO_Pin_0 << n)

/*IO端口枚举*/
typedef enum{
	_PA_ = 0,
	_PB_,
	_PC_,
	_PD_,
	_PE_,
	_PF_,
	_PG_,
	_PORT_BUTT_
} BSP_PORT_ENUM;
#define _PORT_NC_ _PORT_BUTT_

/*IO引脚枚举*/
typedef enum{
	_P0_ = 0,
	_P1_,
	_P2_,
	_P3_,
	_P4_,
	_P5_,
	_P6_,
	_P7_,
	_P8_,
	_P9_,
	_P10_,
	_P11_,
	_P12_,
	_P13_,
	_P14_,
	_P15_,
	_PIN_BUTT_
} BSP_PIN_ENUM;
#define _PIN_NC_ _PIN_BUTT_

/*IO口工作模式枚举*/
typedef enum{
	_IN_FLOATING_ = 0,	/*浮空输入*/
	_OUT_PP_,			/*推挽输出*/
	_OUT_OD_,			/*开漏输出*/
	_IO_TYPE_BUTT_
} BSP_IO_TYPE_ENUM;


/*外部中断函数注册表*/
extern void(* BSP_ExtiIRQHandler[])(void);

/* 初始化
  ----------------------------------------------------------------------*/

/* 初始化引脚
  -----------------------------------------
  入口：引脚端口、引脚编号、输入/推挽/开漏
  返回值：成功true，失败false
*/
extern bool BSP_Init_Pin(BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number, BSP_IO_TYPE_ENUM type);

/* 初始化引脚中断
  -----------------------------------------
  入口：引脚端口、引脚编号
  返回值：成功true，失败false
*/
extern bool BSP_Init_PinInterrupt(BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number);

/* 操作
  ----------------------------------------------------------------------*/

/* 读引脚状态
  -----------------------------------------
  入口：引脚端口、引脚编号
  返回值：高电平true，低电平false
*/
extern bool BSP_ReadPin(BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number);

/* 写引脚状态
  -----------------------------------------
  入口：引脚端口、引脚编号
*/
extern void BSP_WritePin(BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number, bool status);

#endif
