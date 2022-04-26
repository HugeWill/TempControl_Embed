/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

			嵌入式开发平台——BSP层 SPI接口封装

  -------------------------------------------------------
   文 件 名   : BSP_Spi.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年10月15日
   功能描述   : 定义一种基于STM32的板级支持包。
				该层依赖于硬件电路。
   依赖于     : STM32F10x_StdPeriph_Lib_V3.5.0
 ********************************************************/

#ifndef _BSP_SPI_H_
#define _BSP_SPI_H_

#include "stdint.h"
#include "stdbool.h"
#include "stm32f10x.h"
#include "BSP_Gpio.h"

/*SPI口枚举*/
typedef enum{
	_SPI1_ = 0,
	_SPI2_,
	_SPI3_,
	_SPI_BUTT_
} BSP_SPI_ENUM;

/*SPI口NSS状态枚举*/
typedef enum{
	NSS_LOW = 0,
	NSS_HIGH
} BSP_SPI_NSS_STATUS_ENUM;

/*SPI口状态类型枚举*/
typedef enum{
	TXE_STATUS = 0,
	RXNE_STATUS
} BSP_SPI_STATUS_TYPE_ENUM;

/* 初始化
  ----------------------------------------------------------------------*/

/* 初始化SPI接口
  -----------------------------------------
  入口：SPI模块编号，引脚端口
  返回值：成功true，失败false
*/
extern bool BSP_InitSpi(BSP_SPI_ENUM spi_number, \
						BSP_PORT_ENUM nss_port_number, BSP_PIN_ENUM nss_pin_number, \
						BSP_PORT_ENUM sck_port_number, BSP_PIN_ENUM sck_pin_number, \
						BSP_PORT_ENUM miso_port_number, BSP_PIN_ENUM miso_pin_number, \
						BSP_PORT_ENUM mosi_port_number, BSP_PIN_ENUM mosi_pin_number);

/* 操作
  ----------------------------------------------------------------------*/

/* SPI接口NSS独立控制
  -----------------------------------------
  入口：SPI模块编号，NSS状态
  返回值：成功true，失败false
*/
extern bool BSP_SpiNssSet(BSP_PORT_ENUM nss_port_number, BSP_PIN_ENUM nss_pin_number, \
							BSP_SPI_NSS_STATUS_ENUM status);

/* SPI接口发送数据
  -----------------------------------------
  入口：SPI模块编号，数据值
  返回值：成功true，失败false
*/
extern bool BSP_SpiSendData(BSP_SPI_ENUM spi_number, uint16_t data);

/* SPI接口接收数据
  -----------------------------------------
  入口：SPI模块编号
  返回值：接收到的数据
*/
extern uint16_t BSP_SpiGetData(BSP_SPI_ENUM spi_number);

/* SPI接口状态读取
  -----------------------------------------
  入口：SPI模块编号，要读取的状态类型
  返回值：true 完成，false 未完成
*/
extern bool BSP_SpiGetStatus(BSP_SPI_ENUM spi_number, BSP_SPI_STATUS_TYPE_ENUM type);

#endif
