/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

  嵌入式开发平台——运动组件（电机+复位/工位/码齿/撞击信号）

  -------------------------------------------------------
   文 件 名   : Drv_MotionControlComponents.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年06月27日
   功能描述   : 运动组件定义
   依赖于     : uC/OS-II V2.92.07
				Drv_Motor V1.0.0.0
   注         ：该模块不依赖于硬件电路
 ********************************************************/

#include "Drv_MotionControlComponents.h"

static DRV_MCC_TYPE* _gp_MCCunits[MCC_BUTT] = {0};	/*组件的注册表*/

static MCC_COMPENSATION _g_Compensation[MCC_BUTT] = 	/*组件的复位补偿参数表*/
{
	10, 10, 10, 10
};

static MCC_PRESET_LOCATION _g_PresetLocation[MCC_BUTT][MCC_PRESET_LOCATION_MAX_NUMBER] = 	/*组件的位置坐标参数表*/
{
	{{UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, 
	 {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}},
	{{UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, 
	 {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}},
	{{UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, 
	 {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}},
	{{UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, 
	 {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}, {UNDEFINED_CLOSE_CYCLE,0,0,0,0,0}},
};

/* 接口函数
 ********************************************************/

/* 组件初始化
  ---------------------------------
  入口：组件编号，细分，步距角（°），导程（按角度°或距离mm），最大行程（按角度°或距离mm）
  组件类型，复位方向，复位状态
	码齿状态、撞击状态
  复位端口，工位端口，码齿端口，防撞端口，使能端口，方向端口，脉冲端口
  返回值：初始化好的组件指针
*/
DRV_MCC_TYPE* MCC_Init(MCC_ENUM id, \
						uint8_t subdivision, double step_angle, double ratio, double max_distance, \
						MCC_TYPE_ENUM type, MCC_RESET_DIR_ENUM reset_dir, MCC_SHELTER_ENUM reset_status, \
						MCC_SHELTER_ENUM d_code_status, MCC_SHELTER_ENUM strike_status, \
						BSP_PORT_ENUM reset_port_number,   BSP_PIN_ENUM reset_pin_number, \
						BSP_PORT_ENUM d_code_port_number,  BSP_PIN_ENUM d_code_pin_number, \
						BSP_PORT_ENUM strike_port_number,  BSP_PIN_ENUM strike_pin_number, \
						BSP_PORT_ENUM strike2_port_number,  BSP_PIN_ENUM strike2_pin_number, \
						BSP_PORT_ENUM en_port_number,      BSP_PIN_ENUM en_pin_number, \
						BSP_PORT_ENUM dir_port_number,     BSP_PIN_ENUM dir_pin_number, \
						BSP_PORT_ENUM step_port_number,    BSP_PIN_ENUM step_pin_number)
{
	DRV_MCC_TYPE* p_mcc = (DRV_MCC_TYPE*)calloc(1, sizeof(DRV_MCC_TYPE));	/*申请内存*/
	
	if(p_mcc == NULL) 					/*分配失败返回NULL*/
		return NULL;
		
	if(id >= MCC_BUTT) 					/*id错误返回NULL*/
	{
		free(p_mcc);
		return NULL;
	}
	
	if(_gp_MCCunits[id] != NULL)		/*如果当前模块存在，取消分配新存储区*/
	{
		free(p_mcc);
		p_mcc = _gp_MCCunits[id];
	}
	else
	{
		_gp_MCCunits[id] = p_mcc;
	}
		
	/*初始化*/
	
	/*分模块初始化*/
	switch(id)
	{
		case MCC_1:
			/*------------------------------------------常量*/
			p_mcc->motor = Motor_Init(MOTOR_1, subdivision, \
									   en_port_number, en_pin_number, \
									   dir_port_number, dir_pin_number, \
									   step_port_number, step_pin_number);	/*初始化电机*/
			break;
			
		case MCC_2:
			/*------------------------------------------常量*/
			p_mcc->motor = Motor_Init(MOTOR_2, subdivision, \
									   en_port_number, en_pin_number, \
									   dir_port_number, dir_pin_number, \
									   step_port_number, step_pin_number);	/*初始化电机*/
			break;
			
		case MCC_3:
			/*------------------------------------------常量*/
			p_mcc->motor = Motor_Init(MOTOR_3, subdivision, \
									   en_port_number, en_pin_number, \
									   dir_port_number, dir_pin_number, \
									   step_port_number, step_pin_number);	/*初始化电机*/
			break;
			
		case MCC_4:
			/*------------------------------------------常量*/
			p_mcc->motor = Motor_Init(MOTOR_4, subdivision, \
									   en_port_number, en_pin_number, \
									   dir_port_number, dir_pin_number, \
									   step_port_number, step_pin_number);	/*初始化电机*/
			break;
			
		default:
			free(p_mcc);
			return NULL;
	}
	
	/*公共的初始化*/	
	
	/*------------------------------常量*/
	p_mcc->id             = id;					/*组件编号*/
	p_mcc->type           = type;				/*组件类型（带复位、工位、撞击的直线机构）*/
	p_mcc->compensation = &_g_Compensation[id];	/*复位补偿结构体*/
	p_mcc->preset_location = &_g_PresetLocation[id][0];	/*预设位置参数*/
	p_mcc->reset_dir      = reset_dir;			/*复位方向*/
	p_mcc->reset_status   = reset_status;		/*复位的状态*/
	p_mcc->d_code_status   = d_code_status;		/*码齿的状态*/
	p_mcc->strike_status   = strike_status;		/*撞击的状态*/
	p_mcc->subdivision    = subdivision;		/*细分数*/
	p_mcc->step_angle     = step_angle;			/*步距角*/
	p_mcc->ratio          = ratio;				/*导程*/
	p_mcc->max_step       = MCC_CalcSubdivisionStep(p_mcc, max_distance);	/*最大步数*/
	
	if(p_mcc->sem == NULL)						/*组件信号量*/
		p_mcc->sem = OSSemCreate(0);
		
	p_mcc->reset_port_number = reset_port_number;		/*复位信号*/
	p_mcc->reset_pin_number = reset_pin_number;
	BSP_Init_PinInterrupt(reset_port_number, reset_pin_number);
		
	p_mcc->d_code_port_number = d_code_port_number;		/*码齿信号*/
	p_mcc->d_code_pin_number = d_code_pin_number;
	BSP_Init_PinInterrupt(d_code_port_number, d_code_pin_number);
	
	p_mcc->strike_port_number = strike_port_number;		/*防撞信号*/
	p_mcc->strike_pin_number = strike_pin_number;
	BSP_Init_PinInterrupt(strike_port_number, strike_pin_number);
	
	p_mcc->strike2_port_number = strike2_port_number;		/*第二防撞信号*/
	p_mcc->strike2_pin_number = strike2_pin_number;
	BSP_Init_PinInterrupt(strike2_port_number, strike2_pin_number);
	
	switch(p_mcc->id)
	{
		case MCC_1:
			if(p_mcc->reset_pin_number != _PIN_BUTT_)	/*有复位信号，登记中断处理函数*/
				BSP_ExtiIRQHandler[p_mcc->reset_pin_number] = MCC1_ResetExtiIrqHandler;
			if(p_mcc->d_code_pin_number != _PIN_BUTT_)	/*有码齿信号，登记中断处理函数*/
				BSP_ExtiIRQHandler[p_mcc->d_code_pin_number] = MCC1_DCodeExtiIrqHandler;
			if(p_mcc->strike_pin_number != _PIN_BUTT_)	/*有撞击信号，登记中断处理函数*/
				BSP_ExtiIRQHandler[p_mcc->strike_pin_number] = MCC1_StrikeExtiIrqHandler;
			if(p_mcc->strike2_pin_number != _PIN_BUTT_)	/*有第二撞击信号，登记中断处理函数*/
				BSP_ExtiIRQHandler[p_mcc->strike2_pin_number] = MCC1_StrikeExtiIrqHandler;
			break;
		case MCC_2:
			if(p_mcc->reset_pin_number != _PIN_BUTT_)	/*有复位信号，登记中断处理函数*/
				BSP_ExtiIRQHandler[p_mcc->reset_pin_number] = MCC2_ResetExtiIrqHandler;
			if(p_mcc->d_code_pin_number != _PIN_BUTT_)	/*有码齿信号，登记中断处理函数*/
				BSP_ExtiIRQHandler[p_mcc->d_code_pin_number] = MCC2_DCodeExtiIrqHandler;
			if(p_mcc->strike_pin_number != _PIN_BUTT_)	/*有撞击信号，登记中断处理函数*/
				BSP_ExtiIRQHandler[p_mcc->strike_pin_number] = MCC2_StrikeExtiIrqHandler;
			if(p_mcc->strike2_pin_number != _PIN_BUTT_)	/*有第二撞击信号，登记中断处理函数*/
				BSP_ExtiIRQHandler[p_mcc->strike2_pin_number] = MCC2_StrikeExtiIrqHandler;
			break;
		case MCC_3:
			if(p_mcc->reset_pin_number != _PIN_BUTT_)	/*有复位信号，登记中断处理函数*/
				BSP_ExtiIRQHandler[p_mcc->reset_pin_number] = MCC3_ResetExtiIrqHandler;
			if(p_mcc->d_code_pin_number != _PIN_BUTT_)	/*有码齿信号，登记中断处理函数*/
				BSP_ExtiIRQHandler[p_mcc->d_code_pin_number] = MCC3_DCodeExtiIrqHandler;
			if(p_mcc->strike_pin_number != _PIN_BUTT_)	/*有撞击信号，登记中断处理函数*/
				BSP_ExtiIRQHandler[p_mcc->strike_pin_number] = MCC3_StrikeExtiIrqHandler;
			if(p_mcc->strike2_pin_number != _PIN_BUTT_)	/*有第二撞击信号，登记中断处理函数*/
				BSP_ExtiIRQHandler[p_mcc->strike2_pin_number] = MCC3_StrikeExtiIrqHandler;
			break;
		case MCC_4:
			if(p_mcc->reset_pin_number != _PIN_BUTT_)	/*有复位信号，登记中断处理函数*/
				BSP_ExtiIRQHandler[p_mcc->reset_pin_number] = MCC4_ResetExtiIrqHandler;
			if(p_mcc->d_code_pin_number != _PIN_BUTT_)	/*有码齿信号，登记中断处理函数*/
				BSP_ExtiIRQHandler[p_mcc->d_code_pin_number] = MCC4_DCodeExtiIrqHandler;
			if(p_mcc->strike_pin_number != _PIN_BUTT_)	/*有撞击信号，登记中断处理函数*/
				BSP_ExtiIRQHandler[p_mcc->strike_pin_number] = MCC4_StrikeExtiIrqHandler;
			if(p_mcc->strike2_pin_number != _PIN_BUTT_)	/*有第二撞击信号，登记中断处理函数*/
				BSP_ExtiIRQHandler[p_mcc->strike2_pin_number] = MCC4_StrikeExtiIrqHandler;
			break;
		default:
			break;
	}
		
	/*------------------------------变常*/
	p_mcc->true_station = 0;					/*工位实际脉冲数记录*/
	p_mcc->preset_location_now = NULL;			/*当前目标预设位置*/
		
	/*------------------------------变量*/
	p_mcc->status = MCC_IDLE;					/*组件状态初始化*/
	p_mcc->position = 0;
	p_mcc->location = 0;
	p_mcc->d_code_count = 0;					/*码齿倒计数*/	
	p_mcc->strike_flag = 0;						/*撞击标志*/
	p_mcc->d_code_flag = 0;						/*码齿到位标志*/
	
	return p_mcc;
}


 /* 组件保持力矩关闭
  ---------------------------------
  入口：p_mcc 电机结构体
*/
void MCC_Motor_Disable(DRV_MCC_TYPE* p_mcc)
{
	p_mcc->motor->en_buffer = 1;
	BSP_WritePin(p_mcc->motor->en_port_number, p_mcc->motor->en_pin_number, p_mcc->motor->en_buffer);
}
							  
 /* 组件保持力矩开启
  ---------------------------------
  入口：p_mcc 电机结构体
*/
void MCC_Motor_Enable(DRV_MCC_TYPE* p_mcc)
{
	p_mcc->motor->en_buffer = 0;
	BSP_WritePin(p_mcc->motor->en_port_number, p_mcc->motor->en_pin_number, p_mcc->motor->en_buffer);
}

 /* 单个组件按整步数运动
  ---------------------------------
  按照电机的整步数运动

  入口：p_mcc 电机结构体，dir 方向，step 整步数，
		timeout 超时时间，status 运动类型，count 如果走码齿位置的码齿数，
		line_choice 运动曲线选择
  返回值：err OS报错代码或0xFF
*/
uint8_t MCC_Move_SynchronizingStep(DRV_MCC_TYPE*        p_mcc, \
								   uint8_t              dir, \
								   uint32_t             synchronizing_step, \
								   uint32_t             timeout, \
								   MCC_STATUS_ENUM      status, \
								   uint8_t              count, \
								   MCC_LINE_CHOICE_ENUM line_choice)
{
	uint8_t  err              = OS_ERR_NONE;
	uint32_t subdivision_step = p_mcc->subdivision * synchronizing_step;	/*换算为细分后的脉冲数*/
	
	err = _MCC_Move_SubdivisionStep(p_mcc, dir, subdivision_step, timeout, status, count, line_choice);
	
	return err;
}

/* 单个组件按整步数坐标运动
  ---------------------------------
  按照电机的整步数运动

  入口：p_mcc 电机结构体，location 脉冲坐标（相对于复位点），
		timeout 超时时间，status 运动类型，count 如果走码齿位置的码齿数，
		line_choice 运动曲线选择
  返回值：err OS报错代码或0xFF
*/
uint8_t MCC_Move_SynchronizingLocation(DRV_MCC_TYPE*        p_mcc, \
									   int32_t              synchronizing_location, \
									   uint32_t             timeout, \
									   MCC_STATUS_ENUM      status, \
									   uint8_t              count, \
									   MCC_LINE_CHOICE_ENUM line_choice)
{
	uint8_t  err                  = OS_ERR_NONE;
	uint32_t subdivision_location = p_mcc->subdivision * synchronizing_location;	/*换算为细分后的脉冲数*/
	
	err = _MCC_Move_Location(p_mcc, subdivision_location, timeout, status, count, line_choice);
	
	return err;
}

/* 单个组件按距离/角度/液量运动
  ---------------------------------
  单位为mm/°/uL，计算为细分后的脉冲数，且规整到整步

  入口：p_mcc 电机结构体，dir 方向，distance 距离/角度/液量（正值），
		timeout 超时时间，status 运动类型，count 如果走码齿位置的码齿数，
		line_choice 运动曲线选择
  返回值：err OS报错代码或0xFF
*/
uint8_t MCC_Move_Distance(DRV_MCC_TYPE*           p_mcc, \
						  uint8_t                 dir, \
						  double                  distance, \
						  uint32_t                timeout, \
						  MCC_STATUS_ENUM         status, \
						  uint8_t                 count, \
						  MCC_LINE_CHOICE_ENUM    line_choice)
{
	uint8_t  err              = OS_ERR_NONE;
	uint32_t subdivision_step = MCC_CalcSubdivisionStep(p_mcc, distance);	/*换算为脉冲数*/
	
	err = _MCC_Move_SubdivisionStep(p_mcc, dir, subdivision_step, timeout, status, count, line_choice);
	
	return err;
}

/* 单个组件按距离/角度/液量坐标运动
  ---------------------------------
  入口：p_mcc 电机结构体，coordinates 位置/角度坐标（相对于复位点，有正负），
		slowstep 慢速步数，timeout 超时时间，status 运动类型，
		count 如果走码齿位置的码齿数，line_choice 运动曲线选择
  返回值：err OS报错代码或0xFF
*/
uint8_t MCC_Move_Coordinates(DRV_MCC_TYPE*        p_mcc, \
							 double               coordinates, \
							 uint32_t             timeout, \
							 MCC_STATUS_ENUM      status, \
							 uint8_t              count, \
							 MCC_LINE_CHOICE_ENUM line_choice)
{
	uint8_t err                  = OS_ERR_NONE;
	int32_t subdivision_location = MCC_CalcSubdivisionStep(p_mcc, coordinates);	/*换算为脉冲数*/

	err = _MCC_Move_Location(p_mcc, subdivision_location, timeout, status, count, line_choice);
	
	return err;
}

/* 单个组件按预设位置编号运动
  ---------------------------------
  入口：p_mcc 电机结构体，preset_location 预设位置参数表，
		timeout 超时时间，status 运动类型，
        count 如果走码齿位置的码齿数，line_choice 运动曲线选择
  返回值：err OS报错代码或0xFF
*/
uint8_t MCC_Move_PresetLocation(DRV_MCC_TYPE*     p_mcc, \
								MCC_PRESET_LOCATION* preset_location, \
								uint32_t             timeout, \
								MCC_STATUS_ENUM      status, \
								uint8_t              count, \
								MCC_LINE_CHOICE_ENUM line_choice)
{
	uint8_t err                  = OS_ERR_NONE;
	int32_t subdivision_location = 0;	/*目标脉冲坐标（有正负）*/
	uint8_t dir = 0;	/*方向*/
	
	p_mcc->preset_location_now = preset_location;	/*记录当前目标预设位置*/
	
	subdivision_location = MCC_CalcSubdivisionStep(p_mcc, preset_location->data);	/*目标坐标换算为脉冲坐标*/
	
	/*判断运动方向*/
	if((subdivision_location - p_mcc->position) < 0)		/*复位方向为坐标负方向*/
	{		
		dir = p_mcc->reset_dir;
	}
	else if((subdivision_location - p_mcc->position) > 0)
	{
		dir = !(p_mcc->reset_dir);
	}
	else
	{
		return err;	/*step等于0，直接返回完成*/
	}
	
	switch(status)
	{
		case MCC_MOVE_DISTANCE:	/*开环*/
			break;
		
		case MCC_MOVE_D_CODE:	/*闭环*/
			if(dir == p_mcc->reset_dir)	/*负方向预减速*/
				subdivision_location = MCC_CalcSubdivisionStep(p_mcc, preset_location->reverse_pre_deceleration_data);	/*换算为脉冲坐标*/
			else						/*正方向预减速*/
				subdivision_location = MCC_CalcSubdivisionStep(p_mcc, preset_location->forward_pre_deceleration_data);	/*换算为脉冲坐标*/
			break;
		
		default:
			return 0xFF;	/*出错*/
	}

	
	err = _MCC_Move_Location(p_mcc, subdivision_location, timeout, status, count, line_choice);	/*按坐标运动*/
	
	return err;
}

/* 单个组件按码齿数运动
  ---------------------------------
  入口：p_mcc 电机结构体，preset_location 预设位置参数表，
		timeout 超时时间，status 运动类型，
        count 如果走码齿位置的码齿数，line_choice 运动曲线选择
  返回值：err OS报错代码或0xFF
*/
uint8_t MCC_Move_DCodeCount(DRV_MCC_TYPE*     p_mcc, \
								uint8_t                 dir, \
								MCC_PRESET_LOCATION* preset_location, \
								uint32_t             timeout, \
								MCC_STATUS_ENUM      status, \
								uint8_t              count, \
								MCC_LINE_CHOICE_ENUM line_choice)
{
	uint8_t err                  = OS_ERR_NONE;
	int32_t subdivision_location = 0;	/*目标脉冲坐标（有正负）*/
	int32_t temp = 0;
	
	p_mcc->preset_location_now = preset_location;	/*记录当前目标预设位置*/
		
		
	switch(status)
	{
		case MCC_MOVE_DISTANCE:	/*开环*/
			subdivision_location = MCC_CalcSubdivisionStep(p_mcc, preset_location->data) * count;	/*目标坐标换算为脉冲坐标*/
			break;
		
		case MCC_MOVE_D_CODE:	/*闭环*/
//			if(count <= 50)
//			{
				subdivision_location = MCC_CalcSubdivisionStep(p_mcc, preset_location->data) * count;	/*目标坐标换算为脉冲坐标*/
				/*减去与预减速坐标的差值*/
				if(dir == 0)
					temp = MCC_CalcSubdivisionStep(p_mcc, p_mcc->preset_location_now->data) - MCC_CalcSubdivisionStep(p_mcc, p_mcc->preset_location_now->forward_pre_deceleration_data);
				else
					temp = MCC_CalcSubdivisionStep(p_mcc, p_mcc->preset_location_now->data) - MCC_CalcSubdivisionStep(p_mcc, p_mcc->preset_location_now->reverse_pre_deceleration_data);
				subdivision_location -= abs(temp);
//			}
//			else
//			{
//				subdivision_location = MCC_CalcSubdivisionStep(p_mcc, preset_location->data) * (count - 1);	/*目标坐标换算为脉冲坐标*/
//			}			
			break;
		
		default:
			return 0xFF;	/*出错*/
	}
	
	err = _MCC_Move_SubdivisionStep(p_mcc, dir, subdivision_location, timeout, status, count, line_choice);	/*按坐标运动*/
	
	return err;
}

/* 单个组件复位
  ---------------------------------
  入口：p_mcc 电机结构体，timeout 超时时间(ms)
  返回值：err, OS报错代码
*/
uint8_t MCC_Reset(DRV_MCC_TYPE* p_mcc, MCC_PRESET_LOCATION* preset_location, uint32_t timeout)
{
	INT8U err = OS_ERR_NONE;
		
	if((p_mcc->reset_status) != BSP_ReadPin(p_mcc->reset_port_number, p_mcc->reset_pin_number))	/*如果光耦没有在复位状态，则寻找复位状态*/
    {
		if(p_mcc->position == 0)	/*当前坐标为0，慢速接近*/
		{
			err = _MCC_Move_SubdivisionStep(p_mcc, \
											p_mcc->reset_dir, \
											p_mcc->max_step, \
											timeout, \
											MCC_MOVE_RESET, \
											0, \
											MCC_LINE_SLOW);
		}
		else						/*当前坐标不为0，快速接近*/
		{
			if(p_mcc->type == MCC_F)	/*无限旋转机构复位，慢速接近*/
			{
				p_mcc->position = 0;
				err = _MCC_Move_SubdivisionStep(p_mcc, \
												p_mcc->reset_dir, \
												p_mcc->max_step, \
												timeout, \
												MCC_MOVE_RESET, \
												0, \
												MCC_LINE_SLOW);
			}
			else
			{
				if(preset_location == NULL)	/*未传入复位点坐标，预减速按照当前行程的十分之一表示*/
				{
					err = _MCC_Move_SubdivisionStep(p_mcc, \
													p_mcc->reset_dir, \
													abs(p_mcc->position) * 9 / 10, /*加大复位缓冲距离*/\
													timeout, \
													MCC_MOVE_RESET, \
													0, \
													MCC_LINE_AUTO);
				}
				else	/*传入了复位点坐标，预减速按照复位点的预减速参数进行*/
				{
					uint32_t temp = abs(MCC_CalcSubdivisionStep(p_mcc, preset_location->reverse_pre_deceleration_data));
					
					err = _MCC_Move_SubdivisionStep(p_mcc, \
													p_mcc->reset_dir, \
													(abs(p_mcc->position) < temp) ? \
													(abs(p_mcc->position) * 9 / 10) : \
													(abs(p_mcc->position) - temp), \
													timeout, \
													MCC_MOVE_RESET, \
													0, \
													MCC_LINE_AUTO);
				}
			}
		}
		p_mcc->position        = 0;	/*电机当前坐标置零*/
		p_mcc->motor->position = 0;
		p_mcc->location        = 0;
    }
	else																						/*如果光耦在复位状态，先退出，再复位*/
	{
		if(p_mcc->position == 0)	/*当前坐标为0，慢速接近*/
		{
			err = _MCC_Move_SubdivisionStep(p_mcc, \
											!p_mcc->reset_dir, \
											p_mcc->max_step, \
											timeout, \
											MCC_MOVE_RESET, \
											0, \
											MCC_LINE_SLOW);
		}
		else						/*当前坐标不为0，快速接近*/
		{
			if(p_mcc->type == MCC_F)	/*无限旋转机构复位，慢速接近*/
			{
				p_mcc->position = 0;
				err = _MCC_Move_SubdivisionStep(p_mcc, \
												!p_mcc->reset_dir, \
												p_mcc->max_step, \
												timeout, \
												MCC_MOVE_RESET, \
												0, \
												MCC_LINE_SLOW);
			}
			else
			{
				if(preset_location == NULL)	/*未传入复位点坐标，预减速按照当前行程的十分之一表示*/
				{
					err = _MCC_Move_SubdivisionStep(p_mcc, \
													!p_mcc->reset_dir, \
													abs(p_mcc->position) * 9 / 10, /*加大复位缓冲距离*/\
													timeout, \
													MCC_MOVE_RESET, \
													0, \
													MCC_LINE_AUTO);
				}
				else	/*传入了复位点坐标，预减速按照复位点的预减速参数进行*/
				{
					uint32_t temp = abs(MCC_CalcSubdivisionStep(p_mcc, preset_location->forward_pre_deceleration_data));
					
					err = _MCC_Move_SubdivisionStep(p_mcc, \
													!p_mcc->reset_dir, \
													(abs(p_mcc->position) < temp) ? \
													(abs(p_mcc->position) * 9 / 10) : \
													(abs(p_mcc->position) - temp), \
													timeout, \
													MCC_MOVE_RESET, \
													0, \
													MCC_LINE_AUTO);
				}
			}
		}
		if(OS_ERR_NONE == err)		/*未出错*/
		{
			OSTimeDlyHMSM(0, 0, 0, 200);	/*延时200ms避免抖动*/
			p_mcc->position = 0;
			err = _MCC_Move_SubdivisionStep(p_mcc, \
											p_mcc->reset_dir, \
											p_mcc->max_step, \
											timeout, \
											MCC_MOVE_RESET, \
											0, \
											MCC_LINE_SLOW);
		}
		p_mcc->position        = 0;	/*电机当前坐标置零*/
		p_mcc->motor->position = 0;
		p_mcc->location        = 0;
	}
	
    return err;
}

/* 获取复位光耦状态
  ---------------------------------
  入口：p_mcc 电机结构体
  返回值：1 复位状态，0 未复位
*/
bool MCC_Is_ResetOpStatus(DRV_MCC_TYPE* p_mcc)
{
	if((p_mcc->reset_status) != BSP_ReadPin(p_mcc->reset_port_number, p_mcc->reset_pin_number))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/* 获取码齿间隔
  ---------------------------------
  入口：电机结构体，目标位置脉冲坐标
  返回值：码齿间隔数
*/
uint16_t MCC_GetDCodeInterval(DRV_MCC_TYPE* p_mcc, int32_t location)
{
	uint16_t d_code_number = 0;	/*记录码齿数*/
	int32_t preset_location = 0;	/*记录预设位置脉冲坐标*/
	uint8_t i, j, k;
	
	if(p_mcc->position < location)	/*当前位置小于目标位置*/
	{
		/*先找当前位置*/
		for(i = 0; i < MCC_PRESET_LOCATION_MAX_NUMBER; i++)
		{
			if((p_mcc->preset_location + i)->en_close_cycle == UNDEFINED_CLOSE_CYCLE)	/*未找到*/
			{
				return 0;
			}
			/*依次获取预设位置反向预减速坐标*/
			preset_location = MCC_CalcSubdivisionStep(p_mcc, (p_mcc->preset_location + i)->reverse_pre_deceleration_data);
			if(p_mcc->position <= preset_location)/*找到当前位置*/
			{
				/*再找目标位置*/
				for(j = 0; j < MCC_PRESET_LOCATION_MAX_NUMBER; j++)
				{
					if((p_mcc->preset_location + j)->en_close_cycle == UNDEFINED_CLOSE_CYCLE)	/*未找到*/
					{
						return 0;
					}
					/*依次获取预设位置反向预减速坐标*/
					preset_location = MCC_CalcSubdivisionStep(p_mcc, (p_mcc->preset_location + j)->reverse_pre_deceleration_data);
					if(location <= preset_location)/*找到目标位置*/
					{
						/*数两个位置之间的码齿数*/
						for(k = i; k < j; k++)
						{
							if((p_mcc->preset_location + k)->en_close_cycle == ENABLE_CLOSE_CYCLE)
							{
								d_code_number++;
							}
						}
						return d_code_number;
					}					
				}
			}
		}
	}
	else		/*当前位置大于目标位置*/
	{
		/*先找目标位置*/
		for(i = 0; i < MCC_PRESET_LOCATION_MAX_NUMBER; i++)
		{
			if((p_mcc->preset_location + i)->en_close_cycle == UNDEFINED_CLOSE_CYCLE)	/*未找到*/
			{
				return 0;
			}
			/*依次获取预设位置反向预减速坐标*/
			preset_location = MCC_CalcSubdivisionStep(p_mcc, (p_mcc->preset_location + i)->reverse_pre_deceleration_data);
			if(location <= preset_location)	/*找到目标位置*/
			{
				/*再找当前位置*/
				for(j = 0; j < MCC_PRESET_LOCATION_MAX_NUMBER; j++)
				{
					if((p_mcc->preset_location + j)->en_close_cycle == UNDEFINED_CLOSE_CYCLE)	/*未找到*/
					{
						return 0;
					}
					/*依次获取预设位置反向预减速坐标*/
					preset_location = MCC_CalcSubdivisionStep(p_mcc, (p_mcc->preset_location + j)->reverse_pre_deceleration_data);
					if(p_mcc->position <= preset_location)/*找到当前位置*/
					{
						/*数两个位置之间的码齿数*/
						for(k = i; k < j; k++)
						{
							if((p_mcc->preset_location + k)->en_close_cycle == ENABLE_CLOSE_CYCLE)
							{
								d_code_number++;
							}
						}
						return d_code_number;
					}					
				}
			}
		}
	}
	
	return 0;
}

/* 按照实际距离或者角度换算脉冲数
  ---------------------------------
  换算的脉冲数已规整到整步

  入口：p_mcc 电机结构体，distance 距离/角度（单位°或mm）
  返回值：转换为脉冲数
*/
int32_t MCC_CalcSubdivisionStep(DRV_MCC_TYPE* p_mcc, double distance)
{
	/*需要走的距离 / 电机一圈对应的距离 => 电机需要走的圈数 * 电机一圈对应的步数 => 电机需要走的步数 * 细分 => 输出脉冲数*/
	int32_t subdivision_step = (int32_t)((distance / (p_mcc->ratio)) * ((double)360 / (p_mcc->step_angle)) * (double)(p_mcc->subdivision));
	uint8_t flag = (subdivision_step > 0) ? 0 : 1;
	
	/*规整到整步，超过细分一半的进一，不足细分一半的舍去*/
	if((abs(subdivision_step) % p_mcc->subdivision) >= (p_mcc->subdivision / 2))
	{
		if(flag == 0)	/*正的*/
			subdivision_step = subdivision_step + (p_mcc->subdivision - (abs(subdivision_step) % p_mcc->subdivision));
		else			/*负的*/
			subdivision_step = subdivision_step - (p_mcc->subdivision - (abs(subdivision_step) % p_mcc->subdivision));
	}
	else
	{
		if(flag == 0)	/*正的*/
			subdivision_step = subdivision_step - (abs(subdivision_step) % p_mcc->subdivision);
		else			/*负的*/
			subdivision_step = subdivision_step + (abs(subdivision_step) % p_mcc->subdivision);
			
	}
	
	return subdivision_step;
}

/* 设置位置参数
  ---------------------------------
  入口：p_mcc 电机结构体，location_num 位置编号，
  en_close_cycle 是否有码齿，data 坐标参数值（单位为mm/°/uL）
  正/反向预减速坐标参数值（单位为mm/°/uL），正/反向补偿参数值（单位为mm/°/uL）
  返回值：true 成功，flase 失败
*/
bool MCC_SetPresetLocation(DRV_MCC_TYPE* p_mcc, uint8_t location_num, 
							MCC_CLOSE_CYCLE_STATUS_ENUM en_close_cycle, double data,
							double forward_pre_deceleration_data, double reverse_pre_deceleration_data,
							double forward_compensation, double reverse_compensation)
{
	(p_mcc->preset_location)[location_num].en_close_cycle = en_close_cycle;
	(p_mcc->preset_location)[location_num].data = data;
	(p_mcc->preset_location)[location_num].forward_pre_deceleration_data = forward_pre_deceleration_data;		/*正向预减速坐标值，单位°/mm/uL*/
	(p_mcc->preset_location)[location_num].reverse_pre_deceleration_data = reverse_pre_deceleration_data;		/*反向预减速坐标值，单位°/mm/uL*/
	(p_mcc->preset_location)[location_num].forward_compensation = ((forward_compensation >= 0) ? \
																  MCC_CalcSubdivisionStep(p_mcc, forward_compensation) /  p_mcc->subdivision : \
																  (-MCC_CalcSubdivisionStep(p_mcc, forward_compensation) /  p_mcc->subdivision));	/*正向补偿值，单位整步*/
	(p_mcc->preset_location)[location_num].reverse_compensation = ((reverse_compensation >= 0) ? \
																  MCC_CalcSubdivisionStep(p_mcc, reverse_compensation) /  p_mcc->subdivision : \
																  (-MCC_CalcSubdivisionStep(p_mcc, reverse_compensation) /  p_mcc->subdivision));	/*反向补偿值，单位°/mm/uL*/
	
	return true;
}

/* 设置复位补偿参数
  ---------------------------------
	入口：p_mcc 电机结构体，reset_compensation 补偿值（单位为整步）
  返回值：true 成功，flase 失败
*/
bool MCC_SetCompensation(DRV_MCC_TYPE* p_mcc, int32_t reset_compensation)
{
	p_mcc->compensation->reset_compensation = reset_compensation;
	
	return true;
}

/* 设置加减速参数
  ---------------------------------
	入口：p_mcc 电机结构体，start_fre 启动频率，end_fre 最高频率，ech_lader_step 每台阶微步，up_max_lader 最大加速台阶，s_par S型曲线的形状参数，ch 0高速曲线 1低速曲线
  返回值：true 成功，flase 失败
*/
bool MCC_SetStempMotorLine(DRV_MCC_TYPE* p_mcc, uint32_t start_fre,	uint32_t end_fre,	uint32_t ech_lader_step, uint32_t up_max_lader,	uint32_t s_par, uint8_t ch)
{
	switch(ch)
	{
		case 0:
			p_mcc->motor->line_high->start_fre = start_fre;
			p_mcc->motor->line_high->end_fre = end_fre;
			p_mcc->motor->line_high->ech_lader_step = ech_lader_step;
			p_mcc->motor->line_high->up_max_lader = up_max_lader;
			p_mcc->motor->line_high->s_par = s_par;
			CalcMotorFre(p_mcc->motor->subdivision, p_mcc->motor->line_high, p_mcc->motor->line_hig_fre, p_mcc->motor->lineh_laderstep);	/*高速运动频率表计算*/
			break;
	
		case 1:
			p_mcc->motor->line_mid->start_fre = start_fre;
			p_mcc->motor->line_mid->end_fre = end_fre;
			p_mcc->motor->line_mid->ech_lader_step = ech_lader_step;
			p_mcc->motor->line_mid->up_max_lader = up_max_lader;
			p_mcc->motor->line_mid->s_par = s_par;
			CalcMotorFre(p_mcc->motor->subdivision, p_mcc->motor->line_mid, p_mcc->motor->line_mid_fre, p_mcc->motor->linem_laderstep);	/*中速运动频率表计算*/
			break;
		
		case 2:
			p_mcc->motor->line_low->start_fre = start_fre;
			p_mcc->motor->line_low->end_fre = end_fre;
			p_mcc->motor->line_low->ech_lader_step = ech_lader_step;
			p_mcc->motor->line_low->up_max_lader = up_max_lader;
			p_mcc->motor->line_low->s_par = s_par;
			CalcMotorFre(p_mcc->motor->subdivision, p_mcc->motor->line_low, p_mcc->motor->line_low_fre,	p_mcc->motor->linel_laderstep);	/*低速运动频率表计算*/
			break;
	}
	
	return true;
}

/* 设置加减速参数(5段S曲线)
  ---------------------------------
	入口：p_mcc 电机结构体，start_fre 启动频率，end_fre 最高频率，ech_lader_step 每台阶微步，up_max_lader 最大加速台阶，s_par S型曲线的形状参数，ch 0高速曲线 1低速曲线
  返回值：true 成功，flase 失败
*/
bool MCC_SetStempMotorLine5(DRV_MCC_TYPE* p_mcc, uint32_t start_fre,	uint32_t end_fre,	uint32_t ech_lader_step, uint32_t up_max_lader,	uint32_t s_par, uint8_t ch)
{
	switch(ch)
	{
		case 0:
			p_mcc->motor->line_high->start_fre = start_fre;
			p_mcc->motor->line_high->end_fre = end_fre;
			p_mcc->motor->line_high->ech_lader_step = ech_lader_step;
			p_mcc->motor->line_high->up_max_lader = up_max_lader;
			p_mcc->motor->line_high->s_par = s_par;
			CalcMotorFre5(p_mcc->motor->subdivision, p_mcc->motor->line_high, p_mcc->motor->line_hig_fre,p_mcc->motor->lineh_laderstep);	/*高速运动频率表计算*/
			break;
	
		case 1:
			p_mcc->motor->line_mid->start_fre = start_fre;
			p_mcc->motor->line_mid->end_fre = end_fre;
			p_mcc->motor->line_mid->ech_lader_step = ech_lader_step;
			p_mcc->motor->line_mid->up_max_lader = up_max_lader;
			p_mcc->motor->line_mid->s_par = s_par;
			CalcMotorFre5(p_mcc->motor->subdivision, p_mcc->motor->line_mid, p_mcc->motor->line_mid_fre,p_mcc->motor->linem_laderstep);	/*中速运动频率表计算*/
			break;
		
		case 2:
			p_mcc->motor->line_low->start_fre = start_fre;
			p_mcc->motor->line_low->end_fre = end_fre;
			p_mcc->motor->line_low->ech_lader_step = ech_lader_step;
			p_mcc->motor->line_low->up_max_lader = up_max_lader;
			p_mcc->motor->line_low->s_par = s_par;
			CalcMotorFre5(p_mcc->motor->subdivision, p_mcc->motor->line_low, p_mcc->motor->line_low_fre,p_mcc->motor->linel_laderstep);	/*低速运动频率表计算*/
			break;
	}
	
	return true;
}

/* 设置加减速参数(梯形曲线)
  ---------------------------------
	入口：p_mcc 电机结构体，start_fre 启动频率，end_fre 最高频率，ech_lader_step 每台阶微步，up_max_lader 最大加速台阶，s_par S型曲线的形状参数，ch 0高速曲线 1低速曲线
  返回值：true 成功，flase 失败
*/
bool MCC_SetStempMotorLineT(DRV_MCC_TYPE* p_mcc, uint32_t start_fre,	uint32_t end_fre,	uint32_t ech_lader_step, uint32_t up_max_lader,	uint32_t s_par, uint8_t ch)
{
	switch(ch)
	{
		case 0:
			p_mcc->motor->line_high->start_fre = start_fre;
			p_mcc->motor->line_high->end_fre = end_fre;
			p_mcc->motor->line_high->ech_lader_step = ech_lader_step;
			p_mcc->motor->line_high->up_max_lader = up_max_lader;
			p_mcc->motor->line_high->s_par = s_par;
			CalcMotorFreT(p_mcc->motor->subdivision, p_mcc->motor->line_high, p_mcc->motor->line_hig_fre, p_mcc->motor->lineh_laderstep);	/*高速运动频率表计算*/
			break;
	
		case 1:
			p_mcc->motor->line_mid->start_fre = start_fre;
			p_mcc->motor->line_mid->end_fre = end_fre;
			p_mcc->motor->line_mid->ech_lader_step = ech_lader_step;
			p_mcc->motor->line_mid->up_max_lader = up_max_lader;
			p_mcc->motor->line_mid->s_par = s_par;
			CalcMotorFreT(p_mcc->motor->subdivision, p_mcc->motor->line_mid, p_mcc->motor->line_mid_fre, p_mcc->motor->linem_laderstep);	/*中速运动频率表计算*/
			break;
		
		case 2:
			p_mcc->motor->line_low->start_fre = start_fre;
			p_mcc->motor->line_low->end_fre = end_fre;
			p_mcc->motor->line_low->ech_lader_step = ech_lader_step;
			p_mcc->motor->line_low->up_max_lader = up_max_lader;
			p_mcc->motor->line_low->s_par = s_par;
			CalcMotorFreT(p_mcc->motor->subdivision, p_mcc->motor->line_low, p_mcc->motor->line_low_fre,p_mcc->motor->linel_laderstep);	/*低速运动频率表计算*/
			break;
	}
	
	return true;
}

/* 内部函数
 ********************************************************/

 /* 单个组件按细分脉冲数运动
  ---------------------------------
  按照细分后的脉冲数运动

  入口：p_mcc 电机结构体，dir 方向，step 脉冲数，
		timeout 超时时间，status 运动类型，count 如果走码齿位置的码齿数，
		line_choice 运动曲线选择
  返回值：err OS报错代码或0xFF
*/
static uint8_t _MCC_Move_SubdivisionStep(DRV_MCC_TYPE* p_mcc, \
					  uint8_t  dir, \
					  uint32_t step, \
					  uint32_t timeout, \
					  MCC_STATUS_ENUM status, \
					  uint8_t  count, \
					  MCC_LINE_CHOICE_ENUM line_choice)
{
	uint8_t err = OS_ERR_NONE;
	uint32_t slow_step = 0;
	
	/*设置目标坐标*/
	if(dir != p_mcc->reset_dir)		/*复位方向为坐标负方向*/
	{
		if(p_mcc->type != MCC_F)	/*不是无限旋转机构的，需要判断最大行程*/
		{
			if(abs(p_mcc->position + (int32_t)step) > p_mcc->max_step)	/*目标位置大于最大行程*/
			{
				return 0xFF;
			}
			else
			{
				p_mcc->location = p_mcc->position + (int32_t)step;	/*设置目标位置坐标*/
			}
		}
		else
		{
			p_mcc->location = p_mcc->position + (int32_t)step;	/*设置目标位置坐标*/
		}
	}
	else
	{
		if(p_mcc->type != MCC_F)	/*不是无限旋转机构的，需要判断最大行程*/
		{
			if(abs(p_mcc->position - (int32_t)step) > p_mcc->max_step)	/*目标位置大于最大行程*/
			{
				return 0xFF;
			}
			else
			{
				p_mcc->location = p_mcc->position - (int32_t)step;	/*设置目标位置坐标*/
			}
		}
		else
		{
			p_mcc->location = p_mcc->position - (int32_t)step;	/*设置目标位置坐标*/
		}
	}
	
	/*设置运动类型*/
	p_mcc->status   = status;		/*获取运动类型*/
	p_mcc->d_code_count = count;	/*获取走码齿数*/
	
	/*设置慢速运动脉冲数*/
	if(status == MCC_MOVE_DISTANCE || status == MCC_MOVE_LOW)	/*走固定脉冲，不需要慢速运动*/
	{
		slow_step = 0;
	}	
	else if(status == MCC_MOVE_RESET)	/*复位的，慢速运动步数为做大位移减去目标坐标*/
	{
		slow_step = p_mcc->max_step - abs(p_mcc->location);
	}
	else							/*走信号位置的，慢速运动步数为正反方向预减速坐标差值*/
	{
		if((p_mcc->type == MCC_F) || (p_mcc->type == MCC_G))	/*无限旋转机构及直线码齿机构走信号位置的，按最大脉冲数走*/
			slow_step = p_mcc->max_step;
		else
			slow_step = abs(MCC_CalcSubdivisionStep(p_mcc, p_mcc->preset_location_now->reverse_pre_deceleration_data) - MCC_CalcSubdivisionStep(p_mcc, p_mcc->preset_location_now->forward_pre_deceleration_data));
	}
	
	switch(line_choice)
	{
		case MCC_LINE_AUTO:
			
			if(step < (p_mcc->motor->line_low->ech_lader_step * p_mcc->motor->line_low->up_max_lader * 2.5))
			{
				Motor_MoveRL_Begin(p_mcc->motor, \
								   dir, \
								   step, \
								   slow_step, \
								   (p_mcc->motor->line_low_fre[0] < p_mcc->motor->line_hig_fre[0]) ? p_mcc->motor->line_low_fre[0] :p_mcc->motor->line_hig_fre[0]);	/*小于低速运动曲线加减速脉冲总数的按最低速运动*/
			}
			else if(step < (p_mcc->motor->line_mid->ech_lader_step * p_mcc->motor->line_mid->up_max_lader * 2.5))	/*p_mcc->motor->subdivision * */
			{
				Motor_MoveSP_Begin(p_mcc->motor, \
								   dir, \
								   step, \
								   slow_step, \
								   0);									/*小于中速运动曲线加减速脉冲总数的按低速曲线运动*/
			}
			else if(step < (p_mcc->motor->line_high->ech_lader_step * p_mcc->motor->line_high->up_max_lader * 2.5))	/*p_mcc->motor->subdivision * */
			{
				Motor_MoveSP_Begin(p_mcc->motor, \
								   dir, \
								   step, \
								   slow_step, \
								   1);									/*小于高速运动曲线加减速脉冲总数的按中速曲线运动*/
			}
			else
			{
				Motor_MoveSP_Begin(p_mcc->motor, \
								   dir, \
								   step, \
								   slow_step, \
								   2);									/*大于高速运动曲线加减速脉冲总数的按高速曲线运动*/
			}
			
			break;
			
		case MCC_LINE_SLOW:
			Motor_MoveRL_Begin(p_mcc->motor, \
							   dir, \
							   step, \
							   slow_step, \
							   (p_mcc->motor->line_low_fre[0] < p_mcc->motor->line_hig_fre[0]) ? p_mcc->motor->line_low_fre[0] :p_mcc->motor->line_hig_fre[0]);
			break;
		case MCC_LINE_LOW:
			Motor_MoveSP_Begin(p_mcc->motor, \
							   dir, \
							   step, \
							   slow_step, \
							   0);
			break;
		case MCC_LINE_MID:
			Motor_MoveSP_Begin(p_mcc->motor, \
							   dir, \
							   step, \
							   slow_step, \
							   1);
			break;
		case MCC_LINE_HIGH:
			Motor_MoveSP_Begin(p_mcc->motor, \
							   dir, \
							   step, \
							   slow_step, \
							   2);
			break;
	}		
	
	/*等待运动完成*/
	OSSemPend(p_mcc->sem, timeout, &err);
	
	p_mcc->status = MCC_IDLE;	/*恢复空闲状态*/
	
	if(err != OS_ERR_NONE)	/*运动出错*/
	{
		/*电机停止*/
		Motor_Move_Stop(p_mcc->motor);
		/*坐标清零，以解决撞击后复位不成功的问题*/
		p_mcc->motor->position = 0;
		p_mcc->position = 0;
	}
	else					/*运动成功*/
	{
		/*更新坐标*/
		if(p_mcc->reset_dir == MCC_RESET_POSITIVE)
		{
			p_mcc->position = -p_mcc->motor->position;
		}
		else
		{
			p_mcc->position = p_mcc->motor->position;
		}
	}
	
	if(p_mcc->strike_flag == 1)	/*发生撞击*/
	{
		p_mcc->strike_flag = 0;
		p_mcc->d_code_flag = 0;
		if((status == MCC_MOVE_D_CODE) && (p_mcc->type == MCC_E))	/*是针在液面检测*/
		{
			if((p_mcc->d_code_status) == BSP_ReadPin(p_mcc->d_code_port_number, p_mcc->d_code_pin_number))	/*检测到了液面*/
			{
				err = 0xCC;	/*撞针且检测到液面判断为样本量不足*/
			}
			else
			{
				err = 0xEE;
			}
		}
		else
		{
			err = 0xEE;
		}
	}
	else if((status == MCC_MOVE_D_CODE) && (p_mcc->d_code_flag == 0))	/*没有找到码齿*/
	{
		err = 0xDD;
	}
	else if((status == MCC_MOVE_D_CODE) && (p_mcc->d_code_flag == 1))	/*找到码齿了*/
	{
		p_mcc->d_code_flag = 0;
	}
	
	return err;
}

/* 单个组件按脉冲坐标运动
  ---------------------------------
  按细分后的脉冲坐标运动

  入口：p_mcc 电机结构体，location 脉冲坐标（相对于复位点），
		timeout 超时时间，status 运动类型，count 如果走码齿位置的码齿数，
		line_choice 运动曲线选择
  返回值：err OS报错代码或0xFF
*/
static uint8_t _MCC_Move_Location(DRV_MCC_TYPE* p_mcc, \
						  int32_t location, \
						  uint32_t timeout, \
						  MCC_STATUS_ENUM status, \
						  uint8_t  count, \
						  MCC_LINE_CHOICE_ENUM line_choice)
{
	uint8_t err = OS_ERR_NONE;
	uint32_t step = 0;
	uint32_t dir = 0;
	
	if(abs(location) > p_mcc->max_step)	/*目标位置大于最大行程*/
	{
		return 0xFF;
	}
		
	/*计算运动脉冲数*/
	step = abs(location - p_mcc->position);	/*获取要走的步数*/
	
	/*计算运动方向*/
	if((location - p_mcc->position) < 0)		/*复位方向为坐标负方向*/
	{		
		dir = p_mcc->reset_dir;
	}
	else if((location - p_mcc->position) > 0)
	{
		dir = !(p_mcc->reset_dir);
	}
	else
	{
		return err;	/*step等于0，直接返回完成*/
	}
	
	err = _MCC_Move_SubdivisionStep(p_mcc, dir, step, timeout, status, count, line_choice);
	
	return err;
}

/* MCC 复位中断处理函数
  ------------------------------------
  入口：p_unit 组件结构体
*/
static void _Mcc_ResetExtiIrqHandler(DRV_MCC_TYPE* p_unit)
{
	uint8_t compensate = 0;	/*复位时补偿到一个整步*/
	
	if(p_unit->status == MCC_MOVE_RESET)		/*MCC正在复位*/
	{
		Motor_Move_Stop(p_unit->motor);		/*停止电机动作*/
		if((p_unit->reset_status) == BSP_ReadPin(p_unit->reset_port_number, p_unit->reset_pin_number))	/*到了复位状态，走复位补偿*/
		{
//			compensate = p_unit->position % p_unit->subdivision;	/*计算整步补偿，即多了多少细分脉冲到一整步*/
			p_unit->status = MCC_MOVE_DISTANCE;
			Motor_MoveRL_Begin(p_unit->motor, \
							   p_unit->motor->dir_buffer, \
							   p_unit->compensation->reset_compensation * p_unit->subdivision + (p_unit->subdivision - compensate), \
							   0, \
							   p_unit->motor->line_low_fre[0]);
		}
		else																			/*退出光耦，直接停止*/
		{
			p_unit->status = MCC_MOVE_DISTANCE;
			Motor_MoveRL_Begin(p_unit->motor, \
							   p_unit->motor->dir_buffer, \
							   10, \
							   0, \
							   p_unit->motor->line_low_fre[0]);	/*走10步冲出去*/
		}
	}
}

/* MCC 单码齿中断处理函数
  ------------------------------------
  入口：p_unit 组件结构体
*/
static void _Mcc_SingleDCodeExtiIrqHandler(DRV_MCC_TYPE* p_unit)
{
	uint8_t compensate = 0;	/*找码齿时补偿到一个整步*/
	int32_t temp = 0;
	
	if(p_unit->status == MCC_MOVE_D_CODE)		/*MCC正在找码齿*/
	{
		if((p_unit->d_code_status) == BSP_ReadPin(p_unit->d_code_port_number, p_unit->d_code_pin_number))	/*到了码齿位置*/
		{
			Motor_Move_Stop(p_unit->motor);		/*停止电机动作*/
//			compensate = p_unit->position % p_unit->subdivision;	/*计算整步补偿，即多了多少细分脉冲到一整步*/
			p_unit->status = MCC_MOVE_DISTANCE;
			if((p_unit->type == MCC_E) && (p_unit->motor->status == MOTOR_RUNING))	/*如果是液面检测，调用减速过程*/
			{
				p_unit->d_code_flag = 1;	/*检测到液面*/
				Motor_MoveSD_Begin(p_unit->motor, \
									p_unit->motor->dir_buffer, \
									p_unit->motor->line_high->ech_lader_step * p_unit->motor->line_high->up_max_lader, /*加速台阶总数*/\
									compensate);
			}
			else
			{
//				Motor_MoveRL_Begin(p_unit->motor, \
//								   p_unit->motor->dir_buffer, \
//								   ((p_unit->motor->dir_buffer != p_unit->reset_dir) ? 
//								   p_unit->preset_location_now->forward_compensation: 
//								   p_unit->preset_location_now->reverse_compensation) * p_unit->subdivision + (p_unit->subdivision - compensate), \
//								   0, \
//								   p_unit->motor->line_low_fre[0]);
				/*20210310 替换上方，实现单码齿闭环按开环坐标定位*/
				temp = MCC_CalcSubdivisionStep(p_unit, p_unit->preset_location_now->data);		/*获取目标坐标*/
				temp = temp - (p_unit->motor->position) * ((p_unit->reset_dir == 0) ? -1 : 1);	/*获取当前坐标和目标坐标的差值*/
				#ifdef XC2000_SAMPLE_STIRRING_AND_REAGENT_STIRRING
				if(abs(temp) > 200) 	/*搅拌旋转丢步判定值改为200*/
				#else
				if(abs(temp) > 1500)	/*XC2000自动清洗模块及三个定位抓手此处为1500，后期考虑结构优化，以实现碰撞检测*//*大于200步认为丢步*/
				#endif
				{
					/*发生丢步*/		/*所有单码齿闭环运动，检测丢步情况，出现丢步立即停止，不进行补偿*/
					p_unit->strike_flag = 1;			/*撞击标志*/
					Motor_MoveRL_Begin(p_unit->motor, \
										p_unit->motor->dir_buffer, \
										 1, \
										 0, \
										 p_unit->motor->line_low_fre[0]);
				}
				else
				{
					p_unit->d_code_flag = 1;			/*检测到码齿*/
					Motor_MoveRL_Begin(p_unit->motor, \
									   (temp > 0) ? !p_unit->reset_dir : p_unit->reset_dir, \
									   abs(temp), \
									   0, \
									   p_unit->motor->line_low_fre[0]);
				}
			}
		}
	}
}

/* MCC 多码齿中断处理函数
  ------------------------------------
  入口：p_unit 组件结构体
*/
static void _Mcc_DCodeExtiIrqHandler(DRV_MCC_TYPE* p_unit)
{
	uint8_t compensate = 0;	/*找码齿时补偿到一个整步*/
	int32_t temp = 0;
	
	if(p_unit->status == MCC_MOVE_D_CODE)		/*MCC正在找码齿*/
	{
		if((p_unit->d_code_status) == BSP_ReadPin(p_unit->d_code_port_number, p_unit->d_code_pin_number))	/*到了码齿位置*/
		{
			if(p_unit->d_code_count > 0)			/*没到位的，码齿计数减一*/
			{
				(p_unit->d_code_count)--;
			}
			
			if(p_unit->d_code_count <= 0)			/*到位后走补偿值++++++++++++++++++++++++++++++++++++*/
			{
				Motor_Move_Stop(p_unit->motor);		/*停止电机动作*/
//				compensate = abs(p_unit->position) % p_unit->subdivision;	/*计算整步补偿，即多了多少细分脉冲到一整步*/
				p_unit->status = MCC_MOVE_DISTANCE;
//				Motor_MoveRL_Begin(p_unit->motor, \
//								   p_unit->motor->dir_buffer, \
//								   ((p_unit->motor->dir_buffer != p_unit->reset_dir) ? 
//								   p_unit->preset_location_now->forward_compensation: 
//								   p_unit->preset_location_now->reverse_compensation) * p_unit->subdivision + (p_unit->subdivision - compensate), \
//								   0, \
//								   p_unit->motor->line_low_fre[0]);
				/*20210310 替换上方，实现多码齿旋转机构闭环按开环坐标定位*/
				if(p_unit->type == MCC_D)	/*有复位信号和多码齿信号的旋转机构，检测丢步情况，出现丢步立即停止，不进行补偿*/
				{
					temp = MCC_CalcSubdivisionStep(p_unit, p_unit->preset_location_now->data);
					temp = temp - (p_unit->motor->position) * ((p_unit->reset_dir == 0) ? -1 : 1);
					if(abs(temp) > 200)	/*大于200步认为丢步*/
					{
						/*发生丢步或撞击*/
						p_unit->strike_flag = 1;			/*撞击标志*/
						Motor_MoveRL_Begin(p_unit->motor, \
											p_unit->motor->dir_buffer, \
											 1, \
											 0, \
											 p_unit->motor->line_low_fre[0]);
					}
					else
					{
						p_unit->d_code_flag = 1;			/*检测到码齿*/
						Motor_MoveRL_Begin(p_unit->motor, \
										   (temp > 0) ? !p_unit->reset_dir : p_unit->reset_dir, \
										   abs(temp), \
										   0, \
										   p_unit->motor->line_low_fre[0]);
					}
				}
				else	/*其他多码齿运动机构（多码齿直线机构和无限旋转机构），不检测丢步情况，出现丢步进行补偿*/
				{
					p_unit->d_code_flag = 1;	/*检测到码齿*/
					Motor_MoveRL_Begin(p_unit->motor, \
									   p_unit->motor->dir_buffer, \
									   ((p_unit->motor->dir_buffer != p_unit->reset_dir) ? 
									   p_unit->preset_location_now->forward_compensation: 
									   p_unit->preset_location_now->reverse_compensation) * p_unit->subdivision + (p_unit->subdivision - compensate), \
									   0, \
									   p_unit->motor->line_low_fre[0]);
				}
			}
		}
	}
}

/* MCC 撞击中断
  ------------------------------------
  入口：p_unit 组件结构体
*/
static void MCC_StrikeExtiIrqHandler(DRV_MCC_TYPE* p_unit)
{
	if((p_unit->status != MCC_IDLE) && (p_unit->status != MCC_MOVE_RESET) && (p_unit->status != MCC_MOVE_LOW))		/*MCC在动，并且不是复位不是匀速脱离*/
	{
		Motor_Move_Stop(p_unit->motor);		/*停止电机动作*/
		p_unit->status = MCC_MOVE_DISTANCE;	/*只有走固定脉冲完成才能退出*/
		p_unit->strike_flag = 1;			/*撞击标志*/
		Motor_MoveRL_Begin(p_unit->motor, \
							p_unit->motor->dir_buffer, \
							 1, \
							 0, \
							 p_unit->motor->line_low_fre[0]);
	}
}

/* 外部接口函数
 ********************************************************/

/* MCC_1 复位中断
  ------------------------------------
*/
void MCC1_ResetExtiIrqHandler(void)
{
	_Mcc_ResetExtiIrqHandler(_gp_MCCunits[MCC_1]);
}
/* MCC_1 码齿中断
  ------------------------------------
*/
void MCC1_DCodeExtiIrqHandler(void)
{
	if((_gp_MCCunits[MCC_1]->type == MCC_B) || (_gp_MCCunits[MCC_1]->type == MCC_C) || (_gp_MCCunits[MCC_1]->type == MCC_E))	/*单码齿*/
	{
		_Mcc_SingleDCodeExtiIrqHandler(_gp_MCCunits[MCC_1]);
	}
	else if((_gp_MCCunits[MCC_1]->type == MCC_D) || (_gp_MCCunits[MCC_1]->type == MCC_F) || (_gp_MCCunits[MCC_1]->type == MCC_G))	/*多码齿*/
	{
		_Mcc_DCodeExtiIrqHandler(_gp_MCCunits[MCC_1]);
	}
}
/* MCC_1 撞击中断
  ------------------------------------
*/
void MCC1_StrikeExtiIrqHandler(void)
{
	MCC_StrikeExtiIrqHandler(_gp_MCCunits[MCC_1]);
}

/* MCC_2 复位中断
  ------------------------------------
*/
void MCC2_ResetExtiIrqHandler(void)
{
	_Mcc_ResetExtiIrqHandler(_gp_MCCunits[MCC_2]);
}
/* MCC_2 码齿中断
  ------------------------------------
*/
void MCC2_DCodeExtiIrqHandler(void)
{
	if((_gp_MCCunits[MCC_2]->type == MCC_B) || (_gp_MCCunits[MCC_2]->type == MCC_C) || (_gp_MCCunits[MCC_2]->type == MCC_E))	/*单码齿*/
	{
		_Mcc_SingleDCodeExtiIrqHandler(_gp_MCCunits[MCC_2]);
	}
	else if((_gp_MCCunits[MCC_2]->type == MCC_D) || (_gp_MCCunits[MCC_2]->type == MCC_F) || (_gp_MCCunits[MCC_2]->type == MCC_G))	/*多码齿*/
	{
		_Mcc_DCodeExtiIrqHandler(_gp_MCCunits[MCC_2]);
	}
}
/* MCC_2 撞击中断
  ------------------------------------
*/
void MCC2_StrikeExtiIrqHandler(void)
{
	MCC_StrikeExtiIrqHandler(_gp_MCCunits[MCC_2]);
}

/* MCC_3复位中断
  ------------------------------------
*/
void MCC3_ResetExtiIrqHandler(void)
{
	_Mcc_ResetExtiIrqHandler(_gp_MCCunits[MCC_3]);
}
/* MCC_3 码齿中断
  ------------------------------------
*/
void MCC3_DCodeExtiIrqHandler(void)
{
	if((_gp_MCCunits[MCC_3]->type == MCC_B) || (_gp_MCCunits[MCC_3]->type == MCC_C) || (_gp_MCCunits[MCC_3]->type == MCC_E))	/*单码齿*/
	{
		_Mcc_SingleDCodeExtiIrqHandler(_gp_MCCunits[MCC_3]);
	}
	else if((_gp_MCCunits[MCC_3]->type == MCC_D) || (_gp_MCCunits[MCC_3]->type == MCC_F) || (_gp_MCCunits[MCC_3]->type == MCC_G))	/*多码齿*/
	{
		_Mcc_DCodeExtiIrqHandler(_gp_MCCunits[MCC_3]);
	}
}
/* MCC_3 撞击中断
  ------------------------------------
*/
void MCC3_StrikeExtiIrqHandler(void)
{
	MCC_StrikeExtiIrqHandler(_gp_MCCunits[MCC_3]);
}

/* MCC_4复位中断
  ------------------------------------
*/
void MCC4_ResetExtiIrqHandler(void)
{
	_Mcc_ResetExtiIrqHandler(_gp_MCCunits[MCC_4]);
}
/* MCC_4 码齿中断
  ------------------------------------
*/
void MCC4_DCodeExtiIrqHandler(void)
{
	if((_gp_MCCunits[MCC_4]->type == MCC_B) || (_gp_MCCunits[MCC_4]->type == MCC_C) || (_gp_MCCunits[MCC_4]->type == MCC_E))	/*单码齿*/
	{
		_Mcc_SingleDCodeExtiIrqHandler(_gp_MCCunits[MCC_4]);
	}
	else if((_gp_MCCunits[MCC_4]->type == MCC_D) || (_gp_MCCunits[MCC_4]->type == MCC_F) || (_gp_MCCunits[MCC_4]->type == MCC_G))	/*多码齿*/
	{
		_Mcc_DCodeExtiIrqHandler(_gp_MCCunits[MCC_4]);
	}
}
/* MCC_4 撞击中断
  ------------------------------------
*/
void MCC4_StrikeExtiIrqHandler(void)
{
	MCC_StrikeExtiIrqHandler(_gp_MCCunits[MCC_4]);
}

/* 下层接口函数
 ********************************************************/
 
/* 电机运动完成
  ------------------------------
  运动完成，释放组件信号量
*/
void __Motor_Done(DRV_STEPMOTOR_TYPE* motor)
{
	switch(motor->id)
	{
		case MOTOR_1:
			OSSemPost(_gp_MCCunits[MCC_1]->sem);
			break;
		case MOTOR_2:
			OSSemPost(_gp_MCCunits[MCC_2]->sem);
			break;
		case MOTOR_3:
			OSSemPost(_gp_MCCunits[MCC_3]->sem);
			break;
		case MOTOR_4:
			OSSemPost(_gp_MCCunits[MCC_4]->sem);
			break;
		default:
			break;
	}
}
