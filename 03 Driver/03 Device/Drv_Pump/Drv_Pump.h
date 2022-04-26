/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——泵模块

  -------------------------------------------------------
   文 件 名   : Drv_Pump.h
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年09月26日
   功能描述   : 定义一种泵控制模块，支持四个泵的控制。
   使用方法   ：上层模块通过调用Pump_Init获得一个Pump模块
				的指针，通过调用Pump_Start开启泵，通过
				调用Pump_Stop关闭泵。
   依赖于     : BSP.h
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/
 
#ifndef _DRV_PUMP_H_
#define _DRV_PUMP_H_

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "BSP_Gpio.h"
#include "BSP_Timer.h"

/*泵类型枚举*/
typedef enum{
	PUMP_UNADJUSTABLE_SPEED = 0,			/*不可调速泵*/
	PUMP_ADJUSTABLE_SPEED_WITH_POWER = 1,	/*电源调速泵*/
	PUMP_ADJUSTABLE_SPEED_WITH_SIGNAL = 2,	/*信号调速泵*/	
} DRV_PUMP_TYPE_ENUM;

/*驱动类型枚举*/
typedef enum{
	PUMP_POSITIVE_LOGIC = 0,	/*正逻辑控制*/
	PUMP_NEGATIVE_LOGIC = 1,	/*负逻辑控制*/
} DRV_PUMP_LOGIC_TYPE_ENUM;

/*泵模块枚举*/
typedef enum{
	PUMP_1 = 0,
	PUMP_2,
	PUMP_3,
	PUMP_4,
	PUMP_5,
	PUMP_6,
	PUMP_7,
	PUMP_8,
	PUMP_9,
	PUMP_10,
	PUMP_11,
	PUMP_12,
	PUMP_13,
	PUMP_14,
	PUMP_15,
	PUMP_16,
	PUMP_17,
	PUMP_18,
	PUMP_19,
	PUMP_20,
	PUMP_21,
	PUMP_22,
	PUMP_23,
	PUMP_24,
	PUMP_25,
	PUMP_26,
	PUMP_27,
	PUMP_28,
	PUMP_29,
	PUMP_30,
	PUMP_31,
	PUMP_32,
	PUMP_33,
	PUMP_34,
	PUMP_35,
	PUMP_BUTT
} DRV_PUMP_ENUM;

/*泵模块状态枚举*/
typedef enum{
	PUMP_UNDEFINED = 0,				/*未初始化*/
	PUMP_START     = 1,				/*开启*/
	PUMP_STOP      = 2,				/*停止*/
} DRV_PUMP_STATUS_ENUM;

/*泵模块结构体*/
typedef struct{
	/*------------------------------变量*/
	DRV_PUMP_STATUS_ENUM status;			/*该泵模块的状态*/
	uint8_t duty_cycle_count;				/*占空比计数*/
	bool power_pin_buffer;					/*电源引脚输出缓存*/
	bool signal_pin_buffer;					/*信号引脚输出缓存*/
	/*------------------------------变常*/
	/*------------------------------常量*/
	DRV_PUMP_ENUM id;						/*泵模块id*/
	DRV_PUMP_TYPE_ENUM type;				/*泵类型*/
	DRV_PUMP_LOGIC_TYPE_ENUM logic_type;	/*控制类型*/
	uint32_t fre;							/*频率（单位Hz）*/
	uint8_t duty_cycle;						/*占空比*/
	BSP_TIMER_ENUM timer_number;			/*所使用的定时器*/
	BSP_PORT_ENUM power_port_number; 		/*所控制的电源引脚端口号*/
	BSP_PIN_ENUM power_pin_number;			/*所控制的电源引脚编号*/
	BSP_PORT_ENUM signal_port_number; 		/*所控制的信号引脚端口号*/
	BSP_PIN_ENUM signal_pin_number;			/*所控制的信号引脚编号*/
} DRV_PUMP_TYPE;

/* 接口函数
 ********************************************************/

/* 初始化泵模块
  -----------------------------
  入口：泵的ID号，不可调速/电源调速/信号调速，正/负逻辑，IO端口
  返回值：初始化好的电磁阀模块结构体
*/
extern DRV_PUMP_TYPE* PumpInit(DRV_PUMP_ENUM id, \
								  DRV_PUMP_TYPE_ENUM pump_type, \
								  DRV_PUMP_LOGIC_TYPE_ENUM pump_logic, \
								  uint32_t fre, \
								  uint8_t duty_cycle, \
								  BSP_TIMER_ENUM timer_number, \
								  BSP_PORT_ENUM power_port_number, BSP_PIN_ENUM power_pin_number, \
								  BSP_PORT_ENUM signal_port_number, BSP_PIN_ENUM signal_pin_number);

/* 泵启动
  -----------------------------
  入口：泵结构体
*/
extern void PumpStart(DRV_PUMP_TYPE* p_unit);

/* 泵停止
  -----------------------------
 入口：泵结构体
*/
extern void PumpStop(DRV_PUMP_TYPE* p_unit);

/* 判断泵是否启动
  -----------------------------
  入口：泵结构体
  返回值：如果泵启动则返回true
*/
extern bool PumpIsStart(DRV_PUMP_TYPE* p_unit);

/* 内部函数
 ********************************************************/

/* 泵电源开启
  -----------------------------
  入口：泵结构体
*/
static void _PumpPowerUp(DRV_PUMP_TYPE* p_unit);

/* 泵电源关闭
  -----------------------------
  入口：泵结构体
*/
static void _PumpPowerOff(DRV_PUMP_TYPE* p_unit);

/* 泵信号线拉高
  -----------------------------
  入口：泵结构体
*/
static void _PumpSignalUp(DRV_PUMP_TYPE* p_unit);

/* 泵信号线拉低
  -----------------------------
  入口：泵结构体
*/
static void _PumpSignalOff(DRV_PUMP_TYPE* p_unit);

/* 中断处理函数
 ********************************************************/

/* 泵调速中断
  -----------------------------
*/
static void _Drv_PumpIRQHandler(void);

#endif
