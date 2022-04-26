/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——加热膜模块

  -------------------------------------------------------
   文 件 名   : Drv_Heat.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2020年2月14日
   功能描述   : 定义一种加热膜控制模块。
   使用方法   ：上层模块通过调用Drv_HeatInit获得一个Heat模块
				的指针，通过调用Drv_HeatOpen开启加热膜，通过
				调用Drv_HeatClose关闭加热膜。
   依赖于     : BSP_Gpio V1.0.0.0
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#include "Drv_Heat.h"

DRV_HEAT_TYPE* _gp_HeatUnits[HEAT_BUTT] = {0};	/*加热膜的注册表*/

/* 接口函数
 ********************************************************/

/* 初始化加热膜模块
  -----------------------------
  入口：加热膜的ID号，IO端口
  返回值：初始化好的加热膜模块结构体
*/
DRV_HEAT_TYPE* Drv_HeatInit(DRV_HEAT_ENUM id, \
							BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number)
{	
	DRV_HEAT_TYPE* p_unit = (DRV_HEAT_TYPE*)calloc(1, sizeof(DRV_HEAT_TYPE));	/*申请内存*/
	
	/*分配失败返回NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id错误返回NULL*/
	if(id >= HEAT_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*如果当前模块存在，取消分配新存储区*/
	if(_gp_HeatUnits[id] != NULL)
	{
		free(p_unit);
		p_unit = _gp_HeatUnits[id];
	}
	else
	{
		_gp_HeatUnits[id] = p_unit;
	}
	
	/*初始化*/
	
	/*-------------------------------常量*/
	p_unit->id          = id;						/*模块id*/
	p_unit->port_number = port_number;				/*引脚端口号*/
	p_unit->pin_number  = pin_number;				/*引脚序号*/
	BSP_Init_Pin(port_number, pin_number, _OUT_PP_);	/*初始化硬件*/
	
	/*-------------------------------变量*/
	p_unit->status = HEAT_CLOSE;	/*全闭*/
	
	p_unit->pin_buffer = 0;							/*初始化为不控制*/
	BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	
	return p_unit;
}

/* 打开加热膜
  -----------------------------
  入口：加热膜结构体
*/
void Drv_HeatOpen(DRV_HEAT_TYPE* p_unit)
{
	p_unit->pin_buffer = 1;
	BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	p_unit->status = HEAT_OPEN;
}

/* 关闭加热膜
  -----------------------------
 入口：加热膜结构体
*/
void Drv_HeatClose(DRV_HEAT_TYPE* p_unit)
{
	p_unit->pin_buffer = 0;
	BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	p_unit->status = HEAT_CLOSE;
}

/* 判断加热膜是否开启
  -----------------------------
  入口：加热膜结构体
  返回值：如果加热膜开启则返回true
*/
bool Drv_HeatIsOpen(DRV_HEAT_TYPE* p_unit)
{
	if(p_unit->status == HEAT_OPEN)
		return true;
	else
		return false;
}
