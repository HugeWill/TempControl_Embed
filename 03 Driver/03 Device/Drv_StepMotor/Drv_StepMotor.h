/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

          嵌入式开发平台——电机模块（控制STEP、DIR和EN）

  -------------------------------------------------------
   文 件 名   : Drv_StepMotor.h
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年06月24日
   功能描述   : 定义一种控制步进电机加减速运动的模块，使用
				外部的PWM模块作为脉冲输出。通过配置加减速参
				数，实现电机的任意S型曲线控制、匀速控制，以
				及减速控制。
   使用方法   ：上层模块通过调用Motor_Init，获得一个电机模
				块的指针，通过调用Motor_MoveSP_Begin函数，
				启动电机的加减速控制，通过Motor_Move_Stop函
				数实现运动的中止，__Motor_Done电机运行完成
				函数由上层模块实现。
   依赖于     : BSP V1.0.0.0
				Drv_PWM_TimerDriven V1.0.0.0
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#ifndef _DRV_STEPMOTOR_H__
#define _DRV_STEPMOTOR_H__

#include "stdint.h"
#include "stdbool.h"
#include "math.h"
#include "BSP_Gpio.h"
#include "Drv_PWM_TimerDriven.h"

/*电机模块枚举*/
typedef enum{
	MOTOR_1   = 0,
	MOTOR_2   = 1,
	MOTOR_3   = 2,
	MOTOR_4   = 3,
	MOTOR_BUTT
} DRV_STEPMOTOR_ENUM;

/*电机状态枚举*/
typedef enum {
    MOTOR_IDLE      = 0,		  	/*空闲态*/
    MOTOR_SPEEDUP   = 1,	 		/*加速态*/
    MOTOR_RUNING    = 2,	   		/*运行态*/
    MOTOR_SPEEDDOWN = 3, 			/*减速态*/
    MOTOR_SLOW      = 4,		 	/*慢速态*/
} DRV_STEPMOTOR_STATUS_ENUM;

/*台阶-微步结构体*/
typedef struct
{ 
    uint32_t ladder;		/*台阶*/
    uint32_t step;			/*微步*/
} DRV_STEPMOTOR_LADDERSTEP_TYPE;

/*台阶-微步结构体组*/
typedef struct
{
	uint8_t count;			/*台阶-微步结构体个数*/
	DRV_STEPMOTOR_LADDERSTEP_TYPE* ladder_step;	/*指向台阶-微步结构体的指针*/
} DRV_STEPMOTOR_LADDERSTEPGROUP_TYPE;

/*电机加减速配置结构体*/
typedef struct
{
	uint32_t start_fre;				/*启动频率 PPS*/
	uint32_t end_fre;				/*最高频率 PPS*/
	uint32_t ech_lader_step;		/*每台阶微步 uP*//*5段自动生成*/
	uint32_t up_max_lader;			/*最大加速台阶*/
	uint32_t s_par;					/*S型曲线的形状参数*//*5段标识加速时间ms*/
} DRV_STEPMOTOR_LINE_TYPE;

/*电机结构体*/
typedef struct
{
	/*------------------------------------------变量*/
    DRV_STEPMOTOR_STATUS_ENUM  status;	/*电机状态*/
    int32_t   curLadder;     			/*当前台阶*/
    uint32_t  curStep;					/*当前步数*/
    uint32_t  totalStep;   				/*当前台阶总步数*/
	int32_t   position;					/*当前坐标*/
	
	/*------------------------------------------变常*/
    uint8_t   en_buffer;      			/*使能引脚输出缓存*/
    uint8_t   dir_buffer;      			/*方向引脚输出缓存*/
    uint32_t  slowStep;      			/*慢速运动步数*/
    uint16_t  slowFre;					/*慢速运行频率*/
    uint16_t* p_fre_sp;					/*电机加速频率表*/
    uint16_t* p_fre_sd;					/*电机减速频率表*/
	uint8_t   break_away;				/*启动时是否先匀速脱离*/
	uint32_t  break_away_step;			/*匀速脱离步数*/
    uint32_t  maxLadder_sp;				/*最大的加速台阶数*/
    uint32_t  maxLadder_sd;  			/*最大的减速台阶数*/
    uint32_t  constStep;     			/*匀速运动总步数*/
    uint32_t  sd_total_step;			/*减速运动总步数*/
	
	/*------------------------------------------常量*/
	DRV_STEPMOTOR_ENUM id;				/*电机模块id*/
	
	BSP_PORT_ENUM en_port_number;		/*EN引脚端口号*/
	BSP_PIN_ENUM  en_pin_number;		/*EN引脚序号*/
	BSP_PORT_ENUM dir_port_number;		/*DIR引脚端口号*/
	BSP_PIN_ENUM  dir_pin_number;		/*DIR引脚序号*/
	DRV_PWM_TIMERDRIVEN_TYPE* step_pin;	/*STEP引脚*/
	DRV_STEPMOTOR_LINE_TYPE* line_high;	/*高速运动参数表*/
	DRV_STEPMOTOR_LINE_TYPE* line_mid;	/*中速运动参数表*/
	DRV_STEPMOTOR_LINE_TYPE* line_low;	/*低速运动参数表*/
    DRV_STEPMOTOR_LADDERSTEPGROUP_TYPE stLadStep_sp;	/*加速台阶-微步配置结构体*/
    DRV_STEPMOTOR_LADDERSTEPGROUP_TYPE stLadStep_sd;	/*减速台阶-微步配置结构体*/
	uint16_t* line_hig_fre;				/*高速运动频率表*/
	uint16_t* line_mid_fre;				/*中速运动频率表*/
	uint16_t* line_low_fre;				/*低速运动频率表*/
	uint8_t* lineh_laderstep;		/*微步表*/
	uint8_t* linem_laderstep;		/*微步表*/
	uint8_t* linel_laderstep;		/*微步表*/
	uint8_t subdivision;				/*细分*/
	uint8_t line_switch;
} DRV_STEPMOTOR_TYPE;

/* 接口函数
 ********************************************************/

/* 电机模块初始化
  --------------------------------
  入口：模块编号，使能引脚端口，方向引脚端口，脉冲引脚端口
  返回值：分配的电机模块结构体地址，失败返回NULL
*/
extern DRV_STEPMOTOR_TYPE* Motor_Init(DRV_STEPMOTOR_ENUM id, uint8_t subdivision, \
									  BSP_PORT_ENUM en_port_number,   BSP_PIN_ENUM en_pin_number, \
									  BSP_PORT_ENUM dir_port_number,  BSP_PIN_ENUM dir_pin_number, \
									  BSP_PORT_ENUM step_port_number, BSP_PIN_ENUM step_pin_number);

/* SP运动启动函数
  --------------------------------
  ->加速->匀速->减速->慢速->停止
  入口：p_unit 电机结构体，dir 方向，step 总步数，slowstep 慢速步数，line_switch 高低速曲线选择
  返回值：无
*/
extern void Motor_MoveSP_Begin(DRV_STEPMOTOR_TYPE* p_unit, uint8_t dir, uint32_t step, uint32_t slowstep, uint8_t line_switch);

/* RL运动启动函数
  --------------------------------
  ->慢速->停止
  入口：p_unit 电机结构体，dir 方向，step 总步数，slowstep 慢速步数，fre 频率
  返回值：无
*/
extern void Motor_MoveRL_Begin(DRV_STEPMOTOR_TYPE* p_unit, uint8_t dir, uint32_t step, uint32_t slowstep, uint32_t fre);

/* SD运动启动函数
  --------------------------------
  ->减速->慢速->停止
  入口：p_unit 电机结构体，dir 方向，step 总步数，slowstep 慢速步数
  返回值：无
*/
extern void Motor_MoveSD_Begin(DRV_STEPMOTOR_TYPE* p_unit, uint8_t dir, uint32_t step, uint32_t slowstep);

/* 运动停止函数
  --------------------------------
  入口：p_unit 电机结构体
  返回值：无
*/
extern void Motor_Move_Stop(DRV_STEPMOTOR_TYPE* p_unit);

/* 设置是否缓慢退出
  --------------------------------
  入口：p_unit 电机结构体，break_away 是否需要，break_away_step 运动步数
  返回值：无
*/
extern void Motor_Set_BreakAway(DRV_STEPMOTOR_TYPE* p_unit, uint8_t break_away, uint32_t break_away_step);

/*
  计算加减速曲线频率表
  --------------------------------
  入口：细分数 moto_line 加减速曲线参数表，fre_arry 加减速曲线频率表
  返回值：无
*/
extern void CalcMotorFre(uint8_t subdivision, DRV_STEPMOTOR_LINE_TYPE* moto_line, uint16_t* fre_arry, uint8_t* up_arry);

/*
  计算加减速曲线频率表(5段S曲线)
  --------------------------------
  入口：细分数 moto_line 加减速曲线参数表，fre_arry 加减速曲线频率表
  返回值：无
*/
extern void CalcMotorFre5(uint8_t subdivision, DRV_STEPMOTOR_LINE_TYPE* moto_line, uint16_t* fre_arry, uint8_t* up_arry);

/*
  计算加减速曲线频率表（梯形曲线）
  --------------------------------
  入口：moto_line 加减速曲线参数表，fre_arry 加减速曲线频率表
  返回值：无
*/
extern void CalcMotorFreT(uint8_t subdivision, DRV_STEPMOTOR_LINE_TYPE* moto_line, uint16_t* fre_arry, uint8_t* up_arry);

/* 内部函数
 ********************************************************/

/* 运动过程更新函数
  --------------------------------
  在一个台阶的PWM输出完成时调用
  入口：p_unit 电机结构体
  返回值：更新后电机状态
*/
static DRV_STEPMOTOR_STATUS_ENUM _Motor_Move_Update(DRV_STEPMOTOR_TYPE* p_unit);

/* 配置SP运行函数
  --------------------------------
  入口：p_unit 电机结构体，step 总步数，line_switch 高中低速曲线选择 0低速、1中速、2高速
  返回值：无
*/
static void _ConfigSP(DRV_STEPMOTOR_TYPE* p_unit, uint32_t step, uint8_t line_switch);

/* 配置SD运行函数
  --------------------------------
  入口：p_unit 电机结构体，step 总运动步数，
  返回值：无
*/
static void _ConfigSD(DRV_STEPMOTOR_TYPE* p_unit, uint32_t step);

/* 电机控制状态机
  --------------------------------
  切换台阶时调用
  入口：p_unit 所需控制电机的结构体
  返回值：无
*/
static void _MotorDrv(DRV_STEPMOTOR_TYPE* p_unit);

/* 配置SP Ladstep
  --------------------------------
  入口：p_unit 电机结构体，index 下标（用于分段运动使用），ladder 台阶数，step 每台阶步数
  返回值：false配置失败，true配置成功
*/
static bool _LadstepSp_Config(DRV_STEPMOTOR_TYPE* p_unit, uint8_t index, uint32_t ladder, uint32_t step, uint8_t line_switch);

/* 配置SD Ladstep
  --------------------------------
  入口：p_unit 电机结构体，index 下标（用于分段运动使用），ladder 台阶数，step 每台阶步数
  返回值：false配置失败，true配置成功
*/
static bool _LadstepSd_Config(DRV_STEPMOTOR_TYPE* p_unit, uint8_t index, uint32_t ladder, uint32_t step, uint8_t line_switch);

/*
  获取加减速台阶的步数
  --------------------------------
  入口：p_unit 电机结构体
  返回值：当前台阶步数
*/
static uint32_t _Calc_Cur_Step(DRV_STEPMOTOR_TYPE* p_unit);

/* 申请电机运行频率表存储空间
  --------------------------------
  入口：count 台阶个数
  返回值：分配的内存地址
*/
static uint16_t* _CallocMotorFre(uint16_t count);

/* 申请台阶-微步结构体组存储空间
  --------------------------------
  入口：p_ladder_step_group 台阶-微步结构体组指针，count 成员个数
  返回值：分配的内存地址
*/
extern DRV_STEPMOTOR_LADDERSTEP_TYPE* _CallocLadstep(DRV_STEPMOTOR_LADDERSTEPGROUP_TYPE* p_ladder_step_group, uint8_t count);

/* 电机硬件初始化
  --------------------------------
  入口：EN引脚位置和DIR引脚位置
  返回值：false初始化失败，true初始化成功
*/
static bool _Motor_BSP_Init(GPIO_TypeDef* en_gpio, uint16_t en_pin, GPIO_TypeDef* dir_gpio, uint16_t dir_pin);

/* 外部函数
 ********************************************************/

/* PWM输出完成
  --------------------------------
  由上层模块实现
*/
extern void __Motor_Done(DRV_STEPMOTOR_TYPE* p_unit);

#endif


