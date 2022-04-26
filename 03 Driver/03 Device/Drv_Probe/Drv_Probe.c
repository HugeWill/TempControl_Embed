/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——程序探针

  -------------------------------------------------------
   文 件 名   : Drv_Probe.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2020年03月13日
   功能描述   : 定义一种程序探针模块。
   使用方法   ：上层模块通过调用Probe_Init获得一个程序探针模块
				的指针，通过调用Probe_On输出高电平，通过调用
				Probe_Off输出低电平。				
   依赖于     : BSP_Gpio V1.0.0.0
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#include "Drv_Probe.h"

DRV_PROBE_TYPE* _gp_ProbeUnits[PROBE_BUTT] = {0};	/*程序探针的注册表*/

/* 接口函数
 ********************************************************/

/* 初始化程序探针模块
  -----------------------------
  入口：程序探针模块的ID号，IO端口
  返回值：初始化好的程序探针模块结构体
*/
DRV_PROBE_TYPE* Probe_Init(DRV_PROBE_ENUM id, \
							  BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number)
{	
	DRV_PROBE_TYPE* p_unit = (DRV_PROBE_TYPE*)calloc(1, sizeof(DRV_PROBE_TYPE));	/*申请内存*/

	/*分配失败返回NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id错误返回NULL*/
	if(id >= PROBE_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*如果当前模块存在，取消分配新存储区*/
	if(_gp_ProbeUnits[id] != NULL)
	{
		free(p_unit);
		p_unit = _gp_ProbeUnits[id];
	}
	else
	{
		_gp_ProbeUnits[id] = p_unit;
	}
	
	/*初始化*/
	
	/*-------------------------------常量*/
	p_unit->id          = id;						/*模块id*/
	p_unit->port_number = port_number;				/*引脚端口号*/
	p_unit->pin_number  = pin_number;				/*引脚序号*/
	BSP_Init_Pin(port_number, pin_number, _OUT_PP_);	/*初始化硬件*/
	
	/*-------------------------------变量*/
	p_unit->pin_buffer = 0;							/*初始化为输出低电平*/
	BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	p_unit->status = PROBE_OFF;
	
	return p_unit;
}

/* 程序探针输出高电平
  -----------------------------
  入口：程序探针结构体
*/
void Probe_On(DRV_PROBE_TYPE* p_unit)
{
	p_unit->pin_buffer = 1;
	BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);	
	p_unit->status = PROBE_ON;
}

/* 程序探针输出低电平
  -----------------------------
 入口：程序探针结构体
*/
void Probe_Off(DRV_PROBE_TYPE* p_unit)
{
	p_unit->pin_buffer = 0;
	BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	p_unit->status = PROBE_OFF;
}
