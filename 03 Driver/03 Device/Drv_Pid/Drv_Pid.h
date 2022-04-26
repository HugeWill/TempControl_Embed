/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——PID模块

  -------------------------------------------------------
   文 件 名   : Drv_Pid.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2020年2月14日
   功能描述   : 定义一种PID模块。
   使用方法   ：
   依赖于     : 
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/
 
#ifndef _DRV_PID_H_
#define _DRV_PID_H_ 

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "float.h"
#include "ucos_ii.h"
#include "Drv_Heat.h"
#include "BSP_Gpio.h"

#define DEFAULTPERIOD	1
#define	FULLDUTY		200
#define	ARRAY_DUTY		100.0	/*积分限幅百分比*/
#define	ARRAY_ERR		5  	/*积分舍弃偏差*/
/*圆周率*/
#define PI 3.14159

typedef void PidFunc(void);

/*PID模块枚举*/
typedef enum{
	PID_1 = 0,
	PID_2,
	PID_3,
	PID_4,
	PID_5,
	PID_6,
	PID_7,
	PID_8,
	PID_BUTT
} DRV_PID_ENUM;

/*PID模块状态枚举*/
typedef enum{
	PID_TUNING = 0,		/*整定*/
	PID_WORKING = 1,	/*运行*/
} DRV_PID_STATUS_ENUM;

/*PID控制状态*/
typedef enum
{
	PID_CAL = 0, /*PID计算*/
	PID_CON = 1, /*PID控制*/
}DRV_PID_CONTROLSTATUS_ENUM;

/*PID模块结构体*/
typedef struct{
	/*------------------------------变量*/
    float lasterr; 			/*Error[-1]*/
    float preverr; 			/*Error[-2]*/
    float sumerr; 			/*Sums of Errors*/
	DRV_PID_STATUS_ENUM status;
	/*------------------------------变常*/
	/*------------------------------常量*/
	DRV_PID_ENUM id;		/*PID模块id*/
    float setpoint; 		/*设定目标*/
    float kp; 				/*比例常数*/
    float ki; 				/*积分常数*/
    float kd; 				/*微分常数*/
} DRV_PID_TYPE;

/*PID自整定过程枚举*/
typedef enum 
{
    PRE_PROCESS=0,
    HIGH1,
    LOW1,
    HIGH2,
    LOW2,
    LOW3,
    Average,
    END
}TUNING_STAT;

/*PID自整定结构体*/
typedef struct{
  DRV_PID_TYPE* pid; /*pid模块结构体*/
	void *modular; 				/*模块类型*/
	PidFunc *positivefun;	/*正向函数指针*/
	PidFunc *negativefun;	/*反相函数指针*/
	/*一个pid控制周期时长*/
	float period;
	/*占空比*/
	float duty;
	int16_t pertim; /*周期时长计数*/
//	float aduty;
//	float wduty;
/*用于自整定*/
	DRV_PID_CONTROLSTATUS_ENUM constatus; /*PID控制状态*/
	TUNING_STAT status;
	float tuning_max;
	float tuning_min;
	unsigned int tuning_period;
	/*防止抖动*/
	int jitter;
} DRV_PIDTUNINGPARA_TYPE;

/* 接口函数
 ********************************************************/

/* 初始化PID模块
  -----------------------------
  入口：PID模块的ID号，目标值，P、I、D参数
  返回值：初始化好的PID模块结构体
*/
extern DRV_PID_TYPE* Drv_PidInit(DRV_PID_ENUM id, float setpoint, float kp, float ki, float kd);

/* 初始化PID自整定结构体
  -----------------------------
 */
extern void Drv_PidTuningParaInit(DRV_PIDTUNINGPARA_TYPE* pid_tuning_para, DRV_PID_TYPE* pid, void *modular, void *positivefun, void *negativefun);

/* PID模块输出计算
  -----------------------------
  入口：PID模块指针，当前值
  返回值：输出值
*/
extern float Drv_PidCalculate(DRV_PID_TYPE* p_unit, float current);

/* PID模块复位
  -----------------------------
  入口：PID模块指针
*/
extern void Drv_PidReset(DRV_PID_TYPE* p_unit);

/* PID参数自整定
  -----------------------------
  返回值：自整定状态
*/
extern TUNING_STAT Drv_PidTuning(DRV_PIDTUNINGPARA_TYPE* pid_tuning_para, float current);

/* 消除整定过程状态迁移的抖动
  -----------------------------
*/
static bool _Drv_Avoidjitter(DRV_PIDTUNINGPARA_TYPE *pid_tuning_para);

/* 消除整定过程状态迁移的抖动
  -----------------------------
*/
static void _Drv_PidPara(DRV_PIDTUNINGPARA_TYPE* pid_tuning_para, unsigned int tu, float k);

#endif
