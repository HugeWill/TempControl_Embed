/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——程序探针

  -------------------------------------------------------
   文 件 名   : Drv_Probe.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2020年03月13日
   功能描述   : 定义一种程序探针模块。
   使用方法   ：上层模块通过调用Probe_Init获得一个程序探针模块
				的指针，通过调用Probe_On输出高电平，通过调用
				Probe_Off输出低电平。				
   依赖于     : BSP_Gpio V1.0.0.0
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#ifndef _DRV_PROBE_H_
#define _DRV_PROBE_H_ 

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "BSP_Gpio.h"

/*程序探针枚举*/
typedef enum{
	PROBE_1 = 0,
	PROBE_2,
	PROBE_3,
	PROBE_4,
	PROBE_BUTT
} DRV_PROBE_ENUM;

/*程序探针模块状态枚举*/
typedef enum{
	PROBE_UNDEFINED = 0,						/*未初始化*/
	PROBE_ON        = 1,						/*高电平输出*/
	PROBE_OFF       = 2,						/*低电平输出*/
} DRV_PROBE_STATUS_ENUM;

/*程序探针模块结构体*/
typedef struct{
	/*------------------------------变量*/
	DRV_PROBE_STATUS_ENUM status;			/*该程序探针模块的状态*/
	bool pin_buffer;						/*引脚输出缓存*/
	/*------------------------------变常*/
	/*------------------------------常量*/
	DRV_PROBE_ENUM id;						/*程序探针模块id*/
	BSP_PORT_ENUM port_number; 				/*所控制的引脚端口号*/
	BSP_PIN_ENUM pin_number;				/*所控制的引脚编号*/
} DRV_PROBE_TYPE;

/* 接口函数
 ********************************************************/

/* 初始化程序探针模块
  -----------------------------
  入口：程序探针模块的ID号，IO端口
  返回值：初始化好的程序探针模块结构体
*/
extern DRV_PROBE_TYPE* Probe_Init(DRV_PROBE_ENUM id, \
							  BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number);

/* 程序探针输出高电平
  -----------------------------
  入口：程序探针结构体
*/
extern void Probe_On(DRV_PROBE_TYPE* p_unit);

/* 程序探针输出低电平
  -----------------------------
 入口：程序探针结构体
*/
extern void Probe_Off(DRV_PROBE_TYPE* p_unit);

#endif
