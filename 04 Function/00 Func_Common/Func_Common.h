/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——通用功能

  -------------------------------------------------------
   文 件 名   : Func_Common.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年06月27日
   功能描述   : 通用功能定义
   依赖于     : 
 ********************************************************/

#ifndef _FUNC_COMMON_H_
#define _FUNC_COMMON_H_

#include "stdint.h"
#include "Drv_CANProtocol.h"
#include "Drv_MotionControlComponents.h"
#include "Drv_Valve.h"
#include "Drv_Pump.h"
#include "Prt_CmdDoubleList.h"
#include "Drv_At24c32.h"
#include "BSP_Sys.h"

#pragma pack(1)	/*按1字节对齐*/

typedef struct {
	uint8_t flag;								/*已写标志0x11*/
	uint8_t subdivision;						/*细分*/
	uint8_t step_angle_multiply10;				/*步距角乘以10*/
	uint16_t ratio_multiply100;					/*导程乘以100*/
	uint16_t max_step;							/*最大行程*/
	uint8_t reset_dir;							/*复位方向*/
	uint8_t reset_status;						/*复位状态*/
	uint8_t d_code_status;						/*码齿状态*/
	uint8_t strike_status;						/*撞击状态*/
	uint8_t reset_compensation;					/*复位补偿*/
	uint16_t start_fre_h;						/*高速启动频率*/
	uint16_t end_fre_h;							/*高速最高频率*/
	uint16_t ech_lader_step_h;					/*高速每台阶微步*/
	uint16_t up_max_lader_h;					/*高速最大加速台阶*/
	uint16_t s_par_h_divide100;					/*高速S型曲线的形状参数除100*/
	uint16_t start_fre_l;						/*低速启动频率*/
	uint16_t end_fre_l;							/*低速最高频率*/
	uint16_t ech_lader_step_l;					/*低速每台阶微步*/
	uint16_t up_max_lader_l;					/*低速最大加速台阶*/
	uint16_t s_par_l_divide100;					/*低速S型曲线的形状参数除100*/
} MCC_PARAMETER1_TYPE;							/*一个组件参数 32Byte*/

typedef struct {	
	uint8_t flag;								/*已写标志0x11*/
	uint8_t en_close_cycle;						/*闭环控制可用标识，1可用，0不可用，2表示未使用*/
	int32_t data_multiply100;					/*坐标值乘以10000，单位°/mm/uL*/
	int32_t forward_pre_deceleration_data_multiply100;	/*正向预减速坐标值乘以10000，单位°/mm/uL*/
	int32_t reverse_pre_deceleration_data_multiply100;	/*反向预减速坐标值乘以10000，单位°/mm/uL*/
	int32_t forward_compensation_multiply100;	/*正向补偿值乘以10000，单位整步*/
	int32_t reverse_compensation_multiply100;	/*反向补偿值乘以10000，单位整步*/
	uint8_t reserve[10];						/*预留*/
} MCC_PARAMETER2_TYPE;							/*一个位置参数 32Byte*/

/* 样本针参数结构定义
  ---------------------------------------*/
typedef struct {
   uint8_t flag;               /*已写标志0x11*/ 
   uint16_t up_inhaled_air_multiply100;      /*吸样上升吸空气体积(uL)乘以100*/
   uint32_t rotate_inhaled_air_multiply100;   /*旋转吸空气体积(uL)乘以100*/ 
   uint32_t reset_wash_time;               /*复位洗针时间(ms)*/ 
   uint32_t default_wash_time;               /*默认洗针时间(ms)*/ 
   uint32_t multi_suction_multiply100;         /*多吸量(uL)*/ 
   uint32_t multi_displacement_multiply100;   /*多排量(uL)*/ 
   uint32_t push_back_quantity_multiply100;   /*回推量(uL)*/ 
   uint8_t reserve[3];         /*预留*/ 
} SAMPLE_NEEDLE_PARAMETER_TYPE;					/*一个样本针参数 32Byte*/

/* 搅拌杆参数结构定义
  ---------------------------------------*/
typedef struct {
	uint8_t flag;								/*已写标志0x11*/
	uint32_t reset_wash_time;					/*复位洗针时间(ms)*/
	uint32_t default_wash_time;					/*默认洗针时间(ms)*/
	uint8_t reserve[23];						/*预留*/
} STIRRING_PARAMETER_TYPE;						/*一个搅拌杆参数 32Byte*/

/* 试剂针参数结构定义
  ---------------------------------------*/
typedef struct {
	uint8_t flag;								/*已写标志0x11*/
	uint32_t up_inhaled_air_after_multiply100;	/*排样后上升吸空气体积(uL)乘以100*/
	uint32_t up_inhaled_air_befor_multiply100;	/*吸样后上升吸空气体积(uL)乘以100*/
	uint32_t rotate_inhaled_air_multiply100;	/*旋转吸空气体积(uL)乘以100*/
	uint32_t reset_wash_time;					/*复位洗针时间(ms)*/
	uint32_t default_wash_time;					/*默认洗针时间(ms)*/
	uint32_t multi_suction_multiply100;			/*多吸量(uL)*/
	uint32_t multi_displacement_multiply100;	/*多排量(uL)*/
	uint8_t reserve[3];							/*预留*/
} REAGENT_NEEDLE_PARAMETER_TYPE;				/*一个试剂针参数 32Byte*/

/* 反应盘温控参数结构定义
  ---------------------------------------*/
typedef struct {
	uint8_t flag;								/*已写标志0x11*/
	uint32_t temperature_multiply100;			/*目标温度乘以100*/
	uint32_t kp_multiply10000;					/*kp乘以10000*/
	uint32_t ki_multiply10000;					/*ki乘以10000*/
	uint32_t kd_multiply10000;					/*kd乘以10000*/
	uint8_t temperature_switch[4];				/*传感器开关*/
	uint8_t reserve[15-4];						/*预留*/
} REACTION_TEMPERATURE_PARAMETER_TYPE;			/*一个反应盘温控参数 32Byte*/

/* 清洗温控参数结构定义
  ---------------------------------------*/
typedef struct {
	uint8_t flag;								/*已写标志0x11*/
	uint32_t temperature_multiply100;			/*目标温度乘以100*/
	uint32_t kp_multiply10000;					/*kp乘以10000*/
	uint32_t ki_multiply10000;					/*ki乘以10000*/
	uint32_t kd_multiply10000;					/*kd乘以10000*/
	uint8_t reserve[15];						/*预留*/
} WATER_TEMPERATURE_PARAMETER_TYPE;				/*一个清洗水温控参数 32Byte*/

/* 测光参数结构定义
  ---------------------------------------*/
typedef struct {
	uint8_t flag;								/*已写标志0x11*/
	uint8_t gains[4];							/*增益*/
	uint8_t reserve[17];						/*预留*/
} DETECTION_PARAMETER_TYPE;						/*测光参数 32Byte*/

/*---------------------------------------------------------------------*/

/* 通用参数接口定义
  ---------------------------------------*/
typedef struct {
	uint16_t dwnmachinetype;					/*下位机号*/
	uint8_t  number;             				/*页码*/
	uint8_t  data_count;						/*数据个数 flag到最后*/
	uint32_t flag;								/*已写标志0x11*/
	uint32_t count;								/*参数个数count之后*/
	uint32_t subdivision;						/*细分*/
	uint32_t step_angle_multiply10;				/*步距角乘以10*/
	uint32_t ratio_multiply100;					/*导程乘以100*/
	uint32_t max_step;							/*最大行程*/
	uint32_t reset_dir;							/*复位方向*/
	uint32_t reset_status;						/*复位状态*/
	uint32_t d_code_status;						/*码齿状态*/
	uint32_t strike_status;						/*撞击状态*/
	uint32_t reset_compensation;				/*复位补偿*/
	uint32_t start_fre_h;						/*高速启动频率*/
	uint32_t end_fre_h;							/*高速最高频率*/
	uint32_t ech_lader_step_h;					/*高速每台阶微步*/
	uint32_t up_max_lader_h;					/*高速最大加速台阶*/
	uint32_t s_par_h_divide100;					/*高速S型曲线的形状参数除100*/
	uint32_t start_fre_l;						/*低速启动频率*/
	uint32_t end_fre_l;							/*低速最高频率*/
	uint32_t ech_lader_step_l;					/*低速每台阶微步*/
	uint32_t up_max_lader_l;					/*低速最大加速台阶*/
	uint32_t s_par_l_divide100;					/*低速S型曲线的形状参数除100*/
} MCC_PARAMETER1_INTERFACE_OUT_TYPE;				/*一个组件参数接口*/

typedef struct {
	uint32_t flag;								/*已写标志0x11*/
	uint32_t count;								/*参数个数count之后*/
	uint32_t subdivision;						/*细分*/
	uint32_t step_angle_multiply10;				/*步距角乘以10*/
	uint32_t ratio_multiply100;					/*导程乘以100*/
	uint32_t max_step;							/*最大行程*/
	uint32_t reset_dir;							/*复位方向*/
	uint32_t reset_status;						/*复位状态*/
	uint32_t d_code_status;						/*码齿状态*/
	uint32_t strike_status;						/*撞击状态*/
	uint32_t reset_compensation;				/*复位补偿*/
	uint32_t start_fre_h;						/*高速启动频率*/
	uint32_t end_fre_h;							/*高速最高频率*/
	uint32_t ech_lader_step_h;					/*高速每台阶微步*/
	uint32_t up_max_lader_h;					/*高速最大加速台阶*/
	uint32_t s_par_h_divide100;					/*高速S型曲线的形状参数除100*/
	uint32_t start_fre_l;						/*低速启动频率*/
	uint32_t end_fre_l;							/*低速最高频率*/
	uint32_t ech_lader_step_l;					/*低速每台阶微步*/
	uint32_t up_max_lader_l;					/*低速最大加速台阶*/
	uint32_t s_par_l_divide100;					/*低速S型曲线的形状参数除100*/
} MCC_PARAMETER1_INTERFACE_IN_TYPE;				/*一个组件参数接口*/

typedef struct {
	uint16_t dwnmachinetype;					/*下位机号*/
	uint8_t  number;             				/*页码*/
	uint8_t  data_count;						/*数据个数 flag到最后*/
	uint32_t flag;								/*已写标志0x11*/
	uint32_t count;								/*参数个数*/
	uint32_t en_close_cycle;					/*闭环控制可用标识，1可用，0不可用，2表示未使用*/
	uint32_t data_multiply100;					/*坐标值乘以10000，单位°/mm/uL*/
	uint32_t forward_pre_deceleration_data_multiply100;	/*正向预减速坐标值乘以10000，单位°/mm/uL*/
	uint32_t reverse_pre_deceleration_data_multiply100;	/*反向预减速坐标值乘以10000，单位°/mm/uL*/
	uint32_t forward_compensation_multiply100;	/*正向补偿值乘以10000，单位整步*/
	uint32_t reverse_compensation_multiply100;	/*反向补偿值乘以10000，单位整步*/
} MCC_PARAMETER2_INTERFACE_OUT_TYPE;				/*一个位置参数接口*/

typedef struct {
	uint32_t flag;								/*已写标志0x11*/
	uint32_t count;								/*参数个数*/
	uint32_t en_close_cycle;					/*闭环控制可用标识，1可用，0不可用，2表示未使用*/
	uint32_t data_multiply100;					/*坐标值乘以10000，单位°/mm/uL*/
	uint32_t forward_pre_deceleration_data_multiply100;	/*正向预减速坐标值乘以10000，单位°/mm/uL*/
	uint32_t reverse_pre_deceleration_data_multiply100;	/*反向预减速坐标值乘以10000，单位°/mm/uL*/
	uint32_t forward_compensation_multiply100;	/*正向补偿值乘以10000，单位整步*/
	uint32_t reverse_compensation_multiply100;	/*反向补偿值乘以10000，单位整步*/
} MCC_PARAMETER2_INTERFACE_IN_TYPE;				/*一个位置参数接口*/

/* 样本针参数接口定义
  ---------------------------------------*/
typedef struct {
	uint16_t dwnmachinetype;					/*下位机号*/
	uint8_t  number;             				/*页码*/
	uint8_t  data_count;						/*数据个数 flag到最后*/
	uint32_t flag;								/*已写标志0x11*/
	uint32_t count;								/*参数个数*/
	uint32_t up_inhaled_air_multiply100;		/*上升吸空气体积(uL)乘以100*/
	uint32_t rotate_inhaled_air_multiply100;	/*旋转吸空气体积(uL)乘以100*/
	uint32_t reset_wash_time;					/*复位洗针时间(ms)*/
	uint32_t default_wash_time;					/*默认洗针时间(ms)*/
	uint32_t multi_suction_multiply100;			/*多吸量(uL)*/
	uint32_t multi_displacement_multiply100;	/*多排量(uL)*/
	uint32_t push_back_quantity_multiply100;	/*回推量(uL)*/
} SAMPLE_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE;		/*一个样本针参数*/

typedef struct {
	uint32_t flag;								/*已写标志0x11*/
	uint32_t count;								/*参数个数*/
	uint32_t up_inhaled_air_multiply100;		/*上升吸空气体积(uL)乘以100*/
	uint32_t rotate_inhaled_air_multiply100;	/*旋转吸空气体积(uL)乘以100*/
	uint32_t reset_wash_time;					/*复位洗针时间(ms)*/
	uint32_t default_wash_time;					/*默认洗针时间(ms)*/
	uint32_t multi_suction_multiply100;			/*多吸量(uL)*/
	uint32_t multi_displacement_multiply100;	/*多排量(uL)*/
	uint32_t push_back_quantity_multiply100;	/*回推量(uL)*/
} SAMPLE_NEEDLE_PARAMETER_INTERFACE_IN_TYPE;		/*一个样本针参数*/

/* 搅拌杆参数接口定义
  ---------------------------------------*/
typedef struct {
	uint16_t dwnmachinetype;					/*下位机号*/
	uint8_t  number;             				/*页码*/
	uint8_t  data_count;						/*数据个数 flag到最后*/
	uint32_t flag;								/*已写标志0x11*/
	uint32_t count;								/*参数个数*/
	uint32_t reset_wash_time;					/*复位洗针时间(ms)*/
	uint32_t default_wash_time;					/*默认洗针时间(ms)*/
} STIRRING_PARAMETER_INTERFACE_OUT_TYPE;			/*一个搅拌杆参数*/

typedef struct {
	uint32_t flag;								/*已写标志0x11*/
	uint32_t count;								/*参数个数*/
	uint32_t reset_wash_time;					/*复位洗针时间(ms)*/
	uint32_t default_wash_time;					/*默认洗针时间(ms)*/
} STIRRING_PARAMETER_INTERFACE_IN_TYPE;			/*一个搅拌杆参数*/

/* 试剂针参数接口定义
  ---------------------------------------*/
typedef struct {
	uint16_t dwnmachinetype;					/*下位机号*/
	uint8_t  number;             				/*页码*/
	uint8_t  data_count;						/*数据个数 flag到最后*/
	uint32_t flag;								/*已写标志0x11*/
	uint32_t count;								/*参数个数*/
	uint32_t up_inhaled_air_after_multiply100;	/*排样后上升吸空气体积(uL)乘以100*/
	uint32_t up_inhaled_air_befor_multiply100;	/*吸样后上升吸空气体积(uL)乘以100*/
	uint32_t rotate_inhaled_air_multiply100;	/*旋转吸空气体积(uL)乘以100*/
	uint32_t reset_wash_time;					/*复位洗针时间(ms)*/
	uint32_t default_wash_time;					/*默认洗针时间(ms)*/
	uint32_t multi_suction_multiply100;			/*多吸量(uL)*/
	uint32_t multi_displacement_multiply100;	/*多排量(uL)*/
} REAGENT_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE;		/*一个试剂针参数*/

typedef struct {
	uint32_t flag;								/*已写标志0x11*/
	uint32_t count;								/*参数个数*/
	uint32_t up_inhaled_air_after_multiply100;	/*排样后上升吸空气体积(uL)乘以100*/
	uint32_t up_inhaled_air_befor_multiply100;	/*吸样后上升吸空气体积(uL)乘以100*/
	uint32_t rotate_inhaled_air_multiply100;	/*旋转吸空气体积(uL)乘以100*/
	uint32_t reset_wash_time;					/*复位洗针时间(ms)*/
	uint32_t default_wash_time;					/*默认洗针时间(ms)*/
	uint32_t multi_suction_multiply100;			/*多吸量(uL)*/
	uint32_t multi_displacement_multiply100;	/*多排量(uL)*/
} REAGENT_NEEDLE_PARAMETER_INTERFACE_IN_TYPE;		/*一个试剂针参数*/

/* 反应盘温控参数接口定义
  ---------------------------------------*/
typedef struct {
	uint16_t dwnmachinetype;					/*下位机号*/
	uint8_t  number;             				/*页码*/
	uint8_t  data_count;						/*数据个数 flag到最后*/
	uint32_t flag;								/*已写标志0x11*/
	uint32_t count;								/*参数个数*/
	uint32_t temperature_multiply100;			/*目标温度乘以100*/
	uint32_t kp_multiply10000;					/*kp乘以10000*/
	uint32_t ki_multiply10000;					/*ki乘以10000*/
	uint32_t kd_multiply10000;					/*kd乘以10000*/
	uint32_t temperature_switch[4];				/*传感器开关*/
} REACTION_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE;	/*一个反应盘温控参数*/

typedef struct {
	uint32_t flag;								/*已写标志0x11*/
	uint32_t count;								/*参数个数*/
	uint32_t temperature_multiply100;			/*目标温度乘以100*/
	uint32_t kp_multiply10000;					/*kp乘以10000*/
	uint32_t ki_multiply10000;					/*ki乘以10000*/
	uint32_t kd_multiply10000;					/*kd乘以10000*/
	uint32_t temperature_switch[4];				/*传感器开关*/
} REACTION_TEMPERATURE_PARAMETER_INTERFACE_IN_TYPE;	/*一个反应盘温控参数*/

/* 清洗温控参数接口定义
  ---------------------------------------*/
typedef struct {
	uint16_t dwnmachinetype;					/*下位机号*/
	uint8_t  number;             				/*页码*/
	uint8_t  data_count;						/*数据个数 flag到最后*/
	uint32_t flag;								/*已写标志0x11*/
	uint32_t count;								/*参数个数*/
	uint32_t temperature_multiply100;			/*目标温度乘以100*/
	uint32_t kp_multiply10000;					/*kp乘以10000*/
	uint32_t ki_multiply10000;					/*ki乘以10000*/
	uint32_t kd_multiply10000;					/*kd乘以10000*/
} WATER_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE;	/*一个清洗水温控参数*/

typedef struct {
	uint32_t flag;								/*已写标志0x11*/
	uint32_t count;								/*参数个数*/
	uint32_t temperature_multiply100;			/*目标温度乘以100*/
	uint32_t kp_multiply10000;					/*kp乘以10000*/
	uint32_t ki_multiply10000;					/*ki乘以10000*/
	uint32_t kd_multiply10000;					/*kd乘以10000*/
} WATER_TEMPERATURE_PARAMETER_INTERFACE_IN_TYPE;	/*一个清洗水温控参数*/

/* 测光参数接口定义
  ---------------------------------------*/
typedef struct {
	uint16_t dwnmachinetype;					/*下位机号*/
	uint8_t  number;             				/*页码*/
	uint8_t  data_count;						/*数据个数 flag到最后*/
	uint32_t flag;								/*已写标志0x11*/
	uint32_t count;								/*参数个数*/
	uint32_t gains[4];							/*增益*/
} DETECTION_PARAMETER_INTERFACE_OUT_TYPE;			/*测光参数 32Byte*/

typedef struct {
	uint32_t flag;								/*已写标志0x11*/
	uint32_t count;								/*参数个数*/
	uint32_t gains[4];							/*增益*/
} DETECTION_PARAMETER_INTERFACE_IN_TYPE;			/*测光参数 32Byte*/

/* 通用头帧结构定义
  ---------------------------------------*/
typedef struct {
	uint8_t  serial_number  :7;	/*流水号*/
	uint8_t  end_frame_flag :1;	/*结束帧标志位*/
	uint16_t cmd;				/*命令字*/
	uint8_t  reserved_1[2];		/*预留*/
	uint8_t  id;				/*CAN ID*/
	uint8_t  type;				/*命令类型*/
	uint16_t len;				/*数据长度*/
	uint32_t frame_id;			/*帧id*/
	uint8_t  data[1];			/*数据域*/
} CMD_COMMON_HEAD_TYPE;

/* 调试命令数据域结构定义
  ---------------------------------------*/
/*握手*/
typedef struct {
	uint16_t cmd;				/*命令字*/
} CMD_COMMON_DATA_HELLO_TYPE;
typedef struct {
	uint16_t cmd;				/*命令字*/
} CMD_COMMON_DATA_RETURN_HELLO_TYPE;

/*读位置*/
typedef struct {
	uint16_t cmd;				/*命令字*/
	uint8_t  mcc_number;		/*组件编号*/
	uint8_t  expression_form;	/*表达方式*/
} CMD_COMMON_DATA_READ_COORDINATE_TYPE;
typedef struct {
	uint16_t cmd;				/*命令字*/
	int32_t coordinate;			/*坐标值（带符号）*/
} CMD_COMMON_DATA_RETURN_READ_COORDINATE_TYPE;

/*组件复位*/
typedef struct {
	uint16_t cmd;				/*命令字*/
	uint8_t  mcc_number;		/*组件编号*/
} CMD_COMMON_DATA_MCC_RESET_TYPE;
typedef struct {
	uint16_t cmd;				/*命令字*/
} CMD_COMMON_DATA_RETURN_MCC_RESET_TYPE;

/*组件运动控制*/
typedef struct {
	uint16_t cmd;				/*命令字*/
	uint8_t  mcc_number;		/*组件编号*/
	uint8_t  motion_type;		/*运动方式*/
	uint8_t  dir;				/*方向*/
	uint32_t value;				/*数值*/
} CMD_COMMON_DATA_MCC_MOVE_TYPE;
typedef struct {
	uint16_t cmd;				/*命令字*/
} CMD_COMMON_DATA_RETURN_MCC_MOVE_TYPE;

/*电磁阀控制*/
typedef struct {
	uint16_t cmd;				/*命令字*/
	uint8_t  valve_number;		/*电磁阀编号*/
	uint8_t  status;			/*开关状态*/
} CMD_COMMON_DATA_VALVE_CONTROL_TYPE;
typedef struct {
	uint16_t cmd;				/*命令字*/
} CMD_COMMON_DATA_RETURN_VALVE_CONTROL_TYPE;

/*读取信号*/
typedef struct {
	uint16_t cmd;				/*命令字*/
	uint8_t  signal_number;		/*信号编号*/
} CMD_COMMON_DATA_READ_SIGNAL_TYPE;
typedef struct {
	uint16_t cmd;				/*命令字*/
	uint16_t signal_status;		/*信号状态*/
} CMD_COMMON_DATA_RETURN_READ_SIGNAL_TYPE;

/*泵/直流电机控制*/
typedef struct {
	uint16_t cmd;				/*命令字*/
	uint8_t  pump_number;		/*电机编号*/
	uint8_t  status;			/*启停状态*/
	uint8_t  duty_cycle;		/*占空比*/
	uint8_t  type;				/*控制方式*/
	uint16_t fre;				/*频率（单位kHz）*/
} CMD_COMMON_DATA_PUMP_CONTROL_TYPE;
typedef struct {
	uint16_t cmd;				/*命令字*/
} CMD_COMMON_DATA_RETURN_PUMP_CONTROL_TYPE;

/*单个参数读写*/
typedef struct {
	uint16_t cmd;				/*命令字*/
	uint8_t  mcc_number;		/*组件编号*/
	uint8_t  rw;				/*读写选择*/
	uint8_t  data_type;			/*参数类型*/
	uint8_t  data_number1;		/*参数编号1*/
	uint8_t  data_number2;		/*参数编号2*/
	uint32_t data;				/*参数值*/
} CMD_COMMON_DATA_RW_DATA_TYPE;
typedef struct {
	uint16_t cmd;				/*命令字*/
	uint32_t data;				/*参数值*/
} CMD_COMMON_DATA_RETURN_RW_DATA_TYPE;

typedef struct {
	uint16_t cmd;				/*命令字*/
	uint8_t  number;			/*页号*/
	uint8_t  rw;				/*读写选择*/
	uint8_t  datas[100];		/*参数值*/
} CMD_COMMON_DATA_RW_PAGE_DATA_TYPE;

#pragma pack()

/* 通用命令字定义
  ---------------------------------------*/
  
typedef enum {
	CMD_COM_HELLO			= 0x0001,		/*握手*/
	CMD_READ_MCC_POSITION	= 0x0002,		/*读取位置*/
	CMD_COM_MCC_RESET		= 0x0003,		/*组件复位*/
	CMD_COM_MCC_MOVE		= 0x0004,		/*组件运动控制*/
	CMD_COM_VALVE_CONTROL	= 0x0005,		/*电磁阀/电磁铁控制*/
	CMD_COM_READPIN			= 0x0006,		/*读取信号*/
	CMD_COM_PUMP_CONTROL	= 0x0007,		/*泵/直流电机控制*/
	CMD_COM_RW_PARAMETER	= 0x0008,		/*单个参数读写*/
	CMD_COM_RW_PARAMETER_PAGE	= 0x0009,		/*整体参数读写*/
} CMD_COMMON_ENUM;

extern DRV_AT24C32_TYPE* _gp_EEP;

/* 初始化函数
 ********************************************************/

/* 初始化 Func_Common
  ------------------------------
  入口：指令注册表，模块编号
  返回值：true，成功；false，失败
*/
extern bool Func_Common_Init(DList* dlist, uint8_t ch);

/* 模块1私有初始化（由各模块Func文件实现）
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_PrivateInit1(DList* dlist, DRV_MCC_TYPE** m, DRV_VALVE_TYPE** v, DRV_PUMP_TYPE** p);

/* 模块2私有初始化（由各模块Func文件实现）
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_PrivateInit2(DList* dlist, DRV_MCC_TYPE** m, DRV_VALVE_TYPE** v, DRV_PUMP_TYPE** p);

/* 通用命令任务处理函数
 ********************************************************/
 
/* 握手
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Cmd_Com_Hello(void* p_buffer);

/* 读位置
  ------------------------------ ok
  返回值：true，成功；false，失败
*/
extern bool Func_Cmd_Com_Read_Mcc_Position(void* p_buffer);

/* 组件复位
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Cmd_Com_Mcc_Reset(void* p_buffer);

/* 组件移动
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Cmd_Com_Mcc_Move(void* p_buffer);

/* 电磁阀/电磁铁控制
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Cmd_Com_Valve_Control(void* p_buffer);

/* 读取信号
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Cmd_Com_ReadPin(void* p_buffer);

/* 泵/直流电机控制
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Cmd_Com_Pump_Control(void* p_buffer);

/* 通用参数读写
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Cmd_Com_RW_Parameter(void* p_buffer);

/* 一页参数读写
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Cmd_Com_RW_Page_Parameter(void* p_buffer);

/* 通用常驻任务处理函数
 ********************************************************/
 
#endif
