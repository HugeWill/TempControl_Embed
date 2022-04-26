/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——继电器模块

  -------------------------------------------------------
   文 件 名   : Drv_Relay.h
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2020年05月25日
   功能描述   : 定义一种继电器控制模块。
   使用方法   ：上层模块通过调用Drv_RelayInit获得一个Relay模块
				的指针，通过调用Drv_RelayON开通继电器，通过
				调用Drv_RelayOFF关断继电器。
   依赖于     : BSP.h
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#ifndef _DRV_RELAY_H_
#define _DRV_RELAY_H_

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "BSP_Gpio.h"

/*驱动类型枚举*/
typedef enum{
	RELAY_POSITIVE_LOGIC = 0,	/*正逻辑控制*/
	RELAY_NEGATIVE_LOGIC = 1,	/*负逻辑控制*/
} DRV_RELAY_LOGIC_TYPE_ENUM;

/*继电器模块枚举*/
typedef enum{
	RELAY_1 = 0,
	RELAY_2,
	RELAY_3,
	RELAY_4,
	RELAY_5,
	RELAY_BUTT
} DRV_RELAY_ENUM;

/*继电器模块状态枚举*/
typedef enum{
	RELAY_UNDEFINED = 0,				/*未初始化*/
	RELAY_ON     = 1,					/*开*/
	RELAY_OFF    = 2,					/*关*/
} DRV_RELAY_STATUS_ENUM;

/*继电器模块结构体*/
typedef struct{
	/*------------------------------变量*/
	DRV_RELAY_STATUS_ENUM status;			/*该继电器模块的状态*/
	bool pin_buffer;						/*引脚输出缓存*/
	/*------------------------------变常*/
	/*------------------------------常量*/
	DRV_RELAY_ENUM id;						/*继电器模块id*/
	DRV_RELAY_LOGIC_TYPE_ENUM logic_type;	/*控制类型*/
	BSP_PORT_ENUM port_number; 				/*所控制的引脚端口号*/
	BSP_PIN_ENUM pin_number;				/*所控制的引脚编号*/
} DRV_RELAY_TYPE;

/* 接口函数
 ********************************************************/

/* 初始化继电器模块
  -----------------------------
  入口：继电器的ID号，正/负逻辑，IO端口
  返回值：初始化好的继电器模块结构体
*/
extern DRV_RELAY_TYPE* Drv_RelayInit(DRV_RELAY_ENUM id, \
								  DRV_RELAY_LOGIC_TYPE_ENUM relay_logic, \
								  BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number);

/* 继电器开
  -----------------------------
  入口：继电器结构体
*/
extern void Drv_RelayON(DRV_RELAY_TYPE* p_unit);

/* 继电器关
  -----------------------------
 入口：继电器结构体
*/
extern void Drv_RelayOFF(DRV_RELAY_TYPE* p_unit);

#endif
