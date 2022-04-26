/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——STM32 BSP层

  -------------------------------------------------------
   文 件 名   : BSP.h
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年07月17日
   功能描述   : 定义一种基于STM32的板级支持包。
				该层依赖于硬件电路。
   依赖于     : STM32F10x_StdPeriph_Lib_V3.5.0
 ********************************************************/

#ifndef _BSP_USART_H_
#define _BSP_USART_H_

#include "stdint.h"
#include "stdbool.h"
#include "stm32f10x.h"
#include "ucos_ii.h"
#include "BSP_Gpio.h"

/*串口枚举*/
typedef enum{
	_USART1_ = 0,
	_USART2_,
	_USART3_,
#ifdef STM32F10X_HD
	_UART4_,
#endif
	_USART_BUTT_
} BSP_USART_ENUM;

/*串口中断函数注册表*/
extern void(* BSP_UartIRQHandler[])(void);
/*串口接收DMA缓存数组*/
extern uint8_t receive_data[100];

/* 初始化
  ----------------------------------------------------------------------*/

/* 初始化CAN及接收中断
  -----------------------------------------
  最多可同时支持4个can id的接收，can_id为0的不初始化
  入口：CAN口编号、波特率（单位K），CAN ID，TX、RX端口
  返回值：成功true，失败false
*/
extern bool BSP_Init_UsartInterrupt(BSP_USART_ENUM usart_number, uint32_t baud_rate, \
							 BSP_PORT_ENUM tx_port_number, BSP_PIN_ENUM tx_pin_number, \
							 BSP_PORT_ENUM rx_port_number, BSP_PIN_ENUM rx_pin_number);

/*初始化DMA，内部函数*/
static void DMA_Buffer_Init(void);
/* 操作
  ----------------------------------------------------------------------*/

/* 串口发送一个字节
  -----------------------------
  入口：串口编号，发送内容
  返回值：成功true，失败false
*/
extern bool BSP_UsartSendByte(BSP_USART_ENUM usart_number, uint8_t data);

/* 串口接收一个字节
-----------------------------
入口：串口编号
返回值：串口接收到的数据
*/
extern uint8_t BSP_UsartGetByte(BSP_USART_ENUM usart_number);

/* 中断
  ----------------------------------------------------------------------*/

/* 串口1中断处理
  -------------------------------
*/
extern void BSP_Usart1_IRQHandler(void);

/* 串口2中断处理
  -------------------------------
*/
extern void BSP_Usart2_IRQHandler(void);

/* 串口3中断处理
  -------------------------------
*/
extern void BSP_Usart3_IRQHandler(void);

/* 串口4中断处理
  -------------------------------
*/
extern void BSP_Uart4_IRQHandler(void);

#endif
