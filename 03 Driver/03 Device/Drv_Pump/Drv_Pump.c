/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——泵模块

  -------------------------------------------------------
   文 件 名   : Drv_Pump.c
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

#include "Drv_Pump.h"

DRV_PUMP_TYPE* _gp_PumpUnits[PUMP_BUTT] = {0};	/*泵的注册表*/

/* 接口函数
 ********************************************************/

/* 初始化泵模块
  -----------------------------
  入口：泵的ID号，不可调速/电源调速/信号调速，正/负逻辑，调速频率，调速占空比，IO端口
  返回值：初始化好的电磁阀模块结构体
*/
DRV_PUMP_TYPE* PumpInit(DRV_PUMP_ENUM id, \
						  DRV_PUMP_TYPE_ENUM pump_type, \
						  DRV_PUMP_LOGIC_TYPE_ENUM pump_logic, \
						  uint32_t fre, \
						  uint8_t duty_cycle, \
						  BSP_TIMER_ENUM timer_number, \
						  BSP_PORT_ENUM power_port_number, BSP_PIN_ENUM power_pin_number, \
						  BSP_PORT_ENUM signal_port_number, BSP_PIN_ENUM signal_pin_number)
{	
	DRV_PUMP_TYPE* p_unit = (DRV_PUMP_TYPE*)calloc(1, sizeof(DRV_PUMP_TYPE));	/*申请内存*/
	
	/*分配失败返回NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id错误返回NULL*/
	if(id >= PUMP_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*如果当前模块存在，取消分配新存储区*/
	if(_gp_PumpUnits[id] != NULL)
	{
		free(p_unit);
		p_unit = _gp_PumpUnits[id];
	}
	else
	{
		_gp_PumpUnits[id] = p_unit;
	}
	
	/*初始化*/
	
	/*-------------------------------常量*/
	p_unit->id          = id;						/*模块id*/
	p_unit->type        = pump_type;				/*常开/常闭*/
	p_unit->logic_type  = pump_logic;				/*正/负逻辑*/
	p_unit->fre         = fre;						/*频率*/
	p_unit->duty_cycle  = duty_cycle;				/*占空比*/
	p_unit->timer_number      = timer_number;		/*使用的定时器*/
//	BSP_Init_TimerInterrupt(timer_number, 1000, 8);		/*初始化定时器中断,周期1000,8分频*/
	BSP_TimerIRQHandler[timer_number] = _Drv_PumpIRQHandler;	/*注册中断处理函数*/
	p_unit->power_port_number = power_port_number;	/*电源引脚端口号*/
	p_unit->power_pin_number  = power_pin_number;	/*电源引脚序号*/
	BSP_Init_Pin(power_port_number, power_pin_number, _OUT_PP_);	/*初始化硬件*/
	p_unit->signal_port_number = signal_port_number;/*信号引脚端口号*/
	p_unit->signal_pin_number  = signal_pin_number;	/*信号引脚序号*/
	BSP_Init_Pin(signal_port_number, signal_pin_number, _OUT_PP_);	/*初始化硬件*/
	
	/*-------------------------------变量*/
	p_unit->status = PUMP_STOP;
	p_unit->duty_cycle_count = 0;
	if(p_unit->logic_type == PUMP_POSITIVE_LOGIC)	/*正逻辑控制*/
	{
		p_unit->power_pin_buffer = 0;				/*初始化为不控制*/
		BSP_WritePin(p_unit->power_port_number, p_unit->power_pin_number, p_unit->power_pin_buffer);
	}
	else											/*负逻辑控制*/
	{
		p_unit->power_pin_buffer = 1;				/*初始化为不控制*/
		BSP_WritePin(p_unit->power_port_number, p_unit->power_pin_number, p_unit->power_pin_buffer);
	}
	
//	BSP_SetTimer(p_unit->timer_number, 8000000 * 2 / fre );	/*定时器中断时基8MHz，初始化定时器中断平率*/
//	BSP_EnTimer(p_unit->timer_number);				/*启动定时器*/
	
	return p_unit;
}

/* 泵启动
  -----------------------------
  入口：泵结构体
*/
void PumpStart(DRV_PUMP_TYPE* p_unit)
{
	_PumpPowerUp(p_unit);
	
	p_unit->status = PUMP_START;
}

/* 泵停止
  -----------------------------
 入口：泵结构体
*/
void PumpStop(DRV_PUMP_TYPE* p_unit)
{
	_PumpPowerOff(p_unit);
	
	p_unit->status = PUMP_STOP;
}

/* 判断泵是否启动
  -----------------------------
  入口：泵结构体
  返回值：如果泵启动则返回true
*/
bool PumpIsStart(DRV_PUMP_TYPE* p_unit)
{
	if(p_unit->status == PUMP_START)
		return true;
	else
		return false;
}

/* 内部函数
 ********************************************************/

/* 泵电源开启
  -----------------------------
  入口：泵结构体
*/
static void _PumpPowerUp(DRV_PUMP_TYPE* p_unit)
{
	if(p_unit->logic_type == PUMP_POSITIVE_LOGIC)		/*正逻辑*/
	{
		p_unit->power_pin_buffer = 1;
		BSP_WritePin(p_unit->power_port_number, p_unit->power_pin_number, p_unit->power_pin_buffer);
	}
	else												/*负逻辑*/
	{
		p_unit->power_pin_buffer = 0;
		BSP_WritePin(p_unit->power_port_number, p_unit->power_pin_number, p_unit->power_pin_buffer);
	}
}

/* 泵电源关闭
  -----------------------------
  入口：泵结构体
*/
static void _PumpPowerOff(DRV_PUMP_TYPE* p_unit)
{
	if(p_unit->logic_type == PUMP_POSITIVE_LOGIC)		/*正逻辑*/
	{
		p_unit->power_pin_buffer = 0;
		BSP_WritePin(p_unit->power_port_number, p_unit->power_pin_number, p_unit->power_pin_buffer);
	}
	else												/*负逻辑*/
	{
		p_unit->power_pin_buffer = 1;
		BSP_WritePin(p_unit->power_port_number, p_unit->power_pin_number, p_unit->power_pin_buffer);
	}
}

/* 泵信号线拉高
  -----------------------------
  入口：泵结构体
*/
static void _PumpSignalUp(DRV_PUMP_TYPE* p_unit)
{
	p_unit->signal_pin_buffer = 1;
	BSP_WritePin(p_unit->signal_port_number, p_unit->signal_pin_number, p_unit->signal_pin_buffer);
}

/* 泵信号线拉低
  -----------------------------
  入口：泵结构体
*/
static void _PumpSignalOff(DRV_PUMP_TYPE* p_unit)
{
	p_unit->signal_pin_buffer = 0;
	BSP_WritePin(p_unit->signal_port_number, p_unit->signal_pin_number, p_unit->signal_pin_buffer);
}

/* 中断处理函数
 ********************************************************/

/* 泵调速中断
  -----------------------------
*/
static void _Drv_PumpIRQHandler(void)
{
	uint8_t i;
	
	for(i = 0; i < PUMP_BUTT; i++)
	{
		if(PumpIsStart(_gp_PumpUnits[i]))	/*泵启动*/
		{
			switch(_gp_PumpUnits[i]->type)
			{
				case PUMP_ADJUSTABLE_SPEED_WITH_POWER: 	/*电源调速泵*/
					_gp_PumpUnits[i]->duty_cycle_count++;	/*占空比计数*/
					if(_gp_PumpUnits[i]->duty_cycle_count == _gp_PumpUnits[i]->duty_cycle)
					{
						_PumpPowerOff(_gp_PumpUnits[i]);	/*关泵电源*/
					}
					else if(_gp_PumpUnits[i]->duty_cycle_count >= 100)
					{
						_PumpPowerUp(_gp_PumpUnits[i]);		/*开泵电源*/
					}
					break;
				
				case PUMP_ADJUSTABLE_SPEED_WITH_SIGNAL: /*信号调速泵*/
					_gp_PumpUnits[i]->duty_cycle_count++;	/*占空比计数*/
					if(_gp_PumpUnits[i]->duty_cycle_count == _gp_PumpUnits[i]->duty_cycle)
					{
						_PumpSignalOff(_gp_PumpUnits[i]);	/*信号拉低*/
					}
					else if(_gp_PumpUnits[i]->duty_cycle_count >= 100)
					{
						_PumpSignalUp(_gp_PumpUnits[i]);	/*信号拉高*/
					}
					break;
				
				default:	/*非调速泵不做处理*/
					break;
			}
		}
	}
}
