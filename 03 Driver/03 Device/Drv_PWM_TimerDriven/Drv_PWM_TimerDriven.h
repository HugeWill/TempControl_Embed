/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

          嵌入式开发平台——PWM模块（使用定时器驱动）

  -------------------------------------------------------
   文 件 名   : Drv_PWM_TimerDriven.h
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年06月28日
   功能描述   : 定义一种输出PWM脉冲的模块，使用定时器控制
				指定引脚的输出频率，脉宽50%不可调。
   使用方法   ：上层模块通过调用PWM_Init获得一个PWM模块的指
				针，通过调用PWM_Config配置要输出的脉冲数和
				频率，通过调用PWM_Enable启动PWM输出，通过调
				用PWM_Disable中止PWM输出，并返回剩余脉冲数，
				__PWM_Done输出完成处理函数由上层实现，要求
				快进快出，不得使用延时。
   依赖于     : BSP.h
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#ifndef _DRV_PWM_TIMERDRIVEN_H_
#define _DRV_PWM_TIMERDRIVEN_H_

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "BSP_Gpio.h"
#include "BSP_Timer.h"

/*PWM模块枚举*/
typedef enum{
	PWM_1 = 0,
	PWM_2 = 1,
	PWM_3 = 2,
	PWM_4 = 3,
	PWM_BUTT
} DRV_PWM_TIMERDRIVEN_ENUM;

/*PWM模块状态枚举*/
typedef enum{
	PWM_UNDEFINED = 0,					/*未初始化*/
	PWM_READY     = 1,					/*待命*/
	PWM_BUSY      = 2,					/*工作*/
	PWM_ERROR     = 3,					/*出错*/
} DRV_PWM_TIMERDRIVEN_STATUS_ENUM;

/*PWM模块结构体*/
typedef struct{
	/*------------------------------变量*/
	DRV_PWM_TIMERDRIVEN_STATUS_ENUM status;	/*该PWM模块的状态*/
	uint8_t pin_buffer;						/*引脚输出缓存*/
	uint32_t remaining_pulse;				/*剩余脉冲数*/
	/*------------------------------变常*/
	uint32_t total_pulse;					/*脉冲总数*/
	uint16_t fre;							/*频率*/	
	/*------------------------------常量*/
	DRV_PWM_TIMERDRIVEN_ENUM id;			/*PWM模块id*/
	BSP_TIMER_ENUM timer_number;			/*所使用的定时器*/
	BSP_PORT_ENUM port_number; 				/*所控制的引脚端口号*/
	BSP_PIN_ENUM pin_number;				/*所控制的引脚编号*/
} DRV_PWM_TIMERDRIVEN_TYPE;

/* 接口函数
 ********************************************************/

/* PWM模块初始化
  --------------------------------
  入口：模块编号，定时器编号，引脚端口编号，引脚编号
  返回值：分配的PWM模块结构体地址，失败返回NULL
*/
extern DRV_PWM_TIMERDRIVEN_TYPE* PWM_Init(DRV_PWM_TIMERDRIVEN_ENUM id, \
										  BSP_TIMER_ENUM timer_number, \
										  BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number);

/* PWM模块输出配置
  --------------------------------
  入口：p_unit PWM模块结构体，step 总脉冲数，fre 输出频率
  返回值：true配置成功，false配置失败
*/
extern bool PWM_Config(DRV_PWM_TIMERDRIVEN_TYPE* p_unit, uint32_t step, uint16_t fre);

/* PWM输出使能
  --------------------------------
  用于启动PWM输出
  入口：p_unit PWM模块结构体
  返回值：true使能成功，false使能失败
*/
extern bool PWM_Enable(DRV_PWM_TIMERDRIVEN_TYPE* p_unit);

/* PWM输出停止
  --------------------------------
  用于中止PWM输出
  入口：p_unit PWM模块结构体
  返回值：剩余未输出脉冲数
*/
extern uint32_t PWM_Disable(DRV_PWM_TIMERDRIVEN_TYPE* p_unit);

/* 内部函数
 ********************************************************/
 
/* PWM输出更新
  --------------------------------
  用于在定时器中断中更新PWM输出
  入口：p_unit PWM模块结构体
  返回值：该模块状态
*/
static DRV_PWM_TIMERDRIVEN_STATUS_ENUM _PWM_Update(DRV_PWM_TIMERDRIVEN_TYPE* p_unit);

/* 中断处理函数
 ********************************************************/
 
/* PWM1输出中断
  --------------------------------
*/
static void _Drv_PWM1IRQHandler(void);

/* PWM2输出中断
  --------------------------------
*/
static void _Drv_PWM2IRQHandler(void);

/* PWM3输出中断
  --------------------------------
*/
static void _Drv_PWM3IRQHandler(void);

/* PWM4输出中断
  --------------------------------
*/
static void _Drv_PWM4IRQHandler(void);

/* 外部函数
 ********************************************************/

/* PWM输出完成
  --------------------------------
  由上层模块实现
*/
extern void __PWM_Done(DRV_PWM_TIMERDRIVEN_TYPE* p_unit);

#endif
