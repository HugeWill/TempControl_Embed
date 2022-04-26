/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

			嵌入式开发平台——软件模拟SPI接口模块

  -------------------------------------------------------
   文 件 名   : Drv_SimulateSpi.h
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年10月14日
   功能描述   : 定义一种软件模拟SPI通信模块。
   使用方法   ：上层模块通过调用SPI_Init获得一个SPI通信接口
				的指针，通过调用Spi_Send发送，通过调用Spi_Get
				接收。
   依赖于     : BSP_Gpio V1.0.0.0
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/
 
#ifndef _DRV_SIMULATESPI_H_
#define _DRV_SIMULATESPI_H_ 

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "BSP_Gpio.h"

/*软件模拟SPI接口模块枚举*/
typedef enum{
	SOFTWARE_SPI_1 = 0,
	SOFTWARE_SPI_2,
	SOFTWARE_SPI_3,
	SOFTWARE_SPI_4,
	SOFTWARE_SPI_5,
	SOFTWARE_SPI_6,
	SOFTWARE_SPI_7,
	SOFTWARE_SPI_8,
	SOFTWARE_SPI_9,
	SOFTWARE_SPI_10,
	SOFTWARE_SPI_11,
	SOFTWARE_SPI_12,
	SOFTWARE_SPI_13,
	SOFTWARE_SPI_14,
	SOFTWARE_SPI_15,
	SOFTWARE_SPI_16,
	SOFTWARE_SPI_17,
	SOFTWARE_SPI_18,
	SOFTWARE_SPI_19,
	SOFTWARE_SPI_20,
	SOFTWARE_SPI_21,
	SOFTWARE_SPI_22,
	SOFTWARE_SPI_23,
	SOFTWARE_SPI_24,
	SOFTWARE_SPI_25,
	SOFTWARE_SPI_26,
	SOFTWARE_SPI_27,
	SOFTWARE_SPI_28,
	SOFTWARE_SPI_BUTT
} DRV_SOFTWARE_SPI_ENUM;

/*软件模拟SPI接口模块状态枚举*/
typedef enum{
	SOFTWARE_SPI_UNDEFINED = 0,						/*未初始化*/
	SOFTWARE_SPI_BUSY      = 1,						/*繁忙*/
	SOFTWARE_SPI_READY     = 2,						/*准备好*/
} DRV_SOFTWARE_SPI_STATUS_ENUM;

/*软件模拟SPI接口模块结构体*/
typedef struct{
	/*------------------------------变量*/
	DRV_SOFTWARE_SPI_STATUS_ENUM status;	/*该软件模拟SPI接口模块的状态*/
	bool sdi_pin_buffer;					/*数据输入引脚缓存*/
	bool sdo_pin_buffer;					/*数据输出引脚缓存*/
	bool sck_pin_buffer;					/*时钟输出引脚缓存*/
	bool cs_pin_buffer;						/*使能输出引脚缓存*/
	/*------------------------------变常*/
	/*------------------------------常量*/
	DRV_SOFTWARE_SPI_ENUM id;				/*软件模拟SPI接口模块id*/
	uint32_t theta;							/*时钟速度参数*/
	BSP_PORT_ENUM sdi_port_number; 			/*sdi引脚端口号*/
	BSP_PIN_ENUM sdi_pin_number;			/*sdi引脚编号*/
	BSP_PORT_ENUM sdo_port_number; 			/*sdo引脚端口号*/
	BSP_PIN_ENUM sdo_pin_number;			/*sdo引脚编号*/
	BSP_PORT_ENUM sck_port_number; 			/*sck引脚端口号*/
	BSP_PIN_ENUM sck_pin_number;			/*sck引脚编号*/
	BSP_PORT_ENUM cs_port_number; 			/*cs引脚端口号*/
	BSP_PIN_ENUM cs_pin_number;				/*cs引脚编号*/
} DRV_SOFTWARE_SPI_TYPE;

/* 接口函数
 ********************************************************/

/* 初始化软件模拟SPI接口模块
  -----------------------------
  入口：软件模拟SPI接口模块的ID号，IO端口
  返回值：初始化好的软件模拟SPI接口模块结构体
*/
extern DRV_SOFTWARE_SPI_TYPE* SoftwareSpiInit(DRV_SOFTWARE_SPI_ENUM id, \
												uint8_t theta, \
												BSP_PORT_ENUM sdi_port_number, BSP_PIN_ENUM sdi_pin_number, \
												BSP_PORT_ENUM sdo_port_number, BSP_PIN_ENUM sdo_pin_number, \
												BSP_PORT_ENUM sck_port_number, BSP_PIN_ENUM sck_pin_number, \
												BSP_PORT_ENUM cs_port_number, BSP_PIN_ENUM cs_pin_number);

/* 软件模拟SPI接口模块写一个字节
  -----------------------------
  入口：软件模拟SPI接口模块的ID号，数据
  返回值：true 成功，false 失败
*/
extern bool SoftwareSpiWriteByte(DRV_SOFTWARE_SPI_TYPE* p_unit, uint8_t data);

/* 软件模拟SPI接口模块读一个字节
  -----------------------------
  入口：软件模拟SPI接口模块的ID号
  返回值：数据
*/
extern uint8_t SoftwareSpiReadByte(DRV_SOFTWARE_SPI_TYPE* p_unit);

/* 软件模拟SPI接口模块延时n微秒
  -----------------------------
  入口：延时时间
  返回值：true 成功，false 失败
*/
extern bool _SoftwareSpiDelayUs(DRV_SOFTWARE_SPI_TYPE* p_unit, uint8_t time);

#endif
