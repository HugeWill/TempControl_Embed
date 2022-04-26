/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——电磁阀模块

  -------------------------------------------------------
   文 件 名   : Drv_Valve.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年06月28日
   功能描述   : 定义一种电磁阀控制模块，仅支持两个电磁阀的
				开关控制。
   使用方法   ：上层模块通过调用Valve_Init获得一个Valve模块
				的指针，通过调用Valve_Open打开电磁阀，通过
				调用Valve_Close关闭电磁阀。
   依赖于     : BSP.h
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/
 
#ifndef _DRV_VALVE_H_
#define _DRV_VALVE_H_

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "BSP_Gpio.h"

/*电磁阀类型枚举*/
typedef enum{
	VALVE_NORMAL_CLOSE = 0,		/*常闭阀*/
	VALVE_NORMAL_OPEN = 1,		/*常开阀*/
} DRV_VALVE_TYPE_ENUM;

/*驱动类型枚举*/
typedef enum{
	VALVE_POSITIVE_LOGIC = 0,	/*正逻辑控制*/
	VALVE_NEGATIVE_LOGIC = 1,	/*负逻辑控制*/
} DRV_VALVE_LOGIC_TYPE_ENUM;

/*电磁阀模块枚举*/
typedef enum{
	VALVE_1 = 0,
	VALVE_2,
	VALVE_3,
	VALVE_4,
	VALVE_5,
	VALVE_6,
	VALVE_7,
	VALVE_8,
	VALVE_9,
	VALVE_10,
	VALVE_11,
	VALVE_12,
	VALVE_13,
	VALVE_14,
	VALVE_15,
	VALVE_16,
	VALVE_17,
	VALVE_18,
	VALVE_19,
	VALVE_20,
	VALVE_21,
	VALVE_22,
	VALVE_23,
	VALVE_24,
	VALVE_25,
	VALVE_26,
	VALVE_27,
	VALVE_28,
	VALVE_29,
	VALVE_30,
	VALVE_31,
	VALVE_32,
	VALVE_33,
	VALVE_34,
	VALVE_35,
	VALVE_BUTT
} DRV_VALVE_ENUM;

/*电磁阀模块状态枚举*/
typedef enum{
	VALVE_UNDEFINED = 0,				/*未初始化*/
	VALVE_OPEN     = 1,					/*全开*/
	VALVE_CLOSE    = 2,					/*全闭*/
} DRV_VALVE_STATUS_ENUM;

/*电磁阀模块结构体*/
typedef struct{
	/*------------------------------变量*/
	DRV_VALVE_STATUS_ENUM status;			/*该电磁阀模块的状态*/
	bool pin_buffer;						/*引脚输出缓存*/
	/*------------------------------变常*/
	/*------------------------------常量*/
	DRV_VALVE_ENUM id;						/*电磁阀模块id*/
	DRV_VALVE_TYPE_ENUM type;				/*电磁阀类型*/
	DRV_VALVE_LOGIC_TYPE_ENUM logic_type;	/*控制类型*/
	BSP_PORT_ENUM port_number; 				/*所控制的引脚端口号*/
	BSP_PIN_ENUM pin_number;				/*所控制的引脚编号*/
} DRV_VALVE_TYPE;

/* 接口函数
 ********************************************************/

/* 初始化电磁阀模块
  -----------------------------
  入口：电磁阀的ID号，常开/常闭，正/负逻辑，IO端口
  返回值：初始化好的电磁阀模块结构体
*/
extern DRV_VALVE_TYPE* Valve_Init(DRV_VALVE_ENUM id, \
								  DRV_VALVE_TYPE_ENUM valve_type, \
								  DRV_VALVE_LOGIC_TYPE_ENUM valve_logic, \
								  BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number);

/* 打开电磁阀
  -----------------------------
  入口：电磁阀结构体
*/
extern void Valve_Open(DRV_VALVE_TYPE* p_unit);

/* 关闭电磁阀
  -----------------------------
 入口：电磁阀结构体
*/
extern void Valve_Close(DRV_VALVE_TYPE* p_unit);

/* 判断电磁阀是否开启
  -----------------------------
  入口：电磁阀结构体
  返回值：如果电磁阀开启则返回true
*/
extern bool Valve_Is_Open(DRV_VALVE_TYPE* p_unit);

#endif
