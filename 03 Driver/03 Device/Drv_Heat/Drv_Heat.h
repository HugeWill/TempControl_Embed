/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——加热膜模块

  -------------------------------------------------------
   文 件 名   : Drv_Heat.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2020年2月14日
   功能描述   : 定义一种加热膜控制模块。
   使用方法   ：上层模块通过调用Drv_HeatInit获得一个Heat模块
				的指针，通过调用Drv_HeatOpen开启加热膜，通过
				调用Drv_HeatClose关闭加热膜。
   依赖于     : BSP_Gpio V1.0.0.0
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#ifndef _DRV_HEAT_H_
#define _DRV_HEAT_H_

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "BSP_Gpio.h"

/*加热膜模块枚举*/
typedef enum{
	HEAT_1 = 0,
	HEAT_2,
	HEAT_3,
	HEAT_4,
	HEAT_5,
	HEAT_6,
	HEAT_7,
	HEAT_8,
	HEAT_BUTT
} DRV_HEAT_ENUM;

/*加热膜模块状态枚举*/
typedef enum{
	HEAT_UNDEFINED = 0,				/*未初始化*/
	HEAT_OPEN     = 1,					/*全开*/
	HEAT_CLOSE    = 2,					/*全闭*/
} DRV_HEAT_STATUS_ENUM;

/*加热膜模块结构体*/
typedef struct{
	/*------------------------------变量*/
	DRV_HEAT_STATUS_ENUM status;			/*该加热膜模块的状态*/
	bool pin_buffer;						/*引脚输出缓存*/
	/*------------------------------变常*/
	/*------------------------------常量*/
	DRV_HEAT_ENUM id;						/*加热膜模块id*/
	BSP_PORT_ENUM port_number; 				/*所控制的引脚端口号*/
	BSP_PIN_ENUM pin_number;				/*所控制的引脚编号*/
} DRV_HEAT_TYPE;

/* 接口函数
 ********************************************************/

/* 初始化加热膜模块
  -----------------------------
  入口：加热膜的ID号，IO端口
  返回值：初始化好的加热膜模块结构体
*/
extern DRV_HEAT_TYPE* Drv_HeatInit(DRV_HEAT_ENUM id, \
								  BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number);

/* 打开加热膜
  -----------------------------
  入口：加热膜结构体
*/
extern void Drv_HeatOpen(DRV_HEAT_TYPE* p_unit);

/* 关闭加热膜
  -----------------------------
 入口：加热膜结构体
*/
extern void Drv_HeatClose(DRV_HEAT_TYPE* p_unit);

/* 判断加热膜是否开启
  -----------------------------
  入口：加热膜结构体
  返回值：如果加热膜开启则返回true
*/
extern bool Drv_HeatIsOpen(DRV_HEAT_TYPE* p_unit);

#endif
