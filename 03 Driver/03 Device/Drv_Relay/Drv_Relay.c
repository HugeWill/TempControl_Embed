/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——继电器模块

  -------------------------------------------------------
   文 件 名   : Drv_Relay.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2020年05月25日
   功能描述   : 定义一种继电器控制模块。
   使用方法   ：上层模块通过调用Drv_RelayInit获得一个Relay模块
				的指针，通过调用Drv_RelayON开通继电器，通过
				调用Drv_RelayOFF关断继电器。
   依赖于     : BSP.h
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#include "Drv_Relay.h"

DRV_RELAY_TYPE* _gp_RelayUnits[RELAY_BUTT] = {0};	/*继电器的注册表*/

/* 接口函数
 ********************************************************/

/* 初始化继电器模块
  -----------------------------
  入口：继电器的ID号，正/负逻辑，IO端口
  返回值：初始化好的继电器模块结构体
*/
extern DRV_RELAY_TYPE* Drv_RelayInit(DRV_RELAY_ENUM id, \
								  DRV_RELAY_LOGIC_TYPE_ENUM logic_type, \
								  BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number)
{
	DRV_RELAY_TYPE* p_unit = (DRV_RELAY_TYPE*)calloc(1, sizeof(DRV_RELAY_TYPE));	/*申请内存*/
	
	/*分配失败返回NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id错误返回NULL*/
	if(id >= RELAY_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*如果当前模块存在，取消分配新存储区*/
	if(_gp_RelayUnits[id] != NULL)
	{
		free(p_unit);
		p_unit = _gp_RelayUnits[id];
	}
	else
	{
		_gp_RelayUnits[id] = p_unit;
	}
	
	/*初始化*/
	
	/*-------------------------------常量*/
	p_unit->id          = id;						/*模块id*/
	p_unit->logic_type  = logic_type;				/*正/负逻辑*/
	p_unit->port_number = port_number;				/*引脚端口号*/
	p_unit->pin_number  = pin_number;				/*引脚序号*/
	BSP_Init_Pin(port_number, pin_number, _OUT_PP_);	/*初始化硬件*/
	
	/*-------------------------------变量*/
	if(p_unit->logic_type == RELAY_POSITIVE_LOGIC)	/*正逻辑控制*/
	{
		p_unit->pin_buffer = 0;						/*初始化为关断*/
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	else											/*负逻辑控制*/
	{
		p_unit->pin_buffer = 1;						/*初始化为关断*/
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	p_unit->status = RELAY_OFF;
	
	return p_unit;
}

/* 继电器开
  -----------------------------
  入口：继电器结构体
*/
void Drv_RelayON(DRV_RELAY_TYPE* p_unit)
{
	if(p_unit->logic_type == RELAY_POSITIVE_LOGIC)		/*正逻辑*/
	{
		p_unit->pin_buffer = 1;
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	else												/*负逻辑*/
	{
		p_unit->pin_buffer = 0;
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	
	p_unit->status = RELAY_ON;
}

/* 继电器关
  -----------------------------
 入口：继电器结构体
*/
void Drv_RelayOFF(DRV_RELAY_TYPE* p_unit)
{
	if(p_unit->logic_type == RELAY_POSITIVE_LOGIC)		/*正逻辑*/
	{
		p_unit->pin_buffer = 0;
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	else												/*负逻辑*/
	{
		p_unit->pin_buffer = 1;
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	
	p_unit->status = RELAY_OFF;
}
