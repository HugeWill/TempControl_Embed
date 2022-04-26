/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——风扇模块

  -------------------------------------------------------
   文 件 名   : Drv_Fan.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2020年05月25日
   功能描述   : 定义一种风扇控制模块。
   使用方法   ：上层模块通过调用Drv_FanInit获得一个Fan模块
				的指针，通过调用Drv_FanON开启风扇，通过
				调用Drv_FanOFF关闭风扇。
   依赖于     : BSP.h
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#include "Drv_Fan.h"

DRV_FAN_TYPE* _gp_FanUnits[FAN_BUTT] = {0};	/*风扇的注册表*/

/* 接口函数
 ********************************************************/

/* 初始化风扇模块
  -----------------------------
  入口：风扇的ID号，正/负逻辑，IO端口
  返回值：初始化好的风扇模块结构体
*/
DRV_FAN_TYPE* Drv_FanInit(DRV_FAN_ENUM id, \
						  DRV_FAN_LOGIC_TYPE_ENUM logic_type, \
						  BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number)
{	
	DRV_FAN_TYPE* p_unit = (DRV_FAN_TYPE*)calloc(1, sizeof(DRV_FAN_TYPE));	/*申请内存*/
	
	/*分配失败返回NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id错误返回NULL*/
	if(id >= FAN_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*如果当前模块存在，取消分配新存储区*/
	if(_gp_FanUnits[id] != NULL)
	{
		free(p_unit);
		p_unit = _gp_FanUnits[id];
	}
	else
	{
		_gp_FanUnits[id] = p_unit;
	}
	
	/*初始化*/
	
	/*-------------------------------常量*/
	p_unit->id          = id;						/*模块id*/
	p_unit->logic_type  = logic_type;				/*正/负逻辑*/
	p_unit->port_number = port_number;				/*引脚端口号*/
	p_unit->pin_number  = pin_number;				/*引脚序号*/
	BSP_Init_Pin(port_number, pin_number, _OUT_PP_);	/*初始化硬件*/
	
	/*-------------------------------变量*/
	if(p_unit->logic_type == FAN_POSITIVE_LOGIC)	/*正逻辑控制*/
	{
		p_unit->pin_buffer = 0;						/*初始化为关断*/
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	else											/*负逻辑控制*/
	{
		p_unit->pin_buffer = 1;						/*初始化为关断*/
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	
	return p_unit;
}

/* 风扇开
  -----------------------------
  入口：风扇结构体
*/
void Drv_FanON(DRV_FAN_TYPE* p_unit)
{
	if(p_unit->logic_type == FAN_POSITIVE_LOGIC)		/*正逻辑*/
	{
		p_unit->pin_buffer = 1;
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	else												/*负逻辑*/
	{
		p_unit->pin_buffer = 0;
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	
	p_unit->status = FAN_ON;
}

/* 风扇关
  -----------------------------
 入口：风扇结构体
*/
void Drv_FanOFF(DRV_FAN_TYPE* p_unit)
{
	if(p_unit->logic_type == FAN_POSITIVE_LOGIC)		/*正逻辑*/
	{
		p_unit->pin_buffer = 0;
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	else												/*负逻辑*/
	{
		p_unit->pin_buffer = 1;
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	
	p_unit->status = FAN_OFF;
}
