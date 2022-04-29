#ifndef _FUNC_TEMP_CONTROL_H_
#define _FUNC_TEMP_CONTROL_H_

#include "Drv_Pid.h"
#include "Common_Cmd.h"
#include "Prt_CmdRegedit.h"
#include "math.h"
#include "Drv_Encoder.h"


#pragma pack(4)
/*数据域-----------------------*/

/*end---------------------------*/



/* 控制参数结构体定义
	------------------------------
*/
#pragma pack()
typedef struct{
	uint8_t flag;					//写入eeprom标志
	uint8_t pid_switch;	
	uint8_t mode;					//当前模式，0：制冷，1:加热
	uint8_t speed;				//效率档位
	uint16_t current_ch;		//当前目标温度通道
	uint32_t goal_temp_multiply100;		//0.01°
} TEMP_PARA_TYPE;

typedef struct{
	uint8_t flag;
	long long x_int;
	long long x_frac;
	uint8_t x_index;
} X100_PARAMETER;

/*X100mÄ£¿éÎÂ¶ÈÏµÊý*/
typedef struct
{
	uint32_t flag;
	float a;
	float b;
	float c;
	float d;
	float e;
}FIBERX100M_PARA_TYPE;

typedef struct{
	uint8_t flag;
	uint32_t start_fre;
	uint32_t end_fre;
	uint32_t each_lader_step;
	uint32_t lader_num;
	uint32_t s_para;
} MOTOR_SPEED_CUREV;

typedef enum{
	LIQUID_NULL=0,
	LIQUID_FILL,
	LIQUID_BACK,
	LIQUID_ERR,
}LIQUID_STATUS_ENUM;

typedef enum{
	UP = 0,
	DOWN,
}LIQUID_SENSOR_ENUM;

typedef union{
	uint32_t ch4;
	float fl;
} PARA_CONVERT_UNION;
/* 初始化模块
  ------------------------------
  返回值：true，成功；false，失败
*/
static bool Func_TempInit(void);

/*制冷片通断控制*/
static void _Func_FreezerON(void);

static void _Func_FreezerOFF(void);
/*加热膜通断控制*/
static void _Func_HeaterON(void);

static void _Func_HeaterOFF(void);

#endif
