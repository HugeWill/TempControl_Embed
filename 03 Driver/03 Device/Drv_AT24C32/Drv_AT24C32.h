/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

			嵌入式开发平台——AT24C32 EEP模块

  -------------------------------------------------------
   文 件 名   : Drv_AT24C32.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2020年03月02日
   功能描述   : 定义AT24C32 EEP的驱动。
   使用方法   ：上层模块通过调用At24c32Init获得一个AT24C32
				的指针，通过调用At24c32Read读取数据，通过调用
				At24c32Write写入数据。
   依赖于     : Drv_SimulateIic V1.0.0.0
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/
 
#ifndef _DRV_AT24C32_H_
#define _DRV_AT24C32_H_ 

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "Drv_SimulateIic.h"

/*AT24C32模块枚举*/
typedef enum{
	AT24C32_1 = 0,
	AT24C32_BUTT
} DRV_AT24C32_ENUM;

/*AT24C32模块状态枚举*/
typedef enum{
	AT24C32_UNDEFINED = 0,				/*未初始化*/
	AT24C32_READY  = 1					/*可用*/
} DRV_AT24C32_STATUS_ENUM;

/*AT24C32模块结构体*/
typedef struct{
	/*------------------------------变量*/
	DRV_AT24C32_STATUS_ENUM status;			/*该AT24C32模块的状态*/
	uint8_t read_buffer;					/*读数据缓存*/
	uint8_t write_buffer;					/*写数据缓存*/
	/*------------------------------变常*/
	/*------------------------------常量*/
	DRV_AT24C32_ENUM id;					/*AT24C32模块id*/
	DRV_SIMULATE_IIC_TYPE* iic_port;		/*IIC端口*/
	
} DRV_AT24C32_TYPE;

/* 接口函数
 ********************************************************/

/* 初始化AT24C32模块
  -----------------------------
  入口：AT24C32模块的ID号，引脚端口
  返回值：初始化好的AT24C32模块结构体
*/
extern DRV_AT24C32_TYPE* At24c32Init(DRV_AT24C32_ENUM id, \
											BSP_PORT_ENUM scl_port_number, BSP_PIN_ENUM scl_pin_number, \
											BSP_PORT_ENUM sda_port_number, BSP_PIN_ENUM sda_pin_number);

/* AT24C32模块读一个字节
  -----------------------------
  入口：AT24C32模块的ID号，地址
  返回值：数据
*/
extern uint8_t At24c32ReadByte(DRV_AT24C32_TYPE* p_unit, uint16_t address);

/* AT24C32模块读一个双字
  -----------------------------
  入口：AT24C32模块的ID号，地址
  返回值：数据
*/
extern uint16_t At24c32ReadTwoByte(DRV_AT24C32_TYPE* p_unit, uint16_t address);

/* AT24C32模块写一个字节
  -----------------------------
  入口：AT24C32模块的ID号，地址，数据
  返回值：执行状态，成功true，失败false
*/
extern bool At24c32WriteByte(DRV_AT24C32_TYPE* p_unit, uint16_t address, uint8_t data);

/* AT24C32模块写两个字节
  -----------------------------
  入口：AT24C32模块的ID号，地址，数据
  返回值：执行状态，成功true，失败false
*/
extern bool At24c32WriteTwoByte(DRV_AT24C32_TYPE* p_unit, uint16_t address, uint16_t data);

/* AT24C32模块写一页（32Byte）
  -----------------------------
  入口：AT24C32模块的ID号，地址，数据指针
  返回值：执行状态，成功true，失败false
*/
extern bool At24c32WritePage(DRV_AT24C32_TYPE* p_unit, uint16_t address, uint8_t* datas);

/* AT24C32模块读一页（32Byte）
  -----------------------------
  入口：AT24C32模块的ID号，页地址，数据指针
  返回值：执行状态，成功true，失败false
*/
extern bool At24c32ReadPage(DRV_AT24C32_TYPE* p_unit, uint16_t page_address, uint8_t* datas);

#endif
