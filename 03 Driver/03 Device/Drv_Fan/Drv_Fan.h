/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——风扇模块

  -------------------------------------------------------
   文 件 名   : Drv_Fan.h
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2020年05月25日
   功能描述   : 定义一种风扇控制模块。
   使用方法   ：上层模块通过调用Drv_FanInit获得一个Fan模块
				的指针，通过调用Drv_FanON开启风扇，通过
				调用Drv_FanOFF关闭风扇。
   依赖于     : BSP.h
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#ifndef _DRV_FAN_H_
#define _DRV_FAN_H_

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "BSP_Gpio.h"

/*驱动类型枚举*/
typedef enum{
	FAN_POSITIVE_LOGIC = 0,	/*正逻辑控制*/
	FAN_NEGATIVE_LOGIC = 1,	/*负逻辑控制*/
} DRV_FAN_LOGIC_TYPE_ENUM;

/*风扇模块枚举*/
typedef enum{
	FAN_1 = 0,
	FAN_2,
	FAN_3,
	FAN_4,
	FAN_5,
	FAN_6,
	FAN_7,
	FAN_BUTT
} DRV_FAN_ENUM;

/*风扇模块状态枚举*/
typedef enum{
	FAN_UNDEFINED = 0,				/*未初始化*/
	FAN_ON     = 1,					/*开*/
	FAN_OFF    = 2,					/*关*/
} DRV_FAN_STATUS_ENUM;

/*风扇模块结构体*/
typedef struct{
	/*------------------------------变量*/
	DRV_FAN_STATUS_ENUM status;				/*该风扇模块的状态*/
	bool pin_buffer;						/*引脚输出缓存*/
	/*------------------------------变常*/
	/*------------------------------常量*/
	DRV_FAN_ENUM id;						/*风扇模块id*/
	DRV_FAN_LOGIC_TYPE_ENUM logic_type;		/*控制类型*/
	BSP_PORT_ENUM port_number; 				/*所控制的引脚端口号*/
	BSP_PIN_ENUM pin_number;				/*所控制的引脚编号*/
} DRV_FAN_TYPE;

/* 接口函数
 ********************************************************/

/* 初始化风扇模块
  -----------------------------
  入口：风扇的ID号，正/负逻辑，IO端口
  返回值：初始化好的风扇模块结构体
*/
extern DRV_FAN_TYPE* Drv_FanInit(DRV_FAN_ENUM id, \
								  DRV_FAN_LOGIC_TYPE_ENUM logic_type, \
								  BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number);

/* 风扇开
  -----------------------------
  入口：风扇结构体
*/
extern void Drv_FanON(DRV_FAN_TYPE* p_unit);

/* 风扇关
  -----------------------------
 入口：风扇结构体
*/
extern void Drv_FanOFF(DRV_FAN_TYPE* p_unit);

#endif
