/*  File: Common_Cmd.h
    Discription: Define command words and command 
    format structure between upper machine and 
    lower machine 
    Author: HYC
    Date: 22/04/01
    Version: V1.0
    Release note: First edition
*/

#ifndef _COMMON_CMD_H_
#define _COMMON_CMD_H_

#include "stdint.h"
#include "stdlib.h"
#include "Drv_UartDataInterface.h"
#include "ucos_ii.h"
#include "Drv_MotionControlComponents.h"
#include "Drv_Valve.h"
#include "Drv_Pump.h"
#include "Drv_Led.h"
#include "Drv_Fan.h"
#include "Drv_Heat.h"
#include "Drv_SwitchingSensor.h"
#include "Drv_Refrigerating.h"
#include "Drv_NtcTemp.h"
#include "Drv_X100m.h"
#include "Prt_CmdDoubleList.h"  
#include "Drv_AT24C32.h"
#include "BSP_Sys.h"

#pragma pack()



//命令字定义
typedef enum{
    CMD_MIN = 0x0000,               //命令字最小值限定    
    CMD_HANDSHAKE = 0x0001,         //握手
    CMD_DCPUMPCONTROL = 0x0002,     //泵控制指令
    CMD_STEPPUMPCONTROL = 0x0003,   //蠕动泵控制
    CMD_VALVECONTROL = 0x0004,      //阀控制指令
    CMD_FANCONTROL = 0x0005,        //风扇控制
    CMD_COLDING = 0x0006,           //制冷控制
    CMD_HEATING = 0x0007,           //加热控制
    CMD_READLIQUID = 0x0008,        //读液位信号
    CMD_READTEMP = 0x0009,          //读取温度值
    CMD_LEDCONTROL = 0x000A,        //设置面板指示灯
		CMD_FIBERPARA_RW = 0x000B,			//X100传感器参数读写
		CMD_INQUERYVERSION = 0x000C,		//下位机版本查询
		CMD_TEMP_PARA_RW = 0x0011,			//温控参数读写
		CMD_LIQUID_FILL = 0x0012,				//容器灌注
		CMD_LIQUID_NULL = 0x0013,				//容器排空
		CMD_STEMPUMP_CONTROL,						//蠕动泵开关控制
		CMD_TEMP_REPORTSWITCH,					//温度主动上报开关
		CMD_FREEZER_CURRENT_INQUIRY,		//制冷片电流查询
    CMD_MAX = 0x00FF,               //命令字最大值限定
} CMD_ENUM;

//报错码定义
typedef enum{
    ERR_MIN = 0xFF00,               //报警码最小值限定    
		ERR_CMD_NULL = 0xFF01,					//未识别指令或指令解析失败
		ERR_CMD_FAIL = 0xFF02,					//命令执行失败
		ERR_TEMP1 = 0xFF03,					//制冷液温度传感器异常或未连接
		ERR_TEMP2 = 0xFF04,					//冷板1温度传感器异常或未连接
		ERR_TEMP3 = 0xFF05,					//冷板2温度传感器异常或未连接
		ERR_SENSOR1 = 0xFF0A,					//膀胱温度传感器异常或未连接
		ERR_SENSOR2 = 0xFF0B,					//喉管温度传感器异常或未连接
		ERR_SENSOR3 = 0xFF0C,					//鼻腔温度传感器异常或未连接
		ERR_SENSOR4 = 0xFF0D,					//球囊温度传感器异常或未连接
		ERR_TEMP6 = 0xFF06,					//制冷液温度低于10°
		ERR_TEMP5 = 0xFF07,					//冷板1温度过低
		ERR_TEMP4	= 0xFF08,					//冷板2温度过低
		ERR_CLODX_ERR = 0xFF09,			//制冷片x工作电流异常
		ERR_LIQUID_SENSOR = 0xFF0E,		//液位开关状态异常
		ERR_LIQUID_INIT_ERR = 0xFF0F,	//液位初始化失败
    ERR_MAX = 0xFFFF,               //报警码最大值限定
} ERR_ENUM;

//事件码定义
typedef enum{
    EVENT_MIN = 0x1000,               //命令字最小值限定    
		EVENT_SS1_LEFT = 0x1001,						//旋钮1左旋一格事件
		EVENT_SS1_RIGHT = 0x1002,						//旋钮1右旋一格事件
		EVENT_SS1_BUTTON = 0x1003,					//旋钮1按钮按下事件
		EVENT_SS2_LEFT = 0x1004,						//旋钮2左旋一格事件
		EVENT_SS2_RIGHT = 0x1005,						//旋钮2右旋一格事件
		EVENT_SS2_BUTTON = 0x1006,					//旋钮2按钮按下事件
		EVENT_LIQUID_SENSOR_L = 0x1007,			//液位传感器低液位触发事件
		EVENT_LIQUID_SENSOR_H = 0x1008,			//液位传感器高液位触发事件
		EVENT_INIT_SUCCESS,									//初始化完成
		EVENT_TEMP_REPORT,									//温度值主动上报
    EVENT_MAX = 0x10FF,               //命令字最大值限定
} EVENT_ENUM;

typedef enum{
    MODULE_MIN = 0x00,
    MODULE_DCPUMP,
    MODULE_STEPPUMP,
    MODULE_VALVE,
    MODULE_FAN,
    MODULE_COLDING,
    MODULE_HEATING,
    MODULE_LIQUID,
    MODULE_TEMPSENSOR,
		MODULE_TEMPX100,
    MODULE_LED,
		MODULE_TEMP_CONTROL,
		MODULE_LIQUID_CONTROL,
    MODULE_MAX,
} MODULE_ENUM;
//直流泵枚举
typedef enum{
    DCPUMP1 = 0x00,                 //直流泵1
    DCPUMP2,                //直流泵2
		DCPUMP3,
    DCPUMP_MAX,              //直流泵编号最大值
} DCPUMP_ENUM;
//阀枚举
typedef enum{
    VALVE1 = 0x00,                 //阀1
    VALVE2,                //阀2
    VALVE_MAX,              //阀编号最大值
} VALVE_ENUM;

//蠕动泵枚举
typedef enum{
    STEP_PUMP1 = 0x00,                 //蠕动泵1
    STEP_PUMP2,               	 //蠕动泵2
    STEP_PUMP_MAX,              //蠕动泵编号最大值
} STEP_PUMP_ENUM;
//风扇枚举
typedef enum{
    COLD_FAN1 = 0x00,               //制冷片1散热风扇
    COLD_FAN2,               //制冷片2散热风扇
    COLD_FAN3,               //制冷片3散热风扇
    COLD_FAN4,               //制冷片4散热风扇
    BOARD_FAN,               //板卡散热风扇
    FAN_MAX,                 //风扇编号最大值
} FAN_ENUM;
//制冷片编号枚举
typedef enum{
    COLD1 = 0x00,                   //制冷片1
    COLD2,                   //制冷片2
    COLD3,                   //制冷片3
    COLD4,                   //制冷片4
    COLD_MAX,                //制冷片编号最大值
} COLD_ENUM;
//加热膜编号枚举
typedef enum{
    HEAT1 = 0x00,
    HEAT2,
    HEAT_MAX,
} HEAT_ENUM;
//液位开关编号枚举
typedef enum{
    LIQUIDE1 = 0x00,
    LIQUIDE2,
    LIQUIDE_MAX,
} LIQUIDE_ENUM;
//温度传感器枚举
typedef enum{
    TEMP_SENSOR1 = 0x00, //NTC1
    TEMP_SENSOR2,        //NTC2
    TEMP_SENSOR3,        //NTC3
    TEMP_SENSOR4,        //NTC4
    TEMP_SENSOR5,        //NTC5
    TEMP_SENSOR6,        //NTC6
    TEMP_SENSOR_MAX,     //最大值限定
} TEMP_SENSOR_ENUM;
//光纤传感器枚举
typedef enum{
		FIBER1 = 0x00,
		FIBER2,
		FIBER_MAX,
} FIBER_ENUM;
//LED灯状态枚举
typedef enum{
    LED_RED = 0x00,
    LED_GREEN,
    LED_YELLOW,
    LED_MAX,
} LED_ENUM;
//旋钮信号枚举
typedef enum{
		SS1_BUTTON,
		SS2_BUTTON,
		SS_MAX,
}	SS_ENUM;

#pragma pack(4)
/*********************下发数据域定义**/

//握手命令结构体
typedef struct{
    uint16_t cmd;               //命令字
    uint8_t reverse[2];         //预留字段
} HANDSHAKE_TYPE;
//泵控制指令结构体
typedef struct{
    uint16_t cmd;               //命令字
    uint8_t pump_num;           //泵编号    DCPUMP1~DCPUMP_MAX,MAX表示选中所有泵
    uint8_t on_off;             //开关控制，0:off,1:on
} DCPUMPCONTROL_TYPE;
//蠕动泵控制指令结构体
typedef struct{
    uint16_t cmd;               //命令字
    uint16_t speed_start;             //速度pps
		uint16_t speed_end;
		uint16_t echlader_up;
		uint16_t lader;
		uint16_t s_para;
    uint32_t volumn;            //吸样体积(0.1ul)
} STEPPUMPCONTROL_TYPE;
//阀控制指令结构体
typedef struct{
    uint16_t cmd;               //命令字
    uint8_t valve_num;          //阀编号    
    uint8_t on_off;             //开关控制，0:off,1:on
} VALVECONTROL_TYPE;
//风扇控制结构体
typedef struct{
    uint16_t cmd;               //命令字
    uint8_t fan_num;            //风扇编号
    uint8_t on_off;             //开关控制
} FANCONTROL_TYPE;
//制冷片控制结构体
typedef struct{
    uint16_t cmd;               //命令字
    uint8_t cold_num;           //制冷片编号
    uint8_t on_off;             //开关控制
} COLDING_TYPE;
//加热膜控制结构体
typedef struct{
    uint16_t cmd;               //命令字
    uint8_t heat_num;           //加热膜编号
    uint8_t on_off;             //开关控制
} HEATING_TYPE;
//读取液位开关状态
typedef struct{
    uint16_t cmd;               //命令字
    uint16_t liquid_num;        //液位开关编号
} LIQUIDE_TYPE;
//读取温度值
typedef struct{
    uint16_t cmd;               //命令字
    uint16_t temp_ch;           //传感器编号
} TEMP_TYPE;
//设置LED灯
typedef struct{
    uint16_t cmd;               //命令字
    uint16_t status;            //LED灯状态
} LED_TYPE;
//温控模式参数读写（下发回复通用数据域）
typedef struct{
	uint16_t cmd;
	uint8_t rw;											/*0：设置参数，1：读取参数*/
	uint8_t pid_switch;							/*0：禁止PID控制，1开启PID控制*/
	uint8_t mode;											/*0：制冷，1：加热*/
	uint8_t speed;										/*0：关闭温控，非0：对应工作的加热膜/制冷片最大数量*/
	uint16_t goal_ch;									//目标温度通道，0~6,0为光纤传感器，其他为NTC1~6
	uint32_t goal_temp_multiply100;								/*目标温度值0.01°*/
} CMD_TEMP_DATA;
//X100参数读写(下发回复通用数据域)
typedef struct{
		uint16_t cmd;
		uint8_t rw;				//0:write, 1:read
		long long a_int;
		long long b_int;
		long long c_int;
		long long d_int;
		long long e_int;
		uint8_t a_index;	//小数位数10^-x
		uint8_t b_index;	//小数位数10^-x
		uint8_t c_index;
		uint8_t d_index;
		uint8_t e_index;
		long long a_frac;
		long long b_frac;
		long long c_frac;
		long long d_frac;
		long long e_frac;
} X100_PARA_RW;

//X100参数读写(下发回复通用数据域)
typedef struct{
		uint16_t cmd;
		uint16_t rw;				//0:write, 1:read
		float a;
		float	b;
		float c;
		float d;
		float e;
} FIBERX100_PARA_RW;
//液路灌注
typedef struct{
	uint16_t cmd;
	uint8_t rsv[2];
} CMD_LIQUIDE_FILL_DATA;
//液路回流
typedef struct{
	uint16_t cmd;
	uint8_t rsv[2];
} CMD_LIQUIDE_NULL_DATA;
//蠕动泵开关控制
typedef struct{
	uint16_t cmd;
	uint16_t on_off;			//0：关闭，1：开启
} CMD_STEPPUMP_SWITCH_DATA;
//温度上报开关设置
typedef struct{
	uint16_t cmd;
	uint16_t on_off;			//0：关闭，1：开启
} CMD_TEMP_REPORT_SWITCH_DATA;
//制冷片电流从查询
//液路回流
typedef struct{
	uint16_t cmd;
	uint8_t rsv[2];
} CMD_CURRENT_INQUIRY_DATA;
/***************************end***/


/**************************上报结构域定义********/
//通用回复格式
typedef struct 
{
    uint16_t cmd;
    uint8_t rsv[2];               //占位
	
}COMMON_RETURN_DATA_TYPE;
//回复错误格式
typedef struct 
{
    uint16_t err_code;
    uint8_t module_id;
    uint8_t device_id;

}RETURN_ERR_DATA_TYPE;

//上报事件数据结构
typedef struct 
{
    uint16_t event_id;
    uint8_t rsv[2];         //字节对齐占位
}REPORT_EVENT_DATA_TYPE;
//读取液位状态回复格式
typedef struct 
{
    uint16_t cmd;
    uint8_t sensor_num;         //传感器编号
    uint8_t status;             //0:液位低，1:液位高
}LIQUIDE_RETURN_DATA_TYPE;
//读取温度回复格式
typedef struct 
{
    uint16_t cmd;
    uint16_t temp_ch;           //传感器编号
    uint32_t temp_value;        //温度值(0.01°)
}TEMP_RETURN_DATA_TYPE;
//温度上报格式,温度值单位0.01（乘以100）
typedef struct{
		uint16_t event_id;
		uint32_t temp[7];						//0:X100，1~6：NTC1~6
		uint8_t rsv[2];							//预留
}TEMP_REPORT_DATA_TYPE;

//版本查询
typedef struct 
{
    uint16_t cmd;
	  uint8_t date[32];	
		uint8_t version[22];
}VERSION_RETURN;

//制冷片电流回复格式
typedef struct{
	uint16_t cmd;
	uint16_t current[4];
	uint8_t rsv[2];
}FREEZER_CURRENT_RETURN_TYPE;
/**************************end****************/

#pragma pack()
extern DRV_MCC_TYPE*   _gp_StepPump; /*蠕动泵步进电机*/
extern DRV_VALVE_TYPE* _gp_V;	/*电磁阀/电磁铁*/
extern DRV_FAN_TYPE*	_gp_Fan[FAN_MAX];	/*风扇*/
extern DRV_PUMP_TYPE*  _gp_P[DCPUMP_MAX];	/*泵/直流电机*/
extern DRV_HEAT_TYPE* _gp_H[HEAT_MAX];		/*加热膜*/
extern DRV_REFRIGERATING_TYPE* _gp_C[COLD_MAX];	/*制冷片*/
extern DRV_NTC_TYPE*	_gp_N[TEMP_SENSOR_MAX];		/*NTC温度传感器*/
extern DRV_SWITCHINGSENSOR_TYPE* _gp_S[LIQUIDE_MAX];		/*液位传感器*/
extern DRV_X100M_TYPE* _gp_X;								/*光纤温度传感器*/
extern DRV_SWITCHINGSENSOR_TYPE* _gp_B[SS_MAX];	/*旋钮传感器*/
extern DRV_AT24C32_TYPE* _gp_E;	/*参数存储器*/
extern DRV_LED_TYPE* _gp_LED[LED_MAX];/*三色灯*/
/*指令注册函数*/
extern void Func_Common_Init(DList* dlist);
/*私有初始化函数1*/
extern bool Func_PrivateInit1(DList* dlist);
/*私有初始化函数2*/
/* 通用命令任务处理函数
 ********************************************************/
 
/* 握手
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Cmd_Com_Hello(void* p_buffer);

/* 泵/直流电机控制
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Cmd_Com_Pump_Control(void* p_buffer);

/* 步进电机控制
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Cmd_Com_StepPump_Control(void* p_buffer);

/* 阀控制指令
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Cmd_Com_Valve_Control(void* p_buffer);

/* 风扇控制指令
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Cmd_Com_Fan_Control(void* p_buffer);

/* 制冷片控制指令
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Cmd_Com_Freezer_Control(void* p_buffer);

/* 加热膜控制指令
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Cmd_Com_Heater_Control(void* p_buffer);

/* 读液位传感器指令
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Cmd_Com_Liquid_Read(void* p_buffer);

/* 读温度传感器指令
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Cmd_Com_Temp_Read(void* p_buffer);

/* 台面指示灯控制指令
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Cmd_Com_Led_Control(void* p_buffer);

/* 光纤传感器参数读写指令
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_X100_Para_RW(void* p_buffer);

/* 版本查询指令
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Cmd_Com_Inquiry_Version(void* p_buffer);

/* 温控参数读写指令
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Temp_Para_RW(void* p_buffer);

/* 制冷液灌注
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Liquid_Fill(void* p_buffer);

/* 制冷液回流
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Liquid_Back(void* p_buffer);

/* 蠕动泵开关控制
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Step_Pump_Switch(void* p_buffer);

/* 温度上报开关
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Temp_Report_Switch(void* p_buffer);

/* 制冷片电流查询
  ------------------------------
  返回值：true，成功；false，失败
*/
extern bool Func_Freezer_Current_Inquiry(void* p_buffer);
#endif
