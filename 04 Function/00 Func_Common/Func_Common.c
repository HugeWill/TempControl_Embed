/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——通用功能

  -------------------------------------------------------
   文 件 名   : Func_Common.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年07月19日
   功能描述   : 通用功能定义
   依赖于     : 
 ********************************************************/
 
#include "Func_Common.h"

static DRV_MCC_TYPE*   _gp_M[MCC_BUTT]   = {NULL};	/*运动组件*/
static DRV_VALVE_TYPE* _gp_V[VALVE_BUTT] = {NULL};	/*电磁阀/电磁铁*/
static DRV_PUMP_TYPE*  _gp_P[PUMP_BUTT]  = {NULL};	/*泵/直流电机*/
DRV_AT24C32_TYPE* _gp_EEP = NULL;	/*参数存储器*/

/* 初始化函数
 ********************************************************/

/* 初始化 Func_Common
  ------------------------------
  入口：指令注册表，模块编号
  返回值：true，成功；false，失败
*/
bool Func_Common_Init(DList* dlist, uint8_t ch)
{
	switch(ch)
	{
		case 0:
			Func_PrivateInit1(dlist, _gp_M, _gp_V, _gp_P);	/*模块1私有初始化*/
			return true;
		case 1:
			//Func_PrivateInit2(dlist, _gp_M, _gp_V, _gp_P);	/*模块2私有初始化*/
			return true;
	}
	
	return false;
}

/* 通用指令任务函数
 ********************************************************/

/* 握手
  ------------------------------ OK
  返回值：true，成功；false，失败
*/
bool Func_Cmd_Com_Hello(void* p_buffer)
{
	DRV_CAN_RX_BUFFER_TYPE*          p_buf  = (DRV_CAN_RX_BUFFER_TYPE *)p_buffer;					/*获取缓冲区数据*/
	CMD_COMMON_HEAD_TYPE*            p_msg  = (CMD_COMMON_HEAD_TYPE*)(p_buf->buffer);				/*获取消息内容*/
	
	uint8_t  size          = p_buf->size;			/*获取指令长度*/
	uint8_t  can_id        = p_buf->can_id;			/*获取CAN ID*/
	uint8_t  serial_number = p_msg->serial_number;	/*获取流水号*/
	uint16_t cmd           = p_msg->cmd;			/*获取指令*/
	uint32_t frame_id      = p_msg->frame_id;		/*获取报文ID*/
			
	if(size != 3)
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x05);		/*报错，指令长度错误*/
		return false;
	}
	
	Drv_CANProtocol_SendDoneToBuffer(can_id, serial_number, cmd, frame_id);	/*直接回复执行成功*/
	return true;
}

/* 读位置
  ------------------------------ OK-
  返回值：true，成功；false，失败
*/
bool Func_Cmd_Com_Read_Mcc_Position(void* p_buffer)
{
	DRV_CAN_RX_BUFFER_TYPE*          p_buf  = (DRV_CAN_RX_BUFFER_TYPE *)p_buffer;					/*获取缓冲区数据*/
	CMD_COMMON_HEAD_TYPE*            p_msg  = (CMD_COMMON_HEAD_TYPE*)(p_buf->buffer);				/*获取消息内容*/
	CMD_COMMON_DATA_READ_COORDINATE_TYPE* p_data = (CMD_COMMON_DATA_READ_COORDINATE_TYPE*)(&(p_msg->data));	/*获取数据域*/
	
	uint8_t  size          = p_buf->size;			/*获取指令长度*/
	uint8_t  can_id        = p_buf->can_id;			/*获取CAN ID*/
	uint8_t  serial_number = p_msg->serial_number;	/*获取流水号*/
	uint16_t cmd           = p_msg->cmd;			/*获取指令*/
	uint32_t frame_id      = p_msg->frame_id;		/*获取报文ID*/
	
	DRV_MCC_TYPE* p_mcc    = NULL;					/*获取待操作组件结构体*/
	int32_t  position      = 0;						/*获取坐标值*/
	
	if(size != 3)
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x05);		/*报错，指令长度错误*/
		return false;
	}
	
	if(p_data->mcc_number <= MCC_BUTT)
	{
		p_mcc = _gp_M[p_data->mcc_number - 1];
	}
	else	/*没有这个组件*/
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
		return false;
	}
	
	if(p_mcc == NULL)	/*组件未使用*/
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
		return false;
	}
	
	switch(p_data->expression_form)	/*根据所需数据类型返回值*/
	{
		case 0:	/*细分步数坐标*/
			position = p_mcc->position;
			break;
		
		case 1:	/*角度/距离/液量坐标，包括两位小数*/
			position = (int32_t)(((double)(p_mcc->position) / (double)(p_mcc->subdivision)) * ((p_mcc->step_angle) / (double)360) * (p_mcc->ratio) * 100);
			break;
		
		case 2:	/*信号坐标*/
			break;
	}
	
	Drv_CANProtocol_SendDataToBuffer(can_id, serial_number, cmd, frame_id, position, 50);	/*直接回复执行成功*/
	return true;
}

/* 组件复位
  ------------------------------ OK
  返回值：true，成功；false，失败
*/
bool Func_Cmd_Com_Mcc_Reset(void* p_buffer)
{
	DRV_CAN_RX_BUFFER_TYPE*          p_buf  = (DRV_CAN_RX_BUFFER_TYPE *)p_buffer;					/*获取缓冲区数据*/
	CMD_COMMON_HEAD_TYPE*            p_msg  = (CMD_COMMON_HEAD_TYPE*)(p_buf->buffer);				/*获取消息内容*/
	CMD_COMMON_DATA_MCC_RESET_TYPE*  p_data = (CMD_COMMON_DATA_MCC_RESET_TYPE*)(&(p_msg->data));	/*获取数据域*/
	
	uint8_t  size          = p_buf->size;			/*获取指令长度*/
	uint8_t  can_id        = p_buf->can_id;			/*获取CAN ID*/
	uint8_t  serial_number = p_msg->serial_number;	/*获取流水号*/
	uint16_t cmd           = p_msg->cmd;			/*获取指令*/
	uint32_t frame_id      = p_msg->frame_id;		/*获取报文ID*/
	
	DRV_MCC_TYPE* p_mcc    = NULL;					/*获取待操作组件结构体*/	
	uint8_t  err           = OS_ERR_NONE;
		
	if(size != 3)
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x05);		/*报错，指令长度错误*/
		return false;
	}
	
	if(p_data->mcc_number <= MCC_BUTT)
	{
		p_mcc = _gp_M[p_data->mcc_number - 1];
	}
	else	/*没有这个组件*/
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
		return false;
	}
	
	if(p_mcc == NULL)	/*组件未使用*/
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
		return false;
	}
	
	err = MCC_Reset(p_mcc, 15000);
	if(err != OS_ERR_NONE)	/*执行成功*/
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x09);		/*报错，指令执行错误*/
		return false;
	}
	
	Drv_CANProtocol_SendDoneToBuffer(can_id, serial_number, cmd, frame_id);	/*直接回复执行成功*/	
	return true;
}

/* 组件运动控制
  ------------------------------ OK-
  返回值：true，成功；false，失败
*/
bool Func_Cmd_Com_Mcc_Move(void* p_buffer)
{
	DRV_CAN_RX_BUFFER_TYPE*          p_buf  = (DRV_CAN_RX_BUFFER_TYPE *)p_buffer;					/*获取缓冲区数据*/
	CMD_COMMON_HEAD_TYPE*            p_msg  = (CMD_COMMON_HEAD_TYPE*)(p_buf->buffer);				/*获取消息内容*/
	CMD_COMMON_DATA_MCC_MOVE_TYPE*   p_data = (CMD_COMMON_DATA_MCC_MOVE_TYPE*)(&(p_msg->data));		/*获取数据域*/
	
	uint8_t  size          = p_buf->size;			/*获取指令长度*/
	uint8_t  can_id        = p_buf->can_id;			/*获取CAN ID*/
	uint8_t  serial_number = p_msg->serial_number;	/*获取流水号*/
	uint16_t cmd           = p_msg->cmd;			/*获取指令*/
	uint32_t frame_id      = p_msg->frame_id;		/*获取报文ID*/
	
	uint8_t  err           = OS_ERR_NONE;
	DRV_MCC_TYPE* p_mcc    = NULL;					/*待操作组件结构体*/
	
	if(size != 4)			/*指令长度判断*/
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x05);		/*报错，指令长度错误*/
		return false;
	}
	
	if(p_data->mcc_number <= MCC_BUTT)
	{
		p_mcc = _gp_M[p_data->mcc_number - 1];
	}
	else	/*没有这个组件*/
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
		return false;
	}
	
	if(p_mcc == NULL)	/*组件未使用*/
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
		return false;
	}
		
	switch(p_data->motion_type)		/*控制运动组件*/
	{
		case 0:	/*按整步数走*/
			err = MCC_Move_SynchronizingStep(p_mcc, \
											(p_mcc->reset_dir == MCC_RESET_NEGATIVE) ? !p_data->dir : p_data->dir, \
											p_data->value, \
											5000, \
											MCC_MOVE_DISTANCE, \
											0, \
											MCC_LINE_AUTO);
			break;
			
		case 1:	/*按角度/距离/液量走*/
			err = MCC_Move_Distance(p_mcc, \
									(p_mcc->reset_dir == MCC_RESET_NEGATIVE) ? !p_data->dir : p_data->dir, \
									(double)p_data->value / 100.0, \
									5000, \
									MCC_MOVE_DISTANCE, \
									0, \
									MCC_LINE_AUTO);
			break;
			
		case 2:	/*按位置编号走 开环*/
			err = MCC_Move_PresetLocation(p_mcc, \
										p_mcc->preset_location + p_data->value, \
										5000, \
										MCC_MOVE_DISTANCE, \
										0, \
										MCC_LINE_AUTO);
			break;
			
		case 3:	/*按位置编号走 闭环*/
			err = MCC_Move_PresetLocation(p_mcc, \
										p_mcc->preset_location + p_data->value, \
										5000, \
										((p_mcc->preset_location + p_data->value)->en_close_cycle == ENABLE_CLOSE_CYCLE) ? MCC_MOVE_D_CODE: MCC_MOVE_DISTANCE, \
										MCC_GetDCodeInterval(p_mcc, MCC_CalcSubdivisionStep(p_mcc, (p_mcc->preset_location + p_data->value)->data)), \
										MCC_LINE_AUTO);
			break;
		
		case 4:	/*按整步数坐标走*/
			err = MCC_Move_SynchronizingLocation(p_mcc, \
												(p_data->dir == 0) ? p_data->value : -p_data->value, \
												5000, \
												MCC_MOVE_DISTANCE, \
												0, \
												MCC_LINE_AUTO);
			break;
			
		case 5:	/*按角度/距离/液量坐标走*/
			err = MCC_Move_Coordinates(p_mcc, \
										 (p_data->dir == 0) ? (double)(p_data->value) / 100.0 : -(double)(p_data->value) / 100.0, \
										 5000, \
										 MCC_MOVE_DISTANCE, \
										 0, \
										 MCC_LINE_AUTO);
			break;
			
		case 6:	/*按码齿数走闭环*/
			err = MCC_Move_DCodeCount(p_mcc, \
								(p_mcc->reset_dir == MCC_RESET_NEGATIVE) ? !p_data->dir : p_data->dir, \
								p_mcc->preset_location + 1, \
								5000, \
								MCC_MOVE_D_CODE, \
								p_data->value, \
								MCC_LINE_AUTO);
			break;			
			
		case 7:	/*按码齿数走开环*/
			err = MCC_Move_DCodeCount(p_mcc, \
								(p_mcc->reset_dir == MCC_RESET_NEGATIVE) ? !p_data->dir : p_data->dir, \
								p_mcc->preset_location + 1, \
								5000, \
								MCC_MOVE_DISTANCE, \
								p_data->value, \
								MCC_LINE_AUTO);
			break;			
			
		default:
			break;
	}
	
	if(err != OS_ERR_NONE)
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x09);		/*报错，指令执行错误*/
		return false;
	}
	
	Drv_CANProtocol_SendDoneToBuffer(can_id, serial_number, cmd, frame_id);	/*直接回复执行成功*/
	return true;
}

/* 电磁阀/电磁铁控制
  ------------------------------ OK
  返回值：true，成功；false，失败
*/
bool Func_Cmd_Com_Valve_Control(void* p_buffer)
{
	DRV_CAN_RX_BUFFER_TYPE*             p_buf  = (DRV_CAN_RX_BUFFER_TYPE *)p_buffer;					/*获取缓冲区数据*/
	CMD_COMMON_HEAD_TYPE*               p_msg  = (CMD_COMMON_HEAD_TYPE*)(p_buf->buffer);				/*获取消息内容*/
	CMD_COMMON_DATA_VALVE_CONTROL_TYPE* p_data = (CMD_COMMON_DATA_VALVE_CONTROL_TYPE*)(&(p_msg->data));	/*获取数据域*/
	
	uint8_t  size          = p_buf->size;			/*获取指令长度*/
	uint8_t  can_id        = p_buf->can_id;			/*获取CAN ID*/
	uint8_t  serial_number = p_msg->serial_number;	/*获取流水号*/
	uint16_t cmd           = p_msg->cmd;			/*获取指令*/
	uint32_t frame_id      = p_msg->frame_id;		/*获取报文ID*/
	
	DRV_VALVE_TYPE* p_valve   = NULL;				/*待操作电磁阀结构体*/
	
	if(size != 3)		/*指令长度判断*/
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x05);	/*报错，指令长度错误*/
		return false;
	}
	
	if(p_data->valve_number <= VALVE_BUTT)
	{
		p_valve = _gp_V[p_data->valve_number - 1];
	}
	else	/*没有这个阀*/
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
		return false;
	}
	
	if(p_valve == NULL)	/*阀未使用*/
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
		return false;
	}
	
	switch(p_data->status)		/*控制阀*/
	{
		case 0:		/*关*/
			Valve_Close(p_valve);
			break;
		case 1:		/*开*/
			Valve_Open(p_valve);
			break;
		default:	/*操作未定义*/
			Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
			return false;
	}
	
	Drv_CANProtocol_SendDoneToBuffer(can_id, serial_number, cmd, frame_id);	/*直接回复执行成功*/
	return true;
}

/* 读取信号
  ------------------------------ OK
  返回值：true，成功；false，失败
*/
bool Func_Cmd_Com_ReadPin(void* p_buffer)
{
	DRV_CAN_RX_BUFFER_TYPE*          p_buf  = (DRV_CAN_RX_BUFFER_TYPE *)p_buffer;					/*获取缓冲区数据*/
	CMD_COMMON_HEAD_TYPE*            p_msg  = (CMD_COMMON_HEAD_TYPE*)(p_buf->buffer);				/*获取消息内容*/
	CMD_COMMON_DATA_READ_SIGNAL_TYPE* p_data = (CMD_COMMON_DATA_READ_SIGNAL_TYPE*)(&(p_msg->data));	/*获取数据域*/
	
	uint8_t  size          = p_buf->size;			/*获取指令长度*/
	uint8_t  can_id        = p_buf->can_id;			/*获取CAN ID*/
	uint8_t  serial_number = p_msg->serial_number;	/*获取流水号*/
	uint16_t cmd           = p_msg->cmd;			/*获取指令*/
	uint32_t frame_id      = p_msg->frame_id;		/*获取报文ID*/
	
	uint32_t status        = 0;						/*信号状态*/
		
	if(size != 3)		/*指令长度判断*/
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x05);	/*报错，指令长度错误*/
		return false;
	}
	
	switch(p_data->signal_number)	/*获取待读取信号编号*/
	{
		case 1:		/*信号1(M1的复位信号)*/
			if(_gp_M[0] == NULL)
			{
				Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
				return false;
			}
			status = BSP_ReadPin(_gp_M[0]->reset_port_number, _gp_M[0]->reset_pin_number);
			break;
		case 2:		/*信号2（M2的复位信号）*/
			if(_gp_M[1] == NULL)
			{
				Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
				return false;
			}
			status = BSP_ReadPin(_gp_M[1]->reset_port_number, _gp_M[1]->reset_pin_number);
			break;
		case 3:		/*信号3（M3的复位信号）*/
			if(_gp_M[2] == NULL)
			{
				Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
				return false;
			}
			status = BSP_ReadPin(_gp_M[2]->reset_port_number, _gp_M[2]->reset_pin_number);
			break;
		case 4:		/*信号4（M4的复位信号）*/
			if(_gp_M[3] == NULL)
			{
				Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
				return false;
			}
			status = BSP_ReadPin(_gp_M[3]->reset_port_number, _gp_M[3]->reset_pin_number);
			break;
		default:	/*没有这个信号*/
			Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
			return false;
	}
	
	Drv_CANProtocol_SendDataToBuffer(can_id, serial_number, cmd, frame_id, status, 51);	/*直接回复执行成功*/
	return true;
}

/* 泵/直流电机控制
  ------------------------------ OK
  返回值：true，成功；false，失败
*/
bool Func_Cmd_Com_Pump_Control(void* p_buffer)
{
	DRV_CAN_RX_BUFFER_TYPE*          p_buf  = (DRV_CAN_RX_BUFFER_TYPE *)p_buffer;					/*获取缓冲区数据*/
	CMD_COMMON_HEAD_TYPE*            p_msg  = (CMD_COMMON_HEAD_TYPE*)(p_buf->buffer);				/*获取消息内容*/
	CMD_COMMON_DATA_PUMP_CONTROL_TYPE* p_data = (CMD_COMMON_DATA_PUMP_CONTROL_TYPE*)(&(p_msg->data));	/*获取数据域*/
	
	uint8_t  size          = p_buf->size;			/*获取指令长度*/
	uint8_t  can_id        = p_buf->can_id;			/*获取CAN ID*/
	uint8_t  serial_number = p_msg->serial_number;	/*获取流水号*/
	uint16_t cmd           = p_msg->cmd;			/*获取指令*/
	uint32_t frame_id      = p_msg->frame_id;		/*获取报文ID*/
	
	DRV_PUMP_TYPE* p_pump   = NULL;				/*待操作泵结构体*/
	
	if(size != 3)		/*指令长度判断*/
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x05);	/*报错，指令长度错误*/
		return false;
	}
	
	if(p_data->pump_number <= PUMP_BUTT)	/*获取待操作泵编号*/
	{
		p_pump = _gp_P[p_data->pump_number - 1];
	}
	else	/*没有这个泵*/
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
		return false;
	}
	
	if(p_pump == NULL)	/*泵未使用*/
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
		return false;
	}
	
	switch(p_data->status)		/*控制泵*/
	{
		case 0:		/*停止*/
			PumpStop(p_pump);
			break;
		case 1:		/*启动*/
			PumpStart(p_pump);
			break;
		default:	/*操作未定义*/
			Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
			return false;
	}
	
	Drv_CANProtocol_SendDoneToBuffer(can_id, serial_number, cmd, frame_id);	/*直接回复执行成功*/
	return true;
}

/* 通用参数读写
  ------------------------------ OK-
  返回值：true，成功；false，失败
*/
bool Func_Cmd_Com_RW_Parameter(void* p_buffer)
{
	DRV_CAN_RX_BUFFER_TYPE*          p_buf  = (DRV_CAN_RX_BUFFER_TYPE *)p_buffer;					/*获取缓冲区数据*/
	CMD_COMMON_HEAD_TYPE*            p_msg  = (CMD_COMMON_HEAD_TYPE*)(p_buf->buffer);				/*获取消息内容*/
	CMD_COMMON_DATA_RW_DATA_TYPE* p_data = (CMD_COMMON_DATA_RW_DATA_TYPE*)(&(p_msg->data));			/*获取数据域*/
	
	uint8_t  size          = p_buf->size;			/*获取指令长度*/
	uint8_t  can_id        = p_buf->can_id;			/*获取CAN ID*/
	uint8_t  serial_number = p_msg->serial_number;	/*获取流水号*/
	uint16_t cmd           = p_msg->cmd;			/*获取指令*/
	uint32_t frame_id      = p_msg->frame_id;		/*获取报文ID*/
	
	DRV_MCC_TYPE* p_mcc    = NULL;					/*获取待操作组件结构体*/
	int32_t  parameter     = 0;					/*获取参数值*/
	
	if(size != 4)
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x05);		/*报错，指令长度错误*/
		return false;
	}
	
	if(p_data->mcc_number <= MCC_BUTT)
	{
		p_mcc = _gp_M[p_data->mcc_number - 1];
	}
	else	/*没有这个组件*/
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
		return false;
	}
	
	if(p_mcc == NULL)	/*组件未使用*/
	{
		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
		return false;
	}
	
	switch(p_data->rw)	/*读/写*/
	{
		case 0:	/*读取*/
			switch(p_data->data_type)
			{
				case 0:	/*复位补偿*/
					parameter = (p_mcc->compensation + p_data->data_number1 - 1)->reset_compensation;
					break;
				case 1:	/*位置参数*/
					switch(p_data->data_number2)
					{
						case 1:	/*有否码齿*/
							parameter = (p_mcc->preset_location + p_data->data_number1 - 1)->en_close_cycle;
							break;
						case 2:	/*位置坐标（mm/°/uL）*/
							parameter = (int32_t)((p_mcc->preset_location + p_data->data_number1 - 1)->data * 100);
							break;
						case 3:	/*正向预减速位置坐标（mm/°/uL）*/
							parameter = (int32_t)((p_mcc->preset_location + p_data->data_number1 - 1)->forward_pre_deceleration_data * 100);
							break;
						case 4:	/*反向预减速位置坐标（mm/°/uL）*/
							parameter = (int32_t)((p_mcc->preset_location + p_data->data_number1 - 1)->reverse_pre_deceleration_data * 100);
							break;
						case 5:	/*正向补偿（整步）*/
							parameter = (p_mcc->preset_location + p_data->data_number1 - 1)->forward_compensation;
							break;
						case 6:	/*反向补偿（整步）*/
							parameter = (p_mcc->preset_location + p_data->data_number1 - 1)->reverse_compensation;
							break;
						default:							
							Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
							return false;
					}
					break;
				case 2:	/*电机参数*/
					switch(p_data->data_number2)
					{
						case 1:	/*低速初始频率*/
							parameter = (p_mcc->motor->line_low + p_data->data_number1 - 1)->start_fre;
							break;
						case 2:	/*低速最大频率*/
							parameter = (p_mcc->motor->line_low + p_data->data_number1 - 1)->end_fre;
							break;
						case 3:	/*低速每台阶步数*/
							parameter = (p_mcc->motor->line_low + p_data->data_number1 - 1)->ech_lader_step;
							break;
						case 4:	/*低速加速台阶数*/
							parameter = (p_mcc->motor->line_low + p_data->data_number1 - 1)->up_max_lader;
							break;
						case 5:	/*低速S曲线参数*/
							parameter = (p_mcc->motor->line_low + p_data->data_number1 - 1)->s_par;
							break;
						case 6:	/*高速初始频率*/
							parameter = (p_mcc->motor->line_high + p_data->data_number1 - 1)->start_fre;
							break;
						case 7:	/*高速最大频率*/
							parameter = (p_mcc->motor->line_high + p_data->data_number1 - 1)->end_fre;
							break;
						case 8:	/*高速每台阶步数*/
							parameter = (p_mcc->motor->line_high + p_data->data_number1 - 1)->ech_lader_step;
							break;
						case 9:	/*高速加速台阶数*/
							parameter = (p_mcc->motor->line_high + p_data->data_number1 - 1)->up_max_lader;
							break;
						case 10:/*高速S曲线参数*/
							parameter = (p_mcc->motor->line_high+ p_data->data_number1 - 1)->s_par;
							break;
						default:							
							Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
							return false;
					}
					break;
					
				default:							
					Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
					return false;
			}
			break;
		
		case 1:	/*写入*/
			switch(p_data->data_type)
			{
				case 0:	/*复位补偿*/
					(p_mcc->compensation + p_data->data_number1 - 1)->reset_compensation = parameter;
					break;
				case 1:	/*位置参数*/
					switch(p_data->data_number2)
					{
						case 1:	/*有否码齿*/
							(p_mcc->preset_location + p_data->data_number1 - 1)->en_close_cycle = parameter;
							break;
						case 2:	/*位置坐标（100 mm/°/uL）*/
							(p_mcc->preset_location + p_data->data_number1 - 1)->data = (double)parameter / 100.0;
							break;
						case 3:	/*正向预减速位置坐标（100 mm/°/uL）*/
							(p_mcc->preset_location + p_data->data_number1 - 1)->forward_pre_deceleration_data = (double)parameter / 100.0;
							break;
						case 4:	/*反向预减速位置坐标（100 mm/°/uL）*/
							(p_mcc->preset_location + p_data->data_number1 - 1)->reverse_pre_deceleration_data = (double)parameter / 100.0;
							break;
						case 5:	/*正向补偿（整步）*/
							(p_mcc->preset_location + p_data->data_number1 - 1)->forward_compensation = parameter;
							break;
						case 6:	/*反向补偿（整步）*/
							(p_mcc->preset_location + p_data->data_number1 - 1)->reverse_compensation = parameter;
							break;
						default:							
							Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
							return false;
					}
					break;
				case 2:	/*电机参数*/
					switch(p_data->data_number2)
					{
						case 1:	/*低速初始频率*/
							(p_mcc->motor->line_low + p_data->data_number1 - 1)->start_fre = parameter;
							break;
						case 2:	/*低速最大频率*/
							(p_mcc->motor->line_low + p_data->data_number1 - 1)->end_fre = parameter;
							break;
						case 3:	/*低速每台阶步数*/
							(p_mcc->motor->line_low + p_data->data_number1 - 1)->ech_lader_step = parameter;
							break;
						case 4:	/*低速加速台阶数*/
							(p_mcc->motor->line_low + p_data->data_number1 - 1)->up_max_lader = parameter;
							break;
						case 5:	/*低速S曲线参数*/
							(p_mcc->motor->line_low + p_data->data_number1 - 1)->s_par = parameter;
							break;
						case 6:	/*高速初始频率*/
							(p_mcc->motor->line_high + p_data->data_number1 - 1)->start_fre = parameter;
							break;
						case 7:	/*高速最大频率*/
							(p_mcc->motor->line_high + p_data->data_number1 - 1)->end_fre = parameter;
							break;
						case 8:	/*高速每台阶步数*/
							(p_mcc->motor->line_high + p_data->data_number1 - 1)->ech_lader_step = parameter;
							break;
						case 9:	/*高速加速台阶数*/
							(p_mcc->motor->line_high + p_data->data_number1 - 1)->up_max_lader = parameter;
							break;
						case 10:/*高速S曲线参数*/
							(p_mcc->motor->line_high+ p_data->data_number1 - 1)->s_par = parameter;
							break;
						default:							
							Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
							return false;
					}
					break;
					
				default:							
					Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
					return false;
			}
			break;
			
		default:							
			Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xFF, 0xFF, 0x08);		/*报错，指令参数错误*/
			return false;
	}
	
	Drv_CANProtocol_SendDataToBuffer(can_id, serial_number, cmd, frame_id, parameter, 53);	/*直接回复执行成功*/
	return true;
}
bool Func_Cmd_Com_RW_Page_Parameter(void* p_buffer)
{
	DRV_CAN_RX_BUFFER_TYPE*          p_buf  = (DRV_CAN_RX_BUFFER_TYPE *)p_buffer;					/*获取缓冲区数据*/
	CMD_COMMON_HEAD_TYPE*            p_msg  = (CMD_COMMON_HEAD_TYPE*)(p_buf->buffer);				/*获取消息内容*/
	CMD_COMMON_DATA_RW_PAGE_DATA_TYPE* p_data = (CMD_COMMON_DATA_RW_PAGE_DATA_TYPE*)(&(p_msg->data));			/*获取数据域*/
	
	uint8_t  size          = p_buf->size;			/*获取指令长度*/
	uint8_t  can_id        = p_buf->can_id;			/*获取CAN ID*/
	uint8_t  serial_number = p_msg->serial_number;	/*获取流水号*/
	uint16_t cmd           = p_msg->cmd;			/*获取指令*/
	uint32_t frame_id      = p_msg->frame_id;		/*获取报文ID*/
	
	uint8_t i = 0;
	bool flag = 0;
	uint8_t datas_eep[32] = {0};
	uint8_t temp_eep[32] = {0};
	uint8_t datas_out[100] = {0};
	
//	if(size != 7)
//	{
//		Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 10048);		/*报错，指令长度错误*/
//		return false;
//	}

	
	switch(p_data->rw)	/*读/写*/
	{
		case 0:	/*读取*/
			flag = At24c32ReadPage(_gp_EEP, p_data->number, datas_eep);
			if(flag == false)	/*失败*/
			{
				Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xff,0xff,0x08);		/*报错，读取失败*/
			}
			else
			{
				if((p_data->number < 10) && (p_data->number > 0))	/*读取了电机参数*/
				{
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->dwnmachinetype			= can_id;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->number					= p_data->number;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->data_count				= 21;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->flag 					= ((MCC_PARAMETER1_TYPE*)datas_eep)->flag;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->count 					= 19;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->subdivision 			= ((MCC_PARAMETER1_TYPE*)datas_eep)->subdivision;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->step_angle_multiply10 	= ((MCC_PARAMETER1_TYPE*)datas_eep)->step_angle_multiply10;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->ratio_multiply100 		= ((MCC_PARAMETER1_TYPE*)datas_eep)->ratio_multiply100;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->max_step 				= ((MCC_PARAMETER1_TYPE*)datas_eep)->max_step;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->reset_dir 				= ((MCC_PARAMETER1_TYPE*)datas_eep)->reset_dir;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->reset_status 			= ((MCC_PARAMETER1_TYPE*)datas_eep)->reset_status;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->d_code_status 			= ((MCC_PARAMETER1_TYPE*)datas_eep)->d_code_status;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->strike_status 			= ((MCC_PARAMETER1_TYPE*)datas_eep)->strike_status;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->reset_compensation 	= ((MCC_PARAMETER1_TYPE*)datas_eep)->reset_compensation;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->start_fre_h 			= ((MCC_PARAMETER1_TYPE*)datas_eep)->start_fre_h;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->end_fre_h 				= ((MCC_PARAMETER1_TYPE*)datas_eep)->end_fre_h;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->ech_lader_step_h 		= ((MCC_PARAMETER1_TYPE*)datas_eep)->ech_lader_step_h;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->up_max_lader_h 		= ((MCC_PARAMETER1_TYPE*)datas_eep)->up_max_lader_h;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->s_par_h_divide100 		= ((MCC_PARAMETER1_TYPE*)datas_eep)->s_par_h_divide100;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->start_fre_l 			= ((MCC_PARAMETER1_TYPE*)datas_eep)->start_fre_l;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->end_fre_l 				= ((MCC_PARAMETER1_TYPE*)datas_eep)->end_fre_l;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->ech_lader_step_l 		= ((MCC_PARAMETER1_TYPE*)datas_eep)->ech_lader_step_l;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->up_max_lader_l 		= ((MCC_PARAMETER1_TYPE*)datas_eep)->up_max_lader_l;
					((MCC_PARAMETER1_INTERFACE_OUT_TYPE*)datas_out)->s_par_l_divide100 		= ((MCC_PARAMETER1_TYPE*)datas_eep)->s_par_l_divide100;
					Drv_CANProtocol_SendDatasToBuffer(can_id, serial_number, cmd, frame_id, datas_out, sizeof(MCC_PARAMETER1_INTERFACE_OUT_TYPE), 9);	/*回复执行成功及读取内容*/
				}
				else if((p_data->number < 100) && (p_data->number > 9))	/*读取了位置参数*/
				{
					At24c32ReadPage(_gp_EEP, p_data->number / 10 * 10 + 1, temp_eep);	/*读取首位参数*/
					((MCC_PARAMETER2_INTERFACE_OUT_TYPE*)datas_out)->dwnmachinetype								= can_id;
					((MCC_PARAMETER2_INTERFACE_OUT_TYPE*)datas_out)->number										= p_data->number;
					((MCC_PARAMETER2_INTERFACE_OUT_TYPE*)datas_out)->data_count									= 8;
					((MCC_PARAMETER2_INTERFACE_OUT_TYPE*)datas_out)->flag 										= ((MCC_PARAMETER2_TYPE*)datas_eep)->flag;
					((MCC_PARAMETER2_INTERFACE_OUT_TYPE*)datas_out)->count 										= 6;
					((MCC_PARAMETER2_INTERFACE_OUT_TYPE*)datas_out)->en_close_cycle								= ((MCC_PARAMETER2_TYPE*)datas_eep)->en_close_cycle;
					((MCC_PARAMETER2_INTERFACE_OUT_TYPE*)datas_out)->data_multiply100 							= (uint32_t)(((MCC_PARAMETER2_TYPE*)datas_eep)->data_multiply100 - ((MCC_PARAMETER2_TYPE*)temp_eep)->forward_pre_deceleration_data_multiply100);	/*上传其与最小坐标的差值*/
					((MCC_PARAMETER2_INTERFACE_OUT_TYPE*)datas_out)->forward_pre_deceleration_data_multiply100	= (uint32_t)(((MCC_PARAMETER2_TYPE*)datas_eep)->forward_pre_deceleration_data_multiply100 - ((MCC_PARAMETER2_TYPE*)temp_eep)->forward_pre_deceleration_data_multiply100);
					((MCC_PARAMETER2_INTERFACE_OUT_TYPE*)datas_out)->reverse_pre_deceleration_data_multiply100	= (uint32_t)(((MCC_PARAMETER2_TYPE*)datas_eep)->reverse_pre_deceleration_data_multiply100 - ((MCC_PARAMETER2_TYPE*)temp_eep)->forward_pre_deceleration_data_multiply100);
					((MCC_PARAMETER2_INTERFACE_OUT_TYPE*)datas_out)->forward_compensation_multiply100			= (uint32_t)(((MCC_PARAMETER2_TYPE*)datas_eep)->forward_compensation_multiply100);
					((MCC_PARAMETER2_INTERFACE_OUT_TYPE*)datas_out)->reverse_compensation_multiply100			= (uint32_t)(((MCC_PARAMETER2_TYPE*)datas_eep)->reverse_compensation_multiply100);

					Drv_CANProtocol_SendDatasToBuffer(can_id, serial_number, cmd, frame_id, datas_out, sizeof(MCC_PARAMETER2_INTERFACE_OUT_TYPE), 9);	/*回复执行成功及读取内容*/
				}
				else if(p_data->number == 100) 	/*读取了针参数*/
				{
					#if (defined TD300_SAMPLE_ARM)
					((SAMPLE_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->dwnmachinetype					= can_id;
					((SAMPLE_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->number							= p_data->number;
					((SAMPLE_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->data_count						= 9;
					((SAMPLE_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->flag 								= ((SAMPLE_NEEDLE_PARAMETER_TYPE*)datas_eep)->flag;
					((SAMPLE_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->count 							= 7;
					((SAMPLE_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->up_inhaled_air_multiply100 		= ((SAMPLE_NEEDLE_PARAMETER_TYPE*)datas_eep)->up_inhaled_air_multiply100;
					((SAMPLE_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->rotate_inhaled_air_multiply100 	= ((SAMPLE_NEEDLE_PARAMETER_TYPE*)datas_eep)->rotate_inhaled_air_multiply100;
					((SAMPLE_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->reset_wash_time 					= ((SAMPLE_NEEDLE_PARAMETER_TYPE*)datas_eep)->reset_wash_time;
					((SAMPLE_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->default_wash_time 				= ((SAMPLE_NEEDLE_PARAMETER_TYPE*)datas_eep)->default_wash_time;
					((SAMPLE_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->multi_suction_multiply100 		= ((SAMPLE_NEEDLE_PARAMETER_TYPE*)datas_eep)->multi_suction_multiply100;
					((SAMPLE_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->multi_displacement_multiply100 	= ((SAMPLE_NEEDLE_PARAMETER_TYPE*)datas_eep)->multi_displacement_multiply100;
					((SAMPLE_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->push_back_quantity_multiply100 	= ((SAMPLE_NEEDLE_PARAMETER_TYPE*)datas_eep)->push_back_quantity_multiply100;
					Drv_CANProtocol_SendDatasToBuffer(can_id, serial_number, cmd, frame_id, datas_out, sizeof(SAMPLE_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE), 9);	/*回复执行成功及读取内容*/
					#else
					((REAGENT_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->dwnmachinetype					= can_id;
					((REAGENT_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->number							= p_data->number;
					((REAGENT_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->data_count						= 9;
					((REAGENT_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->flag 							= ((REAGENT_NEEDLE_PARAMETER_TYPE*)datas_eep)->flag;
					((REAGENT_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->count 							= 7;
					((REAGENT_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->up_inhaled_air_after_multiply100	= ((REAGENT_NEEDLE_PARAMETER_TYPE*)datas_eep)->up_inhaled_air_after_multiply100;
					((REAGENT_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->up_inhaled_air_befor_multiply100	= ((REAGENT_NEEDLE_PARAMETER_TYPE*)datas_eep)->up_inhaled_air_befor_multiply100;
					((REAGENT_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->rotate_inhaled_air_multiply100	= ((REAGENT_NEEDLE_PARAMETER_TYPE*)datas_eep)->rotate_inhaled_air_multiply100;
					((REAGENT_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->reset_wash_time	 				= ((REAGENT_NEEDLE_PARAMETER_TYPE*)datas_eep)->reset_wash_time;
					((REAGENT_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->default_wash_time 				= ((REAGENT_NEEDLE_PARAMETER_TYPE*)datas_eep)->default_wash_time;
					((REAGENT_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->multi_suction_multiply100 		= ((REAGENT_NEEDLE_PARAMETER_TYPE*)datas_eep)->multi_suction_multiply100;
					((REAGENT_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->multi_displacement_multiply100 	= ((REAGENT_NEEDLE_PARAMETER_TYPE*)datas_eep)->multi_displacement_multiply100;
					Drv_CANProtocol_SendDatasToBuffer(can_id, serial_number, cmd, frame_id, datas_out, sizeof(REAGENT_NEEDLE_PARAMETER_INTERFACE_OUT_TYPE), 9);	/*回复执行成功及读取内容*/
					#endif
				}
				else if(p_data->number == 104) 	/*读取了杆参数*/
				{
					((STIRRING_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->dwnmachinetype		= can_id;
					((STIRRING_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->number				= p_data->number;
					((STIRRING_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->data_count			= 4;
					((STIRRING_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->flag 				= ((STIRRING_PARAMETER_TYPE*)datas_eep)->flag;
					((STIRRING_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->count 				= 2;
					((STIRRING_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->reset_wash_time 	= ((STIRRING_PARAMETER_TYPE*)datas_eep)->reset_wash_time;
					((STIRRING_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->default_wash_time 	= ((STIRRING_PARAMETER_TYPE*)datas_eep)->default_wash_time;

					Drv_CANProtocol_SendDatasToBuffer(can_id, serial_number, cmd, frame_id, datas_out, sizeof(STIRRING_PARAMETER_INTERFACE_OUT_TYPE), 9);	/*回复执行成功及读取内容*/
				}
				else if((p_data->number == 108) || (p_data->number == 109))	/*读取了测光参数*/
				{
					((DETECTION_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->dwnmachinetype	= can_id;
					((DETECTION_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->number			= p_data->number;
					((DETECTION_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->data_count		= 16;
					((DETECTION_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->flag 				= ((DETECTION_PARAMETER_TYPE*)datas_eep)->flag;
					((DETECTION_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->count 			= 14;
					for(i = 0; i < 14; i++)
					{
						((DETECTION_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->gains[i] = ((DETECTION_PARAMETER_TYPE*)datas_eep)->gains[i];
					}
					
					Drv_CANProtocol_SendDatasToBuffer(can_id, serial_number, cmd, frame_id, datas_out, sizeof(DETECTION_PARAMETER_INTERFACE_OUT_TYPE), 9);	/*回复执行成功及读取内容*/
				}
				else if((p_data->number == 110) || (p_data->number == 111))	/*读取了反应盘温控参数*/
				{
					((REACTION_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->dwnmachinetype				= can_id;
					((REACTION_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->number						= p_data->number;
					((REACTION_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->data_count					= 10;
					((REACTION_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->flag 						= ((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->flag;
					((REACTION_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->count 						= 8;						
					((REACTION_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->temperature_multiply100 	= ((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->temperature_multiply100;	
					((REACTION_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->kp_multiply10000 			= ((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->kp_multiply10000;			
					((REACTION_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->ki_multiply10000 			= ((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->ki_multiply10000;			
					((REACTION_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->kd_multiply10000			= ((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->kd_multiply10000;
					for(i = 0; i < 4;i++)
					{
						((REACTION_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->temperature_switch[i] = ((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->temperature_switch[i];		
					}
					
					Drv_CANProtocol_SendDatasToBuffer(can_id, serial_number, cmd, frame_id, datas_out, sizeof(REACTION_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE), 9);	/*回复执行成功及读取内容*/
				}
				else if((p_data->number >= 112) && (p_data->number <= 117))	/*读取了反应盘温控参数*/
				{
					((WATER_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->dwnmachinetype				= can_id;
					((WATER_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->number						= p_data->number;
					((WATER_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->data_count					= 6;
					((WATER_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->flag 							= ((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->flag;						
				    ((WATER_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->count 						= 4;						
				    ((WATER_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->temperature_multiply100 		= ((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->temperature_multiply100;	
				    ((WATER_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->kp_multiply10000 				= ((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->kp_multiply10000;		
				    ((WATER_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->ki_multiply10000 				= ((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->ki_multiply10000;	
					((WATER_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE*)datas_out)->kd_multiply10000 				= ((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->kd_multiply10000;
					
					Drv_CANProtocol_SendDatasToBuffer(can_id, serial_number, cmd, frame_id, datas_out, sizeof(WATER_TEMPERATURE_PARAMETER_INTERFACE_OUT_TYPE), 9);	/*回复执行成功及读取内容*/
				}
				else
				{
					Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xff,0xff,0x08);			/*报错，指令参数错误*/
				}
			}
			break;
		
		case 1:	/*写入*/
			/*获取参数*/
			if((p_data->number < 10) && (p_data->number > 0))	/*要写电机参数*/
			{
				((MCC_PARAMETER1_TYPE*)datas_eep)->flag 					= ((MCC_PARAMETER1_INTERFACE_IN_TYPE*)(p_data->datas))->flag;
				((MCC_PARAMETER1_TYPE*)datas_eep)->subdivision 				= ((MCC_PARAMETER1_INTERFACE_IN_TYPE*)(p_data->datas))->subdivision;
				((MCC_PARAMETER1_TYPE*)datas_eep)->step_angle_multiply10 	= ((MCC_PARAMETER1_INTERFACE_IN_TYPE*)(p_data->datas))->step_angle_multiply10;
				((MCC_PARAMETER1_TYPE*)datas_eep)->ratio_multiply100 		= ((MCC_PARAMETER1_INTERFACE_IN_TYPE*)(p_data->datas))->ratio_multiply100;
				((MCC_PARAMETER1_TYPE*)datas_eep)->max_step 				= ((MCC_PARAMETER1_INTERFACE_IN_TYPE*)(p_data->datas))->max_step;
				((MCC_PARAMETER1_TYPE*)datas_eep)->reset_dir 				= ((MCC_PARAMETER1_INTERFACE_IN_TYPE*)(p_data->datas))->reset_dir;
				((MCC_PARAMETER1_TYPE*)datas_eep)->reset_status 			= ((MCC_PARAMETER1_INTERFACE_IN_TYPE*)(p_data->datas))->reset_status;
				((MCC_PARAMETER1_TYPE*)datas_eep)->d_code_status 			= ((MCC_PARAMETER1_INTERFACE_IN_TYPE*)(p_data->datas))->d_code_status;
				((MCC_PARAMETER1_TYPE*)datas_eep)->strike_status 			= ((MCC_PARAMETER1_INTERFACE_IN_TYPE*)(p_data->datas))->strike_status;
				((MCC_PARAMETER1_TYPE*)datas_eep)->reset_compensation 		= ((MCC_PARAMETER1_INTERFACE_IN_TYPE*)(p_data->datas))->reset_compensation;
				((MCC_PARAMETER1_TYPE*)datas_eep)->start_fre_h 				= ((MCC_PARAMETER1_INTERFACE_IN_TYPE*)(p_data->datas))->start_fre_h;
				((MCC_PARAMETER1_TYPE*)datas_eep)->end_fre_h 				= ((MCC_PARAMETER1_INTERFACE_IN_TYPE*)(p_data->datas))->end_fre_h;
				((MCC_PARAMETER1_TYPE*)datas_eep)->ech_lader_step_h 		= ((MCC_PARAMETER1_INTERFACE_IN_TYPE*)(p_data->datas))->ech_lader_step_h;
				((MCC_PARAMETER1_TYPE*)datas_eep)->up_max_lader_h 			= ((MCC_PARAMETER1_INTERFACE_IN_TYPE*)(p_data->datas))->up_max_lader_h;
				((MCC_PARAMETER1_TYPE*)datas_eep)->s_par_h_divide100 		= ((MCC_PARAMETER1_INTERFACE_IN_TYPE*)(p_data->datas))->s_par_h_divide100;
				((MCC_PARAMETER1_TYPE*)datas_eep)->start_fre_l 				= ((MCC_PARAMETER1_INTERFACE_IN_TYPE*)(p_data->datas))->start_fre_l;
				((MCC_PARAMETER1_TYPE*)datas_eep)->end_fre_l 				= ((MCC_PARAMETER1_INTERFACE_IN_TYPE*)(p_data->datas))->end_fre_l;
				((MCC_PARAMETER1_TYPE*)datas_eep)->ech_lader_step_l 		= ((MCC_PARAMETER1_INTERFACE_IN_TYPE*)(p_data->datas))->ech_lader_step_l;
				((MCC_PARAMETER1_TYPE*)datas_eep)->up_max_lader_l 			= ((MCC_PARAMETER1_INTERFACE_IN_TYPE*)(p_data->datas))->up_max_lader_l;
				((MCC_PARAMETER1_TYPE*)datas_eep)->s_par_l_divide100 		= ((MCC_PARAMETER1_INTERFACE_IN_TYPE*)(p_data->datas))->s_par_l_divide100;
			}
			else if((p_data->number < 100) && (p_data->number > 9))	/*要写位置参数*/
			{
				At24c32ReadPage(_gp_EEP, p_data->number / 10 * 10 + 1, temp_eep);	/*读取首位参数*/
				
				((MCC_PARAMETER2_TYPE*)datas_eep)->flag 										= ((MCC_PARAMETER2_INTERFACE_IN_TYPE*)(p_data->datas))->flag;
				((MCC_PARAMETER2_TYPE*)datas_eep)->en_close_cycle 								= ((MCC_PARAMETER2_INTERFACE_IN_TYPE*)(p_data->datas))->en_close_cycle;
				((MCC_PARAMETER2_TYPE*)datas_eep)->data_multiply100 							= ((MCC_PARAMETER2_TYPE*)temp_eep)->forward_pre_deceleration_data_multiply100 + ((MCC_PARAMETER2_INTERFACE_IN_TYPE*)(p_data->datas))->data_multiply100;	/*写入其与最小坐标的和，即绝对坐标*/
				((MCC_PARAMETER2_TYPE*)datas_eep)->forward_pre_deceleration_data_multiply100 	= ((MCC_PARAMETER2_TYPE*)temp_eep)->forward_pre_deceleration_data_multiply100 + ((MCC_PARAMETER2_INTERFACE_IN_TYPE*)(p_data->datas))->forward_pre_deceleration_data_multiply100;
				((MCC_PARAMETER2_TYPE*)datas_eep)->reverse_pre_deceleration_data_multiply100 	= ((MCC_PARAMETER2_TYPE*)temp_eep)->forward_pre_deceleration_data_multiply100 + ((MCC_PARAMETER2_INTERFACE_IN_TYPE*)(p_data->datas))->reverse_pre_deceleration_data_multiply100;
				((MCC_PARAMETER2_TYPE*)datas_eep)->forward_compensation_multiply100				= ((MCC_PARAMETER2_INTERFACE_IN_TYPE*)(p_data->datas))->forward_compensation_multiply100;
				((MCC_PARAMETER2_TYPE*)datas_eep)->reverse_compensation_multiply100				= ((MCC_PARAMETER2_INTERFACE_IN_TYPE*)(p_data->datas))->reverse_compensation_multiply100;
			}
			else if(p_data->number == 100) 	/*要写针参数*/
			{
				#if (defined TD300_SAMPLE_ARM)
				((SAMPLE_NEEDLE_PARAMETER_TYPE*)datas_eep)->flag								= ((SAMPLE_NEEDLE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->flag;
				((SAMPLE_NEEDLE_PARAMETER_TYPE*)datas_eep)->up_inhaled_air_multiply100			= ((SAMPLE_NEEDLE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->up_inhaled_air_multiply100;
				((SAMPLE_NEEDLE_PARAMETER_TYPE*)datas_eep)->rotate_inhaled_air_multiply100		= ((SAMPLE_NEEDLE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->rotate_inhaled_air_multiply100;
				((SAMPLE_NEEDLE_PARAMETER_TYPE*)datas_eep)->reset_wash_time						= ((SAMPLE_NEEDLE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->reset_wash_time;
				((SAMPLE_NEEDLE_PARAMETER_TYPE*)datas_eep)->default_wash_time					= ((SAMPLE_NEEDLE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->default_wash_time;
				((SAMPLE_NEEDLE_PARAMETER_TYPE*)datas_eep)->multi_suction_multiply100			= ((SAMPLE_NEEDLE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->multi_suction_multiply100;
				((SAMPLE_NEEDLE_PARAMETER_TYPE*)datas_eep)->multi_displacement_multiply100		= ((SAMPLE_NEEDLE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->multi_displacement_multiply100;
				((SAMPLE_NEEDLE_PARAMETER_TYPE*)datas_eep)->push_back_quantity_multiply100		= ((SAMPLE_NEEDLE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->push_back_quantity_multiply100;
				#else
				((REAGENT_NEEDLE_PARAMETER_TYPE*)datas_eep)->flag								= ((REAGENT_NEEDLE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->flag;
				((REAGENT_NEEDLE_PARAMETER_TYPE*)datas_eep)->up_inhaled_air_after_multiply100	= ((REAGENT_NEEDLE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->up_inhaled_air_after_multiply100;
				((REAGENT_NEEDLE_PARAMETER_TYPE*)datas_eep)->up_inhaled_air_befor_multiply100	= ((REAGENT_NEEDLE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->up_inhaled_air_befor_multiply100;
				((REAGENT_NEEDLE_PARAMETER_TYPE*)datas_eep)->rotate_inhaled_air_multiply100		= ((REAGENT_NEEDLE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->rotate_inhaled_air_multiply100;
				((REAGENT_NEEDLE_PARAMETER_TYPE*)datas_eep)->reset_wash_time					= ((REAGENT_NEEDLE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->reset_wash_time;
				((REAGENT_NEEDLE_PARAMETER_TYPE*)datas_eep)->default_wash_time					= ((REAGENT_NEEDLE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->default_wash_time;
				((REAGENT_NEEDLE_PARAMETER_TYPE*)datas_eep)->multi_suction_multiply100			= ((REAGENT_NEEDLE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->multi_suction_multiply100;
				((REAGENT_NEEDLE_PARAMETER_TYPE*)datas_eep)->multi_displacement_multiply100		= ((REAGENT_NEEDLE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->multi_displacement_multiply100;
				#endif
			}
			else if(p_data->number == 104)	/*要写杆参数*/
			{
				((STIRRING_PARAMETER_TYPE*)datas_eep)->flag				= ((STIRRING_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->flag;
				((STIRRING_PARAMETER_TYPE*)datas_eep)->reset_wash_time	= ((STIRRING_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->reset_wash_time;
				((STIRRING_PARAMETER_TYPE*)datas_eep)->default_wash_time	= ((STIRRING_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->default_wash_time;
			}
			else if((p_data->number == 108) || (p_data->number == 109))	/*要写测光参数*/
			{
				((DETECTION_PARAMETER_TYPE*)datas_eep)->flag = ((DETECTION_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->flag;
				for(i = 0; i < 14; i++)
				{
					((DETECTION_PARAMETER_TYPE*)datas_eep)->gains[i] = ((DETECTION_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->gains[i];
				}
			}
			else if((p_data->number == 110) || (p_data->number == 111))	/*要写反应盘温控参数*/
			{
				((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->flag = ((REACTION_TEMPERATURE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->flag;
				((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->temperature_multiply100 = ((REACTION_TEMPERATURE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->temperature_multiply100;	
				((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->kp_multiply10000 = ((REACTION_TEMPERATURE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->kp_multiply10000;			
				((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->ki_multiply10000 = ((REACTION_TEMPERATURE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->ki_multiply10000;			
				((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->kd_multiply10000 = ((REACTION_TEMPERATURE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->kd_multiply10000;
				for(i = 0; i < 4;i++)
				{
					((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->temperature_switch[i] = ((REACTION_TEMPERATURE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->temperature_switch[i];		
				}
			}
			else if((p_data->number >= 112) && (p_data->number <= 117))	/*要写清洗水温控参数*/
			{
				((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->flag = ((WATER_TEMPERATURE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->flag;
				((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->temperature_multiply100 = ((WATER_TEMPERATURE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->temperature_multiply100;	
				((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->kp_multiply10000 = ((WATER_TEMPERATURE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->kp_multiply10000;		
				((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->ki_multiply10000 = ((WATER_TEMPERATURE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->ki_multiply10000;	
				((REACTION_TEMPERATURE_PARAMETER_TYPE*)datas_eep)->kd_multiply10000 = ((WATER_TEMPERATURE_PARAMETER_INTERFACE_IN_TYPE*)(p_data->datas))->kd_multiply10000;
			}
			else
			{
				Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xff,0xff,0x08);			/*报错，指令参数错误*/
			}
			
			flag = At24c32WritePage(_gp_EEP, p_data->number, (uint8_t*)datas_eep);
			if(flag == false)	/*失败*/
			{
				Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xff,0xff,0x09);		/*报错，写入失败*/
			}
			else
			{
				Drv_CANProtocol_SendDoneToBuffer(can_id, serial_number, cmd, frame_id);				/*回复执行成功*/
				OSTimeDlyHMSM(0,0,0,500);
				BSP_Reboot();	/*软复位*/
			}
			break;
			
		default:							
			Drv_CANProtocol_SendErrorToBuffer(can_id, serial_number, cmd, frame_id, 0xff,0xff,0x05);			/*报错，指令参数错误*/
			return false;
	}
	
	return true;
}
/* 通用常驻任务函数
 ********************************************************/
