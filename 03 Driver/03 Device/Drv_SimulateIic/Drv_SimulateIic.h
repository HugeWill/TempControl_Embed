/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

			嵌入式开发平台——软件模拟IIC通信模块

  -------------------------------------------------------
   文 件 名   : Drv_SimulateIic.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年10月25日
   功能描述   : 定义一种软件模拟IIC通信模块。
   使用方法   ：上层模块通过调用IicInit获得一个IIC通信接口
				的指针，通过调用IicSend发送，通过调用IicGet
				接收。
   依赖于     : BSP_Gpio V1.0.0.0
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/
 
#ifndef _DRV_SIMULATEIIC_H_
#define _DRV_SIMULATEIIC_H_ 

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "BSP_Gpio.h"

#define LOW			0
#define HIGH		1
#define UCERRTIME	250	/*超时时间*/

/*软件模拟IIC接口模块枚举*/
typedef enum{
	SIMULATE_IIC_1 = 0,
	SIMULATE_IIC_2,
	SIMULATE_IIC_3,
	SIMULATE_IIC_4,
	SIMULATE_IIC_BUTT
} DRV_SIMULATE_IIC_ENUM;

/*软件模拟IIC接口模块状态枚举*/
typedef enum{
	SIMULATE_IIC_UNDEFINED = 0,						/*未初始化*/
	SIMULATE_IIC_BUSY      = 1,						/*繁忙*/
	SIMULATE_IIC_READY     = 2,						/*准备好*/
} DRV_SIMULATE_IIC_STATUS_ENUM;

/*软件模拟IIC接口模块结构体*/
typedef struct{
	/*------------------------------变量*/
	DRV_SIMULATE_IIC_STATUS_ENUM status;	/*该软件模拟IIC接口模块的状态*/
	bool sda_pin_set_buffer;				/*数据引脚输出缓存*/
	bool sda_pin_read_buffer;				/*数据引脚输入缓存*/
	bool scl_pin_buffer;					/*时钟引脚缓存*/
	/*------------------------------变常*/
	/*------------------------------常量*/
	DRV_SIMULATE_IIC_ENUM id;				/*软件模拟IIC接口模块id*/
	uint32_t theta;							/*时钟速度参数*/
	BSP_PORT_ENUM sda_port_number; 			/*sda引脚端口号*/
	BSP_PIN_ENUM sda_pin_number;			/*sda引脚编号*/
	BSP_PORT_ENUM scl_port_number; 			/*scl引脚端口号*/
	BSP_PIN_ENUM scl_pin_number;			/*scl引脚编号*/
} DRV_SIMULATE_IIC_TYPE;

/* 接口函数
 ********************************************************/

/* 初始化软件模拟IIC接口模块
  -----------------------------
  入口：软件模拟IIC接口模块的ID号，IO端口
  返回值：初始化好的软件模拟IIC接口模块结构体
*/
extern DRV_SIMULATE_IIC_TYPE* SimulateIicInit(DRV_SIMULATE_IIC_ENUM id, \
												uint32_t baud_rate, \
												BSP_PORT_ENUM sda_port_number, BSP_PIN_ENUM sda_pin_number, \
												BSP_PORT_ENUM scl_port_number, BSP_PIN_ENUM scl_pin_number);

/*
  发送一个字节
  --------------------------------
*/
extern bool Drv_SimulateIicSendByte(DRV_SIMULATE_IIC_TYPE* p_unit, unsigned char txd);

/*
  读取一个字节
  --------------------------------
*/
extern unsigned char Drv_SimulateIicReadByte(DRV_SIMULATE_IIC_TYPE* p_unit);

/*
  发送起始信号
  --------------------------------
*/
extern bool Drv_SimulateIicStart(DRV_SIMULATE_IIC_TYPE* p_unit);
 
/*
  发送停止信号
  --------------------------------
*/
extern bool Drv_SimulateIicStop(DRV_SIMULATE_IIC_TYPE* p_unit);

/*
  等待ACK信号
  --------------------------------
*/
extern bool Drv_SimulateIicWaitAck(DRV_SIMULATE_IIC_TYPE* p_unit);

/*
  发送ACK信号
  --------------------------------
*/
extern bool Drv_SimulateIicAck(DRV_SIMULATE_IIC_TYPE* p_unit);

/*
  发送NACK信号
  --------------------------------
*/
extern bool Drv_SimulateIicNack(DRV_SIMULATE_IIC_TYPE* p_unit);

/*
  延时n个us
  --------------------------------
*/
extern bool Drv_SimulateIicDelayUs(DRV_SIMULATE_IIC_TYPE* p_unit, unsigned short time);

/* 内部函数
 ********************************************************/
  
/*
  SCL状态设置
  --------------------------------
  入口：IIC模块结构体，SCL引脚状态
  返回：true 执行成功，false 执行失败
*/
static bool _Drv_SimulateIicSetScl(DRV_SIMULATE_IIC_TYPE* p_unit, uint8_t state);

/*
  SDA状态设置
  --------------------------------
  入口：IIC模块结构体，SDA引脚状态
  返回：true 执行成功，false 执行失败
*/
static bool _Drv_SimulateIicSetSda(DRV_SIMULATE_IIC_TYPE* p_unit, uint8_t state);

/*
  SDA状态读取
  --------------------------------
  入口：IIC模块结构体
  返回：SDA引脚状态, HIGH或者LOW
*/
static uint8_t _Drv_SimulateIicReadSda(DRV_SIMULATE_IIC_TYPE* p_unit);

#endif
