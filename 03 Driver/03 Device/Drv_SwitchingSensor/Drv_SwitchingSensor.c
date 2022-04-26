/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

			嵌入式开发平台——开关信号模块

  -------------------------------------------------------
   文 件 名   : Drv_SwitchingSensor.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年10月16日
   功能描述   : 定义开关信号的驱动。
   使用方法   ：上层模块通过调用SwitchingSensorInit获得一个
				SwitchingSensor的指针。
   依赖于     : BSP_Gpio V1.0.0.0
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#include "Drv_SwitchingSensor.h"

DRV_SWITCHINGSENSOR_TYPE* _gp_SwitchingSensorUnits[SWITCHINGSENSOR_BUTT] = {0};	/*开关信号模块的注册表*/

/* 接口函数
 ********************************************************/

/* 初始化开关信号模块
  -----------------------------
  入口：开关信号模块的ID号，延迟消抖时间
  返回值：初始化好的M开关信号模块结构体
*/
DRV_SWITCHINGSENSOR_TYPE* Drv_SwitchingSensorInit(DRV_SWITCHINGSENSOR_ENUM id, \
												DRV_SWITCHINGSENSOR_RESPONSE_TYPE_ENUM response_type, \
												uint16_t delay_time, \
												BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number)
{	
	DRV_SWITCHINGSENSOR_TYPE* p_unit = (DRV_SWITCHINGSENSOR_TYPE*)calloc(1, sizeof(DRV_SWITCHINGSENSOR_TYPE));	/*申请内存*/

	/*分配失败返回NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id错误返回NULL*/
	if(id >= SWITCHINGSENSOR_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*如果当前模块存在，取消分配新存储区*/
	if(_gp_SwitchingSensorUnits[id] != NULL)
	{
		free(p_unit);
		p_unit = _gp_SwitchingSensorUnits[id];
	}
	else
	{
		_gp_SwitchingSensorUnits[id] = p_unit;
	}
	
	/*初始化*/
	
	/*-------------------------------常量*/
	p_unit->id          = id;						/*模块id*/
	p_unit->response_type = response_type;			/*响应类型*/
	p_unit->delay_time  = delay_time;				/*消抖延时时间*/
	if(p_unit->sem == NULL)							/*光耦信号量*/
		p_unit->sem = OSSemCreate(0);
	p_unit->port_number = port_number;				/*引脚端口号*/
	p_unit->pin_number  = pin_number;				/*引脚编号*/
	BSP_Init_Pin(port_number, pin_number, _IN_FLOATING_);	/*初始化为普通IO输入*/
	
	/*-------------------------------变量*/
	p_unit->status = SWITCHINGSENSOR_READY;			/*模块状态*/
	p_unit->buffer = 1;								/*引脚状态*/
	p_unit->buffer_check = 0;						/*引脚状态待消抖*/
	p_unit->delay_count = 0;						/*消抖计时*/
	p_unit->delay_flag = 0;							/*消抖标志*/
	p_unit->sem_switch = DIS_SEM;					/*信号量开关*/
	
	return p_unit;
}

/* 信号量设置
  -----------------------------
  入口：开关信号模块结构体，信号量开关，信号量响应类型
  返回值：true 成功，false 失败
*/
bool Drv_SwitchingSensorSemSet(DRV_SWITCHINGSENSOR_TYPE* p_unit, \
								DRV_SWITCHINGSENSOR_SEM_STATUS_ENUM sem_switch, \
								DRV_SWITCHINGSENSOR_RESPONSE_TYPE_ENUM response_type)
{
	p_unit->sem_switch = sem_switch;
	p_unit->response_type = response_type;
	return true;
}

/* 读取信号状态
  -----------------------------
  入口：开关信号模块结构体
  返回值：信号状态
*/
uint8_t Drv_SwitchingSensorRead(DRV_SWITCHINGSENSOR_TYPE* p_unit)
{
	return p_unit->buffer;
}

/* 常驻内存函数
 ********************************************************/

/* 所有信号消抖函数
  -----------------------------
  入口：
  返回值：
*/
void Drv_AllSwitchingSensorDebounce_Loop(void)
{
	uint8_t i = 0;
	uint8_t buffer_now = 0;
	
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 1);	/*每隔1ms，扫描所有开关信号变化*/
		for(i = 0; i < SWITCHINGSENSOR_BUTT; i++)
		{
			if(_gp_SwitchingSensorUnits[i] != NULL)	/*IO在使用的，才做处理*/
			{
				buffer_now = BSP_ReadPin(_gp_SwitchingSensorUnits[i]->port_number,_gp_SwitchingSensorUnits[i]->pin_number);
				if(buffer_now != _gp_SwitchingSensorUnits[i]->buffer)
				{
					OSTimeDlyHMSM(0,0,0,1);
					if(buffer_now == BSP_ReadPin(_gp_SwitchingSensorUnits[i]->port_number,_gp_SwitchingSensorUnits[i]->pin_number)&&buffer_now!=_gp_SwitchingSensorUnits[i]->buffer)
					{	
						if(_gp_SwitchingSensorUnits[i]->response_type == FALLING)
						{
							if(_gp_SwitchingSensorUnits[i]->buffer<buffer_now)
								OSSemPost(_gp_SwitchingSensorUnits[i]->sem);
						}
						else
						{
							if(_gp_SwitchingSensorUnits[i]->buffer>buffer_now)
								OSSemPost(_gp_SwitchingSensorUnits[i]->sem);
						}
						_gp_SwitchingSensorUnits[i]->buffer = buffer_now;
					}
				}
			}
		}
	}
}
