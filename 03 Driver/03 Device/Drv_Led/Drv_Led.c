/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——LED模块

  -------------------------------------------------------
   文 件 名   : Drv_Led.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年07月19日
   功能描述   : 定义一种LED模块，仅支持LED的亮灭控制。
   使用方法   ：上层模块通过调用Led_Init获得一个Led模块
				的指针，通过调用Led_On点亮，通过调用Led_Off
				熄灭。
   依赖于     : BSP_Gpio V1.0.0.0
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#include "Drv_Led.h"

DRV_LED_TYPE* _gp_LedUnits[LED_BUTT] = {0};	/*LED的注册表*/

/* 接口函数
 ********************************************************/

/* 初始化LED模块
  -----------------------------
  入口：LED模块的ID号，正/负逻辑，IO端口
  返回值：初始化好的LED模块结构体
*/
DRV_LED_TYPE* Led_Init(DRV_LED_ENUM id, \
					   DRV_LED_LOGIC_TYPE_ENUM valve_logic, \
					   BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number)
{	
	DRV_LED_TYPE* p_unit = (DRV_LED_TYPE*)calloc(1, sizeof(DRV_LED_TYPE));	/*申请内存*/

	/*分配失败返回NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id错误返回NULL*/
	if(id >= LED_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*如果当前模块存在，取消分配新存储区*/
	if(_gp_LedUnits[id] != NULL)
	{
		free(p_unit);
		p_unit = _gp_LedUnits[id];
	}
	else
	{
		_gp_LedUnits[id] = p_unit;
	}
	
	/*初始化*/
	
	/*-------------------------------常量*/
	p_unit->id          = id;						/*模块id*/
	p_unit->logic_type  = valve_logic;				/*正/负逻辑*/
	p_unit->port_number = port_number;				/*引脚端口号*/
	p_unit->pin_number  = pin_number;				/*引脚序号*/
	BSP_Init_Pin(port_number, pin_number, _OUT_PP_);	/*初始化硬件*/
	
	/*-------------------------------变量*/
	if(p_unit->logic_type == LED_POSITIVE_LOGIC)	/*正逻辑点亮*/
	{
		p_unit->pin_buffer = 0;						/*初始化为熄灭*/
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	else											/*负逻辑点亮*/
	{
		p_unit->pin_buffer = 1;						/*初始化为熄灭*/
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	p_unit->status = LED_OFF;
	
	return p_unit;
}

/* LED点亮
  -----------------------------
  入口：LED结构体
*/
void Led_On(DRV_LED_TYPE* p_unit)
{
	if(p_unit->logic_type == LED_POSITIVE_LOGIC)	/*正逻辑点亮*/
	{
		p_unit->pin_buffer = 1;
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	else											/*负逻辑点亮*/
	{
		p_unit->pin_buffer = 0;
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	
	p_unit->status = LED_ON;
}

/* LED熄灭
  -----------------------------
 入口：LED结构体
*/
void Led_Off(DRV_LED_TYPE* p_unit)
{
	if(p_unit->logic_type == LED_POSITIVE_LOGIC)	/*正逻辑熄灭*/
	{
		p_unit->pin_buffer = 0;
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	else											/*负逻辑熄灭*/
	{
		p_unit->pin_buffer = 1;
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	}
	
	p_unit->status = LED_OFF;
}

/* 判断LED是否点亮
  -----------------------------
  入口：LED结构体
  返回值：如果LED点亮则返回true
*/
bool Led_Is_On(DRV_LED_TYPE* p_unit)
{
	if(p_unit->status == LED_ON)
		return true;
	else
		return false;
}
