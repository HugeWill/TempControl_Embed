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

#ifndef _BSP_CAN_H_
#define _BSP_CAN_H_

#include "stdint.h"
#include "stdbool.h"
#include "stm32f10x.h"
#include "ucos_ii.h"
#include "BSP_Gpio.h"

/*CAN口枚举*/
typedef enum{
	_CAN1_ = 0,
	_CAN_BUTT_
} BSP_CAN_ENUM;

/*CAN接收中断函数注册表*/
extern void(* BSP_CanRxIRQHandler)(uint32_t* p_std_id,	/*标准ID*/
							uint32_t* p_ext_id,	/*扩展ID*/
							uint8_t*  p_ide,	/*ID类型*/
							uint8_t*  p_rtr,	/*帧类型*/
							uint8_t*  p_dlc,	/*帧长度*/
							uint8_t*  p_data,	/*接收的数据，8个字节*/
							uint8_t*  p_fmi);	/*CAN接收中断中断函数指针列表*/

/* 初始化
  ----------------------------------------------------------------------*/

/* 初始化CAN及接收中断
  -----------------------------------------
  最多可同时支持4个can id的接收，can_id为0的不初始化
  入口：CAN口编号、波特率（单位K），CAN ID，TX、RX端口
  返回值：成功true，失败false
*/
extern bool BSP_Init_CANinterrupt(BSP_CAN_ENUM can_number, uint32_t baud_rate, \
						   uint8_t can_id1, uint8_t can_id2, uint8_t can_id3, uint8_t can_id4, \
						   BSP_PORT_ENUM tx_port_number, BSP_PIN_ENUM tx_pin_number, \
						   BSP_PORT_ENUM rx_port_number, BSP_PIN_ENUM rx_pin_number);

/* 操作
  ----------------------------------------------------------------------*/

/* CAN TX 发送数据帧
  -----------------------------------------
  入口：CAN ID, 待发送的数据，帧长度
  返回值：成功true，失败false
*/
extern bool BSP_CAN1_TX(uint8_t can_id, uint8_t* data, uint8_t dlc);

/* CAN TX 接收中断使能
  -----------------------------------------
*/
extern void BSP_CAN1_IT_ENABLE(void);

/* CAN TX 接收中断不使能
  -----------------------------------------
*/
extern void BSP_CAN1_IT_DISENABLE(void);

/* 中断
  ----------------------------------------------------------------------*/

/* CAN RX0接收中断外部处理函数
  -----------------------------------------
  该函数在上层模块实现
*/
extern void BSP_CAN1_RX_IRQHandler(uint32_t* p_std_id,	/*标准ID*/
									uint32_t* p_ext_id,	/*扩展ID*/
									uint8_t*  p_ide,	/*ID类型*/
									uint8_t*  p_rtr,	/*帧类型*/
									uint8_t*  p_dlc,	/*帧长度*/
									uint8_t*  p_data,	/*接收的数据，8个字节*/
									uint8_t*  p_fmi);

#endif
