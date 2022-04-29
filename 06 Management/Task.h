/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——任务定义

  -------------------------------------------------------
   文 件 名   : Tesk.h
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年08月01日
   功能描述   : 定义并行任务。
   依赖于     : uC/OS-II V2.92.07
				Drv_Led V1.0.0.0
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#ifndef _TASK_H_
#define _TASK_H_

#include "stdlib.h"
#include "string.h"
#include "ucos_ii.h"
#include "Drv_Led.h"
#include "Drv_UartDataInterface.h"
#include "Prt_CmdRegedit.h"
#include "Prt_CmdScheduler.h"

#define CONFIG_LED_PORT  _PB_
#define CONFIG_LED_PIN   _P15_

/*--------------------------------------------------------------*/

/*----------------------------------------------------定义任务堆栈深度*/

#define TASK_START_STK_SIZE				128		/*初始任务 堆栈深度*/

#define TASK_CMD_TRANSCEIVER_STK_SIZE	256		/*指令收发器任务 堆栈深度*/
#define TASK_CMD_SCHEDULER1_STK_SIZE	256		/*指令调度器1任务 堆栈深度*/
#define TASK_CMD_SCHEDULER2_STK_SIZE	128		/*指令调度器2任务 堆栈深度*/

#define TASK_LED_STK_SIZE				128		/*系统指示灯任务 堆栈深度*/
#define TASK_RESIDENT_1_STK_SIZE		128		/*私有常驻任务 堆栈深度*/
#define TASK_RESIDENT_2_STK_SIZE		128		/*私有常驻任务 堆栈深度*/
#define TASK1_STK_SIZE 128		/*私有常驻任务 堆栈深度*/
#define TASK2_STK_SIZE 256    /*私有常驻任务 堆栈深度*/
#define TASK3_STK_SIZE 128    /*私有常驻任务 堆栈深度*/
#define TASK4_STK_SIZE 128    /*私有常驻任务 堆栈深度*/
#define TASK5_STK_SIZE 256    /*私有常驻任务 堆栈深度*/
#define TASK6_STK_SIZE 128    /*私有常驻任务 堆栈深度*/
/*----------------------------------------------------定义任务优先级，数值越小优先级越高*/

#define TASK_START_PRIO					20		/*初始任务 优先级*/

#define TASK_CMD_TRANSCEIVER_PRIO		4		/*指令收发器任务 优先级*/
//#define MUTEX_PRIO					5		/*互斥信号量任务提升优先级*/
#define TASK_CMD_SCHEDULER1_PRIO		5		/*指令调度器1任务 优先级*/
#define TASK_CMD_SCHEDULER2_PRIO		7		/*指令调度器2任务 优先级*/
#define TASK_RESIDENT_1_PRIO			6		/*私有常驻任务1 优先级*/
#define TASK_RESIDENT_2_PRIO			8		/*私有常驻任务1 优先级*/
#define TASK1_PRIO								9  /*私有任务1*/
#define TASK2_PRIO								10  /*私有任务2*/
#define TASK3_PRIO								11
#define TASK4_PRIO								12
#define TASK5_PRIO								13
#define TASK6_PRIO								14

#define TASK_LED_PRIO					19		/*系统指示灯任务 优先级*/

/*----------------------------------------------------变量及函数声明*/


extern OS_STK g_TaskStart_Stk[TASK_START_STK_SIZE];

/* 初始线程
  -----------------------------------
*/
extern void TaskStart(void* p_arg);

/* 指令收发器线程
  -----------------------------------
*/
static void TaskCmdTransceiver(void* p_arg);

/* 指令调度器1线程（私有指令线程1）
  -----------------------------------
*/
static void TaskCmdScheduler1(void* p_arg);

/* 指令调度器2线程（私有指令线程2）
  -----------------------------------
*/
extern void TaskCmdScheduler2(void* p_arg);
/* LED指示灯线程
  -----------------------------------
*/
static void TaskLed(void* p_arg);

/* 私有常驻线程1
  -----------------------------------
  由Func层实现
*/
extern void TaskResident1(void* p_arg);

/* 私有常驻线程2
  -----------------------------------
  由Func层实现
*/
extern void TaskResident2(void* p_arg);

/**/

extern void Task1(void* p_arg);

extern void Task2(void* p_arg);

extern void Task3(void* p_arg);

extern void Task4(void* p_arg);

extern void Task5(void* p_arg);

extern void Task6(void* p_arg);
#endif
