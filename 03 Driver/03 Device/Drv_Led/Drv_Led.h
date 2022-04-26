/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——LED模块

  -------------------------------------------------------
   文 件 名   : Drv_Led.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年07月19日
   功能描述   : 定义一种LED模块，仅支持LED的亮灭控制。
   使用方法   ：上层模块通过调用Led_Init获得一个Led模块
				的指针，通过调用Led_On点亮，通过调用Led_Off
				熄灭。
   依赖于     : BSP.h
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/
 
#ifndef _DRV_LED_H_
#define _DRV_LED_H_ 

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "BSP_Gpio.h"

/*驱动类型枚举*/
typedef enum{
	LED_POSITIVE_LOGIC = 0,					/*正逻辑控制*/
	LED_NEGATIVE_LOGIC = 1,					/*负逻辑控制*/
} DRV_LED_LOGIC_TYPE_ENUM;

/*LED模块枚举*/
typedef enum{
	LED_1 = 0,
	LED_2,
	LED_3,
	LED_4,
	LED_BUTT
} DRV_LED_ENUM;

/*LED模块状态枚举*/
typedef enum{
	LED_UNDEFINED = 0,						/*未初始化*/
	LED_ON        = 1,						/*全亮*/
	LED_OFF       = 2,						/*全灭*/
} DRV_LED_STATUS_ENUM;

/*LED模块结构体*/
typedef struct{
	/*------------------------------变量*/
	DRV_LED_STATUS_ENUM status;				/*该LED模块的状态*/
	bool pin_buffer;						/*引脚输出缓存*/
	/*------------------------------变常*/
	/*------------------------------常量*/
	DRV_LED_ENUM id;						/*LED模块id*/
	DRV_LED_LOGIC_TYPE_ENUM logic_type;		/*控制类型*/
	BSP_PORT_ENUM port_number; 				/*所控制的引脚端口号*/
	BSP_PIN_ENUM pin_number;				/*所控制的引脚编号*/
} DRV_LED_TYPE;

/* 接口函数
 ********************************************************/

/* 初始化LED模块
  -----------------------------
  入口：LED模块的ID号，正/负逻辑，IO端口
  返回值：初始化好的电磁阀模块结构体
*/
extern DRV_LED_TYPE* Led_Init(DRV_LED_ENUM id, \
							  DRV_LED_LOGIC_TYPE_ENUM valve_logic, \
							  BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number);

/* LED点亮
  -----------------------------
  入口：LED结构体
*/
extern void Led_On(DRV_LED_TYPE* p_unit);

/* LED熄灭
  -----------------------------
 入口：LED结构体
*/
extern void Led_Off(DRV_LED_TYPE* p_unit);

/* 判断LED是否点亮
  -----------------------------
  入口：LED结构体
  返回值：如果LED点亮则返回true
*/
extern bool Led_Is_On(DRV_LED_TYPE* p_unit);

#endif
