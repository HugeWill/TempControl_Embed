/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

  嵌入式开发平台——运动组件（电机+复位/工位/码齿/撞击信号）

  -------------------------------------------------------
   文 件 名   : Drv_MotionControlComponents.h
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年06月27日
   功能描述   : 运动组件定义
   依赖于     : uC/OS-II V2.92.07
				Drv_Motor V1.0.0.0
   注         ：该模块不依赖于硬件电路
 ********************************************************/

#ifndef _DRV_MOTIONCONTROLCOMPONENTS_H__
#define _DRV_MOTIONCONTROLCOMPONENTS_H__

#include "stdint.h"
#include "stdlib.h"
#include "ucos_ii.h"
#include "stm32f10x.h"
#include "Drv_StepMotor.h"

#define VRT_MAX_STEP 			20000		/*VerticalR1电机最大行程*/
#define VRT_GET_INTO_STEP		400			/*VerticalR1电机进入光耦脉冲数*/
#define VRT_SP_RESET_SLOW_STEP	10000		/*VerticalR1电机SP算法复位低速脉冲数*/

#define RRT_MAX_STEP 			20000		/*RRT电机最大行程*/
#define RRT_GET_INTO_STEP		150			/*RRT电机进入光耦脉冲数*/
#define RRT_SP_RESET_SLOW_STEP	10000		/*RRT电机SP算法复位低速脉冲数*/

#define INJ_MAX_STEP 			50000		/*INJ电机最大行程*/
#define INJ_GET_INTO_STEP		1600		/*INJ电机进入光耦脉冲数*/
#define INJ_SP_RESET_SLOW_STEP	10000		/*INJ电机SP算法复位低速脉冲数*/

#define MCC_PRESET_LOCATION_MAX_NUMBER	10	/*预设位置个数最大值*/

/*复位状态枚举*/
typedef enum {
    MCC_SHELTER_YES = 0,		/*遮挡状态*/
    MCC_SHELTER_NO = 1,			/*非遮挡状态*/
} MCC_SHELTER_ENUM;

/*运动曲线选择枚举*/
typedef enum {
    MCC_LINE_AUTO = 0,			/*自动选择曲线*/
    MCC_LINE_SLOW = 1,			/*匀速*/
    MCC_LINE_LOW = 2,			/*低速曲线*/
    MCC_LINE_MID = 3,			/*中速曲线*/
    MCC_LINE_HIGH = 4,			/*高速曲线*/
} MCC_LINE_CHOICE_ENUM;

/*组件复位方向矫正枚举*/
typedef enum {
    MCC_RESET_POSITIVE = 0,		/*不需矫正复位方向*/
    MCC_RESET_NEGATIVE = 1,		/*矫正复位方向*/
} MCC_RESET_DIR_ENUM;

/*组件类型枚举*/
typedef enum {
	MCC_A = 0,			/*只有复位信号的直线机构（柱塞泵）*/
	MCC_B = 1,			/*有复位信号和单码齿信号的直线机构（自动清洗）*/
	MCC_C = 2,			/*有复位信号和单码齿信号的旋转机构（搅拌旋转）*/
	MCC_D = 3,			/*有复位信号和多码齿信号的旋转机构（加样臂旋转）*/
	MCC_E = 4,			/*有复位信号和单码齿信号、撞击信号的直线机构（加样臂垂直）*/
	MCC_F = 5,			/*有复位信号和多码齿信号的无限旋转机构（盘旋转）*/
	MCC_G = 6,			/*有复位信号和多码齿信号的直线机构（调度区Y方向）*/
} MCC_TYPE_ENUM;

/*组件状态枚举*/
typedef enum {
	MCC_IDLE = 0,				/*空闲态*/
	MCC_MOVE_RESET = 1,			/*走复位态*/
	MCC_MOVE_DISTANCE = 2,		/*走固定脉冲态*/
	MCC_MOVE_D_CODE = 3,		/*走码齿态*/
	MCC_MOVE_LOW = 4,				/*匀速脱离*/
} MCC_STATUS_ENUM;

/*组件枚举*/
typedef enum{
	MCC_1 = 0,
	MCC_2 = 1,
	MCC_3 = 2,
	MCC_4 = 3,
	MCC_BUTT
} MCC_ENUM;

/*组件预设位置状态枚举*/
typedef enum{
	DISENABLE_CLOSE_CYCLE = 0,
	ENABLE_CLOSE_CYCLE = 1,
	UNDEFINED_CLOSE_CYCLE
} MCC_CLOSE_CYCLE_STATUS_ENUM;

/*组件补偿结构体*/
typedef struct{
	int32_t reset_compensation;				/*复位补偿，单位整步*/
} MCC_COMPENSATION;

/*组件预设位置结构体*/
typedef struct{
	MCC_CLOSE_CYCLE_STATUS_ENUM en_close_cycle;	/*闭环控制可用标识，1可用，0不可用，2表示未使用*/
	double data;								/*坐标值，单位°/mm/uL*/
	double forward_pre_deceleration_data;		/*正向预减速坐标值，单位°/mm/uL*/
	double reverse_pre_deceleration_data;		/*反向预减速坐标值，单位°/mm/uL*/
	int32_t forward_compensation;				/*正向补偿值，单位整步*/
	int32_t reverse_compensation;				/*反向补偿值，单位整步*/
} MCC_PRESET_LOCATION;

/*组件结构体*/
typedef struct{
	/*------------------------------变量*/
	MCC_STATUS_ENUM status;					/*组件状态*/
	int32_t position;						/*当前细分坐标*/
	int32_t location;						/*目标细分坐标*/
	uint32_t d_code_count;					/*码齿倒计数*/
	uint8_t strike_flag;					/*撞击标志*/
	uint8_t d_code_flag;					/*码齿到位标志*/
	
	/*------------------------------变常*/
	uint32_t true_station;					/*工位实际脉冲数记录*/
	MCC_PRESET_LOCATION* preset_location_now;	/*当前目标预设位置*/
	
	/*------------------------------常量*/
	MCC_ENUM id;							/*组件编号*/
	MCC_TYPE_ENUM type;						/*组件类型*/
	MCC_COMPENSATION* compensation;			/*复位补偿结构体*/
	MCC_PRESET_LOCATION* preset_location;	/*预设位置参数*/
	MCC_RESET_DIR_ENUM reset_dir;			/*复位方向*/
	MCC_SHELTER_ENUM reset_status;			/*复位的状态*/
	MCC_SHELTER_ENUM d_code_status;			/*码齿位的状态*/
	MCC_SHELTER_ENUM strike_status;			/*撞击的状态*/
	uint8_t subdivision;					/*细分*/
	double step_angle;						/*步距角（单位°）*/
	double ratio;							/*导程（旋转机构电机转一圈旋转的角度，单位°，直线机构电机转一圈前进的距离，单位mm）*/
	uint32_t max_step;						/*最大行程（脉冲数）*/
	OS_EVENT* sem;							/*组件信号量*/
	BSP_PORT_ENUM reset_port_number; 		/*复位信号的引脚端口号*/
	BSP_PIN_ENUM reset_pin_number;			/*复位信号的引脚编号*/
	BSP_PORT_ENUM d_code_port_number; 		/*码盘信号的引脚端口号*/
	BSP_PIN_ENUM d_code_pin_number;			/*码盘信号的引脚编号*/
	BSP_PORT_ENUM strike_port_number; 		/*撞击信号的引脚端口号*/
	BSP_PIN_ENUM strike_pin_number;			/*撞击信号的引脚编号*/
	BSP_PORT_ENUM strike2_port_number; 		/*第二撞击信号的引脚端口号*/
	BSP_PIN_ENUM strike2_pin_number;		/*第二撞击信号的引脚编号*/
	DRV_STEPMOTOR_TYPE* motor;				/*电机结构体*/
} DRV_MCC_TYPE;

/* 组件初始化
  ---------------------------------
  入口：组件编号，细分，步距角（°），导程（按角度°或距离mm），最大行程（按角度°或距离mm）
  组件类型，复位方向，复位状态
	码齿状态，撞击状态
  复位端口，工位端口，码齿端口，防撞端口，使能端口，方向端口，脉冲端口
  返回值：初始化好的组件指针
*/
extern DRV_MCC_TYPE* MCC_Init(MCC_ENUM id, \
							  uint8_t subdivision, double step_angle, double ratio, double max_distance, \
							  MCC_TYPE_ENUM type, MCC_RESET_DIR_ENUM reset_dir, MCC_SHELTER_ENUM reset_status, \
								MCC_SHELTER_ENUM d_code_status, MCC_SHELTER_ENUM strike_status, \
							  BSP_PORT_ENUM reset_port_number,   BSP_PIN_ENUM reset_pin_number, \
							  BSP_PORT_ENUM d_code_port_number,  BSP_PIN_ENUM d_code_pin_number, \
							  BSP_PORT_ENUM strike_port_number,  BSP_PIN_ENUM strike_pin_number, \
							  BSP_PORT_ENUM strike2_port_number,  BSP_PIN_ENUM strike2_pin_number, \
							  BSP_PORT_ENUM en_port_number,      BSP_PIN_ENUM en_pin_number, \
							  BSP_PORT_ENUM dir_port_number,     BSP_PIN_ENUM dir_pin_number, \
							  BSP_PORT_ENUM step_port_number,    BSP_PIN_ENUM step_pin_number);

 /* 组件保持力矩关闭
  ---------------------------------
  入口：p_mcc 电机结构体
*/
extern void MCC_Motor_Disable(DRV_MCC_TYPE* p_mcc);
							  
 /* 组件保持力矩开启
  ---------------------------------
  入口：p_mcc 电机结构体
*/
extern void MCC_Motor_Enable(DRV_MCC_TYPE* p_mcc);

 /* 单个组件按整步数运动
  ---------------------------------
  按照电机的整步数运动

  入口：p_mcc 电机结构体，dir 方向，step 整步数，
		timeout 超时时间，status 运动类型，count 如果走码齿位置的码齿数，
		line_choice 运动曲线选择
  返回值：err OS报错代码或0xFF
*/
extern uint8_t MCC_Move_SynchronizingStep(DRV_MCC_TYPE* p_mcc, \
										  uint8_t  dir, \
										  uint32_t step, \
										  uint32_t timeout, \
										  MCC_STATUS_ENUM status, \
										  uint8_t  count, \
										  MCC_LINE_CHOICE_ENUM line_choice);

/* 单个组件按整步数坐标运动
  ---------------------------------
  按照电机的整步数运动

  入口：p_mcc 电机结构体，location 脉冲坐标（相对于复位点），
		timeout 超时时间，status 运动类型，count 如果走码齿位置的码齿数，
		line_choice 运动曲线选择
  返回值：err OS报错代码或0xFF
*/
extern uint8_t MCC_Move_SynchronizingLocation(DRV_MCC_TYPE* p_mcc, \
											  int32_t location, \
											  uint32_t timeout, \
											  MCC_STATUS_ENUM status, \
											  uint8_t  count, \
											  MCC_LINE_CHOICE_ENUM line_choice);
									  
/* 单个组件按距离/角度运动
  ---------------------------------
  入口：p_MCC 电机结构体，dir 方向，distance 距离/角度，
		timeout 超时时间，status 运动类型，count 如果走码齿位置的码齿数，
		line_choice 运动曲线选择
  返回值：err OS报错代码或0xFF
*/
extern uint8_t MCC_Move_Distance(DRV_MCC_TYPE* p_MCC, \
								 uint8_t  dir, \
								 double distance, \
								 uint32_t timeout, \
								 MCC_STATUS_ENUM status, \
								 uint8_t  count, \
								 MCC_LINE_CHOICE_ENUM line_choice);

/* 单个组件按位置/角度坐标运动
  ---------------------------------
  入口：p_MCC 电机结构体，coordinates 位置/角度坐标（相对于复位点），
		slowstep 慢速步数，timeout 超时时间，status 运动类型，
		count 如果走码齿位置的码齿数，line_choice 运动曲线选择
  返回值：err OS报错代码或0xFF
*/
extern uint8_t MCC_Move_Coordinates(DRV_MCC_TYPE* p_MCC, \
									double coordinates, \
									uint32_t timeout, \
									MCC_STATUS_ENUM status, \
									uint8_t  count, \
									MCC_LINE_CHOICE_ENUM line_choice);

/* 单个组件按预设位置编号运动
  ---------------------------------
  入口：p_mcc 电机结构体，preset_location 预设位置参数表，
		timeout 超时时间，status 运动类型，
        count 如果走码齿位置的码齿数，line_choice 运动曲线选择
  返回值：err OS报错代码或0xFF
*/
extern uint8_t MCC_Move_PresetLocation(DRV_MCC_TYPE*     p_mcc, \
										MCC_PRESET_LOCATION* preset_location, \
										uint32_t             timeout, \
										MCC_STATUS_ENUM      status, \
										uint8_t              count, \
										MCC_LINE_CHOICE_ENUM line_choice);

/* 单个组件按码齿数运动
  ---------------------------------
  入口：p_mcc 电机结构体，preset_location 预设位置参数表，
		timeout 超时时间，status 运动类型，
        count 如果走码齿位置的码齿数，line_choice 运动曲线选择
  返回值：err OS报错代码或0xFF
*/
extern uint8_t MCC_Move_DCodeCount(DRV_MCC_TYPE*     p_mcc, \
								uint8_t                 dir, \
								MCC_PRESET_LOCATION* preset_location, \
								uint32_t             timeout, \
								MCC_STATUS_ENUM      status, \
								uint8_t              count, \
								MCC_LINE_CHOICE_ENUM line_choice);

/* 单个组件复位
  ---------------------------------
  入口：p_MCC 电机结构体，timeout 超时时间
  返回值：err, OS报错代码
*/
extern uint8_t MCC_Reset(DRV_MCC_TYPE* p_MCC, MCC_PRESET_LOCATION* preset_location, uint32_t timeout);

/* 获取复位光耦状态
  ---------------------------------
  入口：p_MCC 电机结构体
  返回值：1 复位状态，0 未复位
*/
extern bool MCC_Is_ResetOpStatus(DRV_MCC_TYPE* p_MCC);

/* 获取码齿间隔
  ---------------------------------
  入口：电机结构体，目标位置脉冲坐标
  返回值：1 复位状态，0 未复位
*/
extern uint16_t MCC_GetDCodeInterval(DRV_MCC_TYPE* p_mcc, int32_t location);

/* 按照实际距离或者角度换算脉冲数
  ---------------------------------
  入口：p_MCC 电机结构体，distance 距离/角度（单位°或mm）
  返回值：转换为脉冲数
*/
extern int32_t MCC_CalcSubdivisionStep(DRV_MCC_TYPE* p_MCC, double distance);

/* 设置位置参数
  ---------------------------------
  入口：p_mcc 电机结构体，location_num 位置编号，
  en_close_cycle 是否有码齿，data 坐标参数值（单位为mm/°/uL）
  正/反向预减速坐标参数值（单位为mm/°/uL），正/反向补偿参数值（单位为mm/°/uL）
  返回值：true 成功，flase 失败
*/
extern bool MCC_SetPresetLocation(DRV_MCC_TYPE* p_mcc, uint8_t location_num, 
							MCC_CLOSE_CYCLE_STATUS_ENUM en_close_cycle, double data,
							double forward_pre_deceleration_data, double reverse_pre_deceleration_data,
							double forward_compensation, double reverse_compensation);

/* 设置补偿参数
  ---------------------------------
	入口：p_mcc 电机结构体，reset_compensation 补偿值（单位为整步）
  返回值：true 成功，flase 失败
*/
extern bool MCC_SetCompensation(DRV_MCC_TYPE* p_mcc, int32_t reset_compensation);

/* 设置加减速参数
  ---------------------------------
	入口：p_mcc 电机结构体，start_fre 启动频率，end_fre 最高频率，ech_lader_step 每台阶微步，up_max_lader 最大加速台阶，s_par S型曲线的形状参数，ch 0高速曲线 1低速曲线
  返回值：true 成功，flase 失败
*/
extern bool MCC_SetStempMotorLine(DRV_MCC_TYPE* p_mcc, uint32_t start_fre,	uint32_t end_fre,	uint32_t ech_lader_step, uint32_t up_max_lader,	uint32_t s_par, uint8_t ch);

/* 设置加减速参数（5段S曲线）
  ---------------------------------
	入口：p_mcc 电机结构体，start_fre 启动频率，end_fre 最高频率，ech_lader_step 每台阶微步，up_max_lader 最大加速台阶，s_par S型曲线的形状参数，ch 0高速曲线 1低速曲线
  返回值：true 成功，flase 失败
*/
extern bool MCC_SetStempMotorLine5(DRV_MCC_TYPE* p_mcc, uint32_t start_fre,	uint32_t end_fre,	uint32_t ech_lader_step, uint32_t up_max_lader,	uint32_t s_par, uint8_t ch);

/* 设置加减速参数(梯形曲线)
  ---------------------------------
	入口：p_mcc 电机结构体，start_fre 启动频率，end_fre 最高频率，ech_lader_step 每台阶微步，up_max_lader 最大加速台阶，s_par S型曲线的形状参数，ch 0高速曲线 1低速曲线
  返回值：true 成功，flase 失败
*/
extern bool MCC_SetStempMotorLineT(DRV_MCC_TYPE* p_mcc, uint32_t start_fre,	uint32_t end_fre,	uint32_t ech_lader_step, uint32_t up_max_lader,	uint32_t s_par, uint8_t ch);

/* 内部函数
 ********************************************************/
 
/* 单个组件按细分脉冲数运动
  ---------------------------------
  入口：p_MCC 电机结构体，dir 方向，step 脉冲数，
		timeout 超时时间，status 运动类型，count 如果走码齿位置的码齿数，
		line_choice 运动曲线选择
  返回值：err OS报错代码或0xFF
*/
static uint8_t _MCC_Move_SubdivisionStep(DRV_MCC_TYPE* p_MCC, \
										  uint8_t  dir, \
										  uint32_t step, \
										  uint32_t timeout, \
										  MCC_STATUS_ENUM status, \
										  uint8_t  count, \
										  MCC_LINE_CHOICE_ENUM line_choice);

/* 单个组件按脉冲坐标运动
  ---------------------------------
  入口：p_MCC 电机结构体，location 脉冲坐标（相对于复位点），
		timeout 超时时间，status 运动类型，count 如果走码齿位置的码齿数，
		line_choice 运动曲线选择
  返回值：err OS报错代码或0xFF
*/
static uint8_t _MCC_Move_Location(DRV_MCC_TYPE* p_MCC, \
							     int32_t location, \
							     uint32_t timeout, \
							     MCC_STATUS_ENUM status, \
								 uint8_t  count, \
							     MCC_LINE_CHOICE_ENUM line_choice);


/* MCC 复位中断处理函数
  ------------------------------------
  入口：p_unit 组件结构体
*/
static void _Mcc_ResetExtiIrqHandler(DRV_MCC_TYPE* p_unit);

/* MCC 单码齿中断处理函数
  ------------------------------------
  入口：p_unit 组件结构体
*/
static void _Mcc_SingleDCodeExtiIrqHandler(DRV_MCC_TYPE* p_unit);

/* MCC 多码齿中断处理函数
  ------------------------------------
  入口：p_unit 组件结构体
*/
static void _Mcc_DCodeExtiIrqHandler(DRV_MCC_TYPE* p_unit);

/* MCC 撞击中断
  ------------------------------------
  入口：p_unit 组件结构体
*/
static void MCC_StrikeExtiIrqHandler(DRV_MCC_TYPE* p_unit);

/* 外部接口函数
 ********************************************************/

/* MCC_1 复位中断
  ------------------------------------
*/
extern void MCC1_ResetExtiIrqHandler(void);
/* MCC_1 码齿中断
  ------------------------------------
*/
extern void MCC1_DCodeExtiIrqHandler(void);
/* MCC_1 撞击中断
  ------------------------------------
*/
extern void MCC1_StrikeExtiIrqHandler(void);

/* MCC_2 复位中断
  ------------------------------------
*/
extern void MCC2_ResetExtiIrqHandler(void);
/* MCC_2 码齿中断
  ------------------------------------
*/
extern void MCC2_DCodeExtiIrqHandler(void);
/* MCC_2 撞击中断
  ------------------------------------
*/
extern void MCC2_StrikeExtiIrqHandler(void);

/* MCC_3复位中断
  ------------------------------------
*/
extern void MCC3_ResetExtiIrqHandler(void);
/* MCC_3 码齿中断
  ------------------------------------
*/
extern void MCC3_DCodeExtiIrqHandler(void);
/* MCC_3 撞击中断
  ------------------------------------
*/
extern void MCC3_StrikeExtiIrqHandler(void);

/* MCC_4复位中断
  ------------------------------------
*/
extern void MCC4_ResetExtiIrqHandler(void);
/* MCC_4 码齿中断
  ------------------------------------
*/
extern void MCC4_DCodeExtiIrqHandler(void);
/* MCC_4 撞击中断
  ------------------------------------
*/
extern void MCC4_StrikeExtiIrqHandler(void);

#endif
