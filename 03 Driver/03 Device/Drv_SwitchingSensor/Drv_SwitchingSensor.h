/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

			嵌入式开发平台——开关信号模块

  -------------------------------------------------------
   文 件 名   : Drv_SwitchingSensor.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年10月16日
   功能描述   : 定义开关信号的驱动。
   使用方法   ：上层模块通过调用SwitchingSensorInit获得一个
				SwitchingSensor的指针。
   依赖于     : BSP_Gpio V1.0.0.0
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/
 
#ifndef _DRV_SWITCHINGSENSOR_H_
#define _DRV_SWITCHINGSENSOR_H_ 

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "ucos_ii.h"
#include "BSP_Gpio.h"

/*开关信号模块枚举*/
typedef enum{
	SWITCHINGSENSOR_1 = 0,
	SWITCHINGSENSOR_2,
	SWITCHINGSENSOR_3,
	SWITCHINGSENSOR_4,
	SWITCHINGSENSOR_5,
	SWITCHINGSENSOR_6,
	SWITCHINGSENSOR_7,
	SWITCHINGSENSOR_8,
	SWITCHINGSENSOR_9,
	SWITCHINGSENSOR_10,
	SWITCHINGSENSOR_BUTT
} DRV_SWITCHINGSENSOR_ENUM;

/*开关信号模块响应类型枚举*/
typedef enum{
	RISING = 0,	/*上升沿响应*/
	FALLING		/*下降沿响应*/
} DRV_SWITCHINGSENSOR_RESPONSE_TYPE_ENUM;

/*开关信号模块信号量状态枚举*/
typedef enum{
	DIS_SEM = 0,	/*不使能*/
	EN_SEM			/*使能*/
} DRV_SWITCHINGSENSOR_SEM_STATUS_ENUM;

/*开关信号模块状态枚举*/
typedef enum{
	SWITCHINGSENSOR_UNDEFINED = 0,				/*未初始化*/
	SWITCHINGSENSOR_READY     = 1				/*设置好*/
} DRV_SWITCHINGSENSOR_STATUS_ENUM;

/*开关信号模块结构体*/
typedef struct{
	/*------------------------------变量*/
	DRV_SWITCHINGSENSOR_STATUS_ENUM status;		/*该开关信号模块的状态*/
	uint8_t buffer;								/*IO口状态*/
	uint8_t buffer_check;						/*IO口状态待确认*/
	uint16_t delay_count;						/*消抖计时，单位ms*/
	uint8_t delay_flag;							/*消抖计时标志，1有效*/
	DRV_SWITCHINGSENSOR_SEM_STATUS_ENUM sem_switch;	/*信号量开关*/
	/*------------------------------变常*/
	/*------------------------------常量*/
	DRV_SWITCHINGSENSOR_ENUM id;				/*开关信号模块id*/
	DRV_SWITCHINGSENSOR_RESPONSE_TYPE_ENUM response_type;	/*响应类型*/
	uint16_t delay_time;						/*延时消抖时间，单位ms*/
	OS_EVENT* sem;								/*光耦信号量*/
	BSP_PORT_ENUM port_number; 					/*对应的引脚端口号*/
	BSP_PIN_ENUM pin_number;					/*对应的引脚编号*/
} DRV_SWITCHINGSENSOR_TYPE;

/* 接口函数
 ********************************************************/

/* 初始化开关信号模块
  -----------------------------
  入口：开关信号模块的ID号，延迟消抖时间
  返回值：初始化好的M开关信号模块结构体
*/
extern DRV_SWITCHINGSENSOR_TYPE* Drv_SwitchingSensorInit(DRV_SWITCHINGSENSOR_ENUM id, \
														DRV_SWITCHINGSENSOR_RESPONSE_TYPE_ENUM response_type, \
														uint16_t delay_time, \
														BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number);

/* 信号量设置
  -----------------------------
  入口：开关信号模块结构体，信号量开关，信号量响应类型
  返回值：true 成功，false 失败
*/
extern bool Drv_SwitchingSensorSemSet(DRV_SWITCHINGSENSOR_TYPE* p_unit, \
								DRV_SWITCHINGSENSOR_SEM_STATUS_ENUM sem_switch, \
								DRV_SWITCHINGSENSOR_RESPONSE_TYPE_ENUM response_type);

/* 读取信号状态
  -----------------------------
  入口：开关信号模块结构体
  返回值：信号状态
*/
extern uint8_t Drv_SwitchingSensorRead(DRV_SWITCHINGSENSOR_TYPE* p_unit);

/* 常驻内存函数
 ********************************************************/

/* 所有信号消抖函数
  -----------------------------
  入口：
  返回值：
*/
extern void Drv_AllSwitchingSensorDebounce_Loop(void);

#endif
