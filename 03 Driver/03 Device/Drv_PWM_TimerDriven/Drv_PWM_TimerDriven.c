/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

          嵌入式开发平台——PWM模块（使用定时器驱动）

  -------------------------------------------------------
   文 件 名   : Drv_PWM_TimerDriven.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年06月28日
   功能描述   : 定义一种输出PWM脉冲的模块，使用定时器控制
				指定引脚的输出频率，脉宽50%不可调。
   使用方法   ：上层模块通过调用PWM_Init获得一个PWM模块的指
				针，通过调用PWM_Config配置要输出的脉冲数和
				频率，通过调用PWM_Enable启动PWM输出，通过调
				用PWM_Disable中止PWM输出，并返回剩余脉冲数，
				__PWM_Done输出完成处理函数由上层实现，要求
				快进快出，不得使用延时。
   依赖于     : BSP.h
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#include "Drv_PWM_TimerDriven.h"

static DRV_PWM_TIMERDRIVEN_TYPE* _gp_PWMunits[PWM_BUTT] = {0};	/*PWM模块的注册表*/

/* 接口函数
 ********************************************************/

/* PWM模块初始化
  --------------------------------
  入口：模块编号，定时器编号，引脚端口编号，引脚编号
  返回值：分配的PWM模块结构体地址，失败返回NULL
*/
DRV_PWM_TIMERDRIVEN_TYPE* PWM_Init(DRV_PWM_TIMERDRIVEN_ENUM id, \
								   BSP_TIMER_ENUM timer_number, \
								   BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number)
{
	DRV_PWM_TIMERDRIVEN_TYPE* p_unit = (DRV_PWM_TIMERDRIVEN_TYPE*)calloc(1, sizeof(DRV_PWM_TIMERDRIVEN_TYPE));	/*申请内存*/
	
	/*分配失败返回NULL*/
	if(p_unit == NULL) 					
	{
		return NULL;
	}
	
	/*id错误返回NULL*/
	if(id >= PWM_BUTT) 					
	{
		free(p_unit); 
		return NULL;
	}
	
	/*如果当前模块存在，取消分配新存储区*/
	if(_gp_PWMunits[id] != NULL)		
	{
		free(p_unit);
		p_unit = _gp_PWMunits[id];
	}
	else
	{
		_gp_PWMunits[id] = p_unit;
	}
	
	/*初始化*/
	
	/*------------------------------常量*/
	p_unit->id    			= id;						/*模块ID*/
	p_unit->timer_number    = timer_number;				/*使用的定时器*/
	p_unit->port_number     = port_number;				/*引脚端口号*/
	p_unit->pin_number      = pin_number;				/*引脚序号*/
	BSP_Init_Pin(port_number, pin_number, _OUT_PP_);	/*初始化引脚*/
	BSP_Init_TimerInterrupt(timer_number, 1000, 8);		/*初始化定时器中断,周期1000,8分频*/
	switch((uint8_t)(p_unit->id))
	{
		case PWM_1: BSP_TimerIRQHandler[p_unit->timer_number] = _Drv_PWM1IRQHandler;
		break;
		
		case PWM_2: BSP_TimerIRQHandler[p_unit->timer_number] = _Drv_PWM2IRQHandler;
		break;
		
		case PWM_3: BSP_TimerIRQHandler[p_unit->timer_number] = _Drv_PWM3IRQHandler;
		break;
		
		case PWM_4: BSP_TimerIRQHandler[p_unit->timer_number] = _Drv_PWM4IRQHandler;
		break;
	}
	
	/*------------------------------变常*/
	p_unit->total_pulse     = 0;				/*总脉冲数*/
	p_unit->fre             = 0;				/*输出频率*/
	/*------------------------------变量*/
	p_unit->status          = PWM_UNDEFINED;	/*模块状态*/
	p_unit->pin_buffer      = 0;				/*引脚状态*/
	p_unit->remaining_pulse = 0;				/*剩余脉冲数*/

	/*引脚初始化*/
	BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);

	return p_unit;
}

/* PWM模块输出配置
  --------------------------------
  入口：p_unit PWM模块结构体，step 总脉冲数，fre 输出频率
  返回值：true配置成功，false配置失败
*/
bool PWM_Config(DRV_PWM_TIMERDRIVEN_TYPE* p_unit, uint32_t step, uint16_t fre)
{
	if(p_unit->status != PWM_BUSY)
	{
		p_unit->total_pulse = step;	/*记录总脉冲数和频率*/
		p_unit->remaining_pulse = p_unit->total_pulse * 2;	/*脉冲翻转两次为输出一个脉冲*/
		p_unit->fre = fre;
		BSP_SetTimer(p_unit->timer_number, p_unit->fre);				/*初始化定时器溢出值*/
		
		p_unit->status = PWM_READY;	/*配置完成，待命*/
		
		return true;
	}
	
	return false;
}

/* PWM输出使能
  --------------------------------
  用于启动PWM输出
  入口：p_unit PWM模块结构体
  返回值：true使能成功，false使能失败
*/
bool PWM_Enable(DRV_PWM_TIMERDRIVEN_TYPE* p_unit)
{	
	if(p_unit->status == PWM_READY)
	{
		p_unit->status = PWM_BUSY;
		BSP_EnTimer(p_unit->timer_number);
		
		return true;
	}
	
	return false;
}

/* PWM输出停止
  --------------------------------
  用于中止PWM输出
  入口：p_unit PWM模块结构体
  返回值：剩余未输出脉冲数
*/
uint32_t PWM_Disable(DRV_PWM_TIMERDRIVEN_TYPE* p_unit)
{
	if(p_unit->status == PWM_BUSY)		/*该模块在运行时才进行停止操作*/
	{
		BSP_DisTimer(p_unit->timer_number);
		p_unit->pin_buffer = 0;			/*pwm引脚默认输出低电平*/
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
		p_unit->status = PWM_READY;		/*状态切换为待命*/
		
		return p_unit->remaining_pulse;	/*返回剩余脉冲数*/
	}
	
	return 0;
}

/* 内部函数
 ********************************************************/
 
/* PWM输出更新
  --------------------------------
  用于在定时器中断中更新PWM输出
  入口：p_unit PWM模块结构体
  返回值：该模块状态
*/
static DRV_PWM_TIMERDRIVEN_STATUS_ENUM _PWM_Update(DRV_PWM_TIMERDRIVEN_TYPE* p_unit)
{
//	static uint8_t count = 0;
	
	if(p_unit->status == PWM_BUSY)	/*该模块正在运行*/
	{
		p_unit->pin_buffer = !(p_unit->pin_buffer);	/*翻转信号缓冲区并输出*/
		BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
//		count = (count + 1) % 2;
//		if(count == 0)								/*输出一个完整脉冲后，剩余脉冲数减一*/
//		{
//		if(--(p_unit->remaining_pulse) <= 0)	/*剩余脉冲数自减，输出完成后，状态切换*//*20191011修改，remaining_pulse赋值时已经乘以2，不再使用count计数*/
//			{
//				PWM_Disable(p_unit);
//			}
//		}
	}
	
	return p_unit->status;	/*返回模块状态*/
}

/* 中断处理函数
 ********************************************************/
 
/* PWM1输出中断
  --------------------------------
*/
static void _Drv_PWM1IRQHandler(void)
{
	if(_gp_PWMunits[PWM_1] != NULL)
	{
		if(_PWM_Update(_gp_PWMunits[PWM_1]) != PWM_BUSY)
		{
//			__PWM_Done(_gp_PWMunits[PWM_1]);
		}
	}
}

/* PWM2输出中断
  --------------------------------
*/
static void _Drv_PWM2IRQHandler(void)
{
	if(_gp_PWMunits[PWM_2] != NULL)
	{
		if(_PWM_Update(_gp_PWMunits[PWM_2]) != PWM_BUSY)
		{
			__PWM_Done(_gp_PWMunits[PWM_2]);
		}
	}
}

/* PWM3输出中断
  --------------------------------
*/
static void _Drv_PWM3IRQHandler(void)
{
	if(_gp_PWMunits[PWM_3] != NULL)
	{
		if(_PWM_Update(_gp_PWMunits[PWM_3]) != PWM_BUSY)
		{
			__PWM_Done(_gp_PWMunits[PWM_3]);
		}
	}
}

/* PWM4输出中断
  --------------------------------
*/
static void _Drv_PWM4IRQHandler(void)
{
	if(_gp_PWMunits[PWM_4] != NULL)
	{
		if(_PWM_Update(_gp_PWMunits[PWM_4]) != PWM_BUSY)
		{
			__PWM_Done(_gp_PWMunits[PWM_4]);
		}
	}
}
