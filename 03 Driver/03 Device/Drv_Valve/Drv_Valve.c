/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——电磁阀模块

  -------------------------------------------------------
   文 件 名   : Drv_Valve.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年06月28日
   功能描述   : 定义一种电磁阀控制模块，支持四个电磁阀的
				控制。
   使用方法   ：上层模块通过调用Valve_Init获得一个Valve模块
				的指针，通过调用Valve_Open打开电磁阀，通过
				调用Valve_Close关闭电磁阀。
   依赖于     : BSP.h
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#include "Drv_Valve.h"

DRV_VALVE_TYPE* _gp_ValveUnits[VALVE_BUTT] = {0};	/*电磁阀的注册表*/

/* 接口函数
 ********************************************************/

/* 初始化电磁阀模块
  -----------------------------
  入口：电磁阀的ID号，常开/常闭，正/负逻辑，IO端口
  返回值：初始化好的电磁阀模块结构体
*/
DRV_VALVE_TYPE* Valve_Init(DRV_VALVE_ENUM id, \
						   DRV_VALVE_TYPE_ENUM valve_type, \
						   DRV_VALVE_LOGIC_TYPE_ENUM valve_logic, \
						   BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number)
{	
	DRV_VALVE_TYPE* p_unit = (DRV_VALVE_TYPE*)calloc(1, sizeof(DRV_VALVE_TYPE));	/*申请内存*/
	
	/*分配失败返回NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id错误返回NULL*/
	if(id >= VALVE_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*如果当前模块存在，取消分配新存储区*/
	if(_gp_ValveUnits[id] != NULL)
	{
		free(p_unit);
		p_unit = _gp_ValveUnits[id];
	}
	else
	{
		_gp_ValveUnits[id] = p_unit;
	}
	
	/*初始化*/
	
	/*-------------------------------常量*/
	p_unit->id          = id;						/*模块id*/
	p_unit->type        = valve_type;				/*常开/常闭*/
	p_unit->logic_type  = valve_logic;				/*正/负逻辑*/
	p_unit->port_number = port_number;				/*引脚端口号*/
	p_unit->pin_number  = pin_number;				/*引脚序号*/
	BSP_Init_Pin(port_number, pin_number, _OUT_PP_);	/*初始化硬件*/
	
	/*-------------------------------变量*/
	if(p_unit->type == VALVE_NORMAL_CLOSE)			/*常闭阀*/
	{
		p_unit->status = VALVE_CLOSE;
	}
	else											/*常开阀*/
	{
		p_unit->status = VALVE_OPEN;
	}
	if(p_unit->logic_type == VALVE_POSITIVE_LOGIC)	/*正逻辑控制*/
	{
		p_unit->pin_buffer = 0;						/*初始化为不控制*/
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	else											/*负逻辑控制*/
	{
		p_unit->pin_buffer = 1;						/*初始化为不控制*/
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	
	return p_unit;
}

/* 打开电磁阀
  -----------------------------
  入口：电磁阀结构体
*/
void Valve_Open(DRV_VALVE_TYPE* p_unit)
{
	if((p_unit->type == VALVE_NORMAL_CLOSE) && (p_unit->logic_type == VALVE_POSITIVE_LOGIC))		/*常闭阀，正逻辑*/
	{
		p_unit->pin_buffer = 1;
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	else if((p_unit->type == VALVE_NORMAL_CLOSE) && (p_unit->logic_type == VALVE_NEGATIVE_LOGIC))	/*常闭阀，负逻辑*/
	{
		p_unit->pin_buffer = 0;
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	else if((p_unit->type == VALVE_NORMAL_OPEN) && (p_unit->logic_type == VALVE_POSITIVE_LOGIC))	/*常开阀，正逻辑*/
	{
		p_unit->pin_buffer = 0;
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	else																							/*常开阀，负逻辑*/
	{
		p_unit->pin_buffer = 1;
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	
	p_unit->status = VALVE_OPEN;
}

/* 关闭电磁阀
  -----------------------------
 入口：电磁阀结构体
*/
void Valve_Close(DRV_VALVE_TYPE* p_unit)
{
	if((p_unit->type == VALVE_NORMAL_CLOSE) && (p_unit->logic_type == VALVE_POSITIVE_LOGIC))		/*常闭阀，正逻辑*/
	{
		p_unit->pin_buffer = 0;
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	else if((p_unit->type == VALVE_NORMAL_CLOSE) && (p_unit->logic_type == VALVE_NEGATIVE_LOGIC))	/*常闭阀，负逻辑*/
	{
		p_unit->pin_buffer = 1;
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	else if((p_unit->type == VALVE_NORMAL_OPEN) && (p_unit->logic_type == VALVE_POSITIVE_LOGIC))	/*常开阀，正逻辑*/
	{
		p_unit->pin_buffer = 1;
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	else																							/*常开阀，负逻辑*/
	{
		p_unit->pin_buffer = 0;
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	
	p_unit->status = VALVE_CLOSE;
}

/* 判断电磁阀是否开启
  -----------------------------
  入口：电磁阀结构体
  返回值：如果电磁阀开启则返回true
*/
bool Valve_Is_Open(DRV_VALVE_TYPE* p_unit)
{
	if(p_unit->status == VALVE_OPEN)
		return true;
	else
		return false;
}
