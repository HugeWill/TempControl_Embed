/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——CAN通信模块

  -------------------------------------------------------
   文 件 名   : Drv_CANProtocol.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年08月01日
   功能描述   : 定义一种CAN通信协议接口。
   使用方法   ：上层模块通过调用CAN_Init获得一个通信模块的
				指针。
   依赖于     : BSP V1.0.0.0
				uC/OS-II V2.92.07
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/
 
#include "Drv_CANProtocol.h"
 
static DRV_CAN_TYPE* _gp_CANunit = NULL;	/*CAN模块的注册表，有且仅有一个元素*/

/* 接口函数
 ********************************************************/

/* CAN通信模块初始化
  ---------------------------------ok
  最多可同时支持4个can id的接收，不使用的can_id写为0
  入口：CAN口编号，波特率，CAN ID，TX端口，TX引脚，RX端口，RX引脚
  返回值：初始化好的CAN通信模块指针
*/
DRV_CAN_TYPE* Drv_CANProtocol_Init(BSP_CAN_ENUM can_number, uint32_t baud_rate, \
								   uint8_t can_id1, uint8_t can_id2, uint8_t can_id3, uint8_t can_id4,\
								   BSP_PORT_ENUM tx_port_number, BSP_PIN_ENUM tx_pin_number, \
								   BSP_PORT_ENUM rx_port_number, BSP_PIN_ENUM rx_pin_number)
{
	uint8_t i;
	uint8_t os_err = OS_ERR_NONE;
	DRV_CAN_TYPE* p_unit = (DRV_CAN_TYPE*)calloc(1, sizeof(DRV_CAN_TYPE));	/*申请内存*/

	/*分配失败返回NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*can_number错误返回NULL*/
	if(can_number != _CAN1_)	/*仅支持_CAN1_*/
	{
		free(p_unit); 
		return NULL;
	}
	
	/*can id 1 不能为0*/
	if(can_id1 == 0)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*如果当前模块存在，取消分配新存储区*/
	if(_gp_CANunit != NULL)
	{
		free(p_unit);
		p_unit = _gp_CANunit;
	}
	else
	{
		_gp_CANunit = p_unit;
	}
	
	/*初始化*/
	
	/*-------------------------------常量*/
	p_unit->can_number     = can_number;				/*模块编号*/
	p_unit->can_id[0]      = can_id1;					/*CAN ID 1*/
	p_unit->can_id[1]      = can_id2;					/*CAN ID 2*/
	p_unit->can_id[2]      = can_id3;					/*CAN ID 3*/
	p_unit->can_id[3]      = can_id4;					/*CAN ID 4*/
	p_unit->baud_rate      = baud_rate;					/*波特率*/
	p_unit->tx_port_number = tx_port_number;			/*TX引脚端口号*/
	p_unit->tx_pin_number  = tx_pin_number;				/*TX引脚序号*/
	p_unit->rx_port_number = rx_port_number;			/*RX引脚端口号*/
	p_unit->rx_pin_number  = rx_pin_number;				/*RX引脚序号*/
	BSP_Init_CANinterrupt((BSP_CAN_ENUM)(p_unit->can_number), p_unit->baud_rate, \
						  p_unit->can_id[0], p_unit->can_id[1], p_unit->can_id[2], p_unit->can_id[3], \
						  p_unit->tx_port_number, p_unit->tx_pin_number, \
						  p_unit->rx_port_number, p_unit->rx_pin_number);	/*初始化硬件*/
	for(i = 0; i < 4; i++)
	{
		p_unit->rx_sem[i] = OSSemCreate(0);				/*4个CAN ID有独立的接收完成信号*/
	}
	p_unit->tx_sem    = OSSemCreate(0);					/*仅有一个发送器，对应仅一个待发送信号*/
	p_unit->send_to_buffer_mutex = OSMutexCreate(OS_PRIO_MUTEX_CEIL_DIS, &os_err);
	
	/*-------------------------------变量*/
	p_unit->status = CAN_IDLE;					/*该CAN模块的状态*/
	p_unit->tx_buffer_pop_index = 0;			/*发送缓存弹出下标*/
	p_unit->tx_buffer_push_index = 0;			/*发送缓存压入下标*/
	memset((void*)(p_unit->rx_buffer), 0, sizeof(DRV_CAN_RX_BUFFER_TYPE) * 4);	/*接收缓存清零*/
	memset((void*)(p_unit->tx_buffer), 0, sizeof(DRV_CAN_TX_BUFFER_TYPE));		/*发送缓存清零*/
	
	return p_unit;
}

/* CAN发送一个数据包到发送缓存
  --------------------------------- OK
  流水号 状态字 数据 数据长度 数据结束标志
  写入缓冲区，等待发送
  入口：CAN ID，数据指针，数据长度，结束标志
  返回值：true成功，false失败
*/
static bool _Drv_CANProtocol_SendToBuffer(uint8_t can_id, uint8_t* data, uint32_t len, DRV_CAN_END_MARK end_mark)
{
	uint32_t j, k;
	uint8_t* p_serial_number = &data[0];	/*流水号*/
	uint16_t* p_status       = (uint16_t*)(&data[1]);	/*状态字*/
	uint8_t* p_data          = &data[3];	/*数据*/
	uint32_t count           = 0;
	uint8_t last_frame_flag  = 0;			/*最后一帧标志位*/
	volatile uint32_t tx_buffer_push_index;			/*用于记录下标*/
//	static uint8_t running_flag = 0;		/*当前函数已在运行标志位*/
		
	if(len > (CAN_TX_CMD_MAX_SIZE * 8 - CAN_TX_CMD_MAX_SIZE + 1))	/*每个帧都包含流水号，需要扣除*/
		return false;	/*数据长度不满足要求*/
	
	/*禁止中断，先记录下标，再进行下标循环自增*/
	BSP_CAN1_IT_DISENABLE();
	tx_buffer_push_index = _gp_CANunit->tx_buffer_push_index;
	_gp_CANunit->tx_buffer_push_index = (_gp_CANunit->tx_buffer_push_index + 1) % CAN_TX_CMD_MAX_NUMBER;
	BSP_CAN1_IT_ENABLE();
	
	/*判断待存入缓冲区状态*/
	if(_gp_CANunit->tx_buffer[tx_buffer_push_index].status == TX_BUFFER_NULL)
	{
		for(j = 0; j < CAN_TX_CMD_MAX_SIZE; j++)
		{
			if(j == 0)	/*第一帧保存流水号和状态字*/
			{
				for(k = 0; k < 8; k++)
				{
					switch(k)
					{
						case 0:
							_gp_CANunit->tx_buffer[tx_buffer_push_index].buffer[j][k] = *p_serial_number;	/*存入流水号*/
							break;
						case 1:
							_gp_CANunit->tx_buffer[tx_buffer_push_index].buffer[j][k] = *p_status;			/*存入状态字*/
							break;
						case 2:
							_gp_CANunit->tx_buffer[tx_buffer_push_index].buffer[j][k] = (*p_status) >> 8;	/*存入状态字*/
							break;
						default:
							if(count < (len - 3))	/*扣除开头的流水号和状态字*/
							{
								_gp_CANunit->tx_buffer[tx_buffer_push_index].buffer[j][k] = p_data[count++];	/*存入数据*/
								if((k == 7) && (count >= (len - 3)))	/*本帧刚好存完，记录最后一帧的长度*/
								{
									last_frame_flag = 1;
									_gp_CANunit->tx_buffer[tx_buffer_push_index].last_frame_byte = k + 1;		/*记录最后一帧的长度*/
								}
							}
							else
							{
								if(last_frame_flag == 0)								/*最后一帧标识*/
								{
									last_frame_flag = 1;
									_gp_CANunit->tx_buffer[tx_buffer_push_index].last_frame_byte = k;		/*记录最后一帧的长度*/
								}
								_gp_CANunit->tx_buffer[tx_buffer_push_index].buffer[j][k] = 0;				/*补齐0*/
							}
							break;
					}
				}
				if(count >= (len - 3))	/*存完*/
				{
					_gp_CANunit->tx_buffer[tx_buffer_push_index].buffer[j][0] |= ((end_mark == MARK_END)? 0x80 : 0x00);	/*打上结束帧标志，如果数据未结束则不打*/
					_gp_CANunit->tx_buffer[tx_buffer_push_index].can_id = can_id;
					_gp_CANunit->tx_buffer[tx_buffer_push_index].size = j + 1;
					_gp_CANunit->tx_buffer[tx_buffer_push_index].status = TX_BUFFER_BUSY;
					OSSemPost(_gp_CANunit->tx_sem);
					return true;
				}
				else
				{
					_gp_CANunit->tx_buffer[tx_buffer_push_index].buffer[j][0] &= 0x7F;	/*打上非结束帧标志*/
				}
			}
			else
			{
				for(k = 0; k < 8; k++)
				{
					switch(k)
					{
						case 0:
							_gp_CANunit->tx_buffer[tx_buffer_push_index].buffer[j][k] = *p_serial_number;	/*存入流水号*/
							break;
						default:
							if(count < (len - 3))
							{
								_gp_CANunit->tx_buffer[tx_buffer_push_index].buffer[j][k] = p_data[count++];	/*存入数据*/
								if((k == 7) && (count >= (len - 3)))	/*本帧刚好存完，记录最后一帧的长度*/
								{
									last_frame_flag = 1;
									_gp_CANunit->tx_buffer[tx_buffer_push_index].last_frame_byte = k + 1;		/*记录最后一帧的长度*/
								}
							}
							else
							{
								if(last_frame_flag == 0)								/*最后一帧标识*/
								{
									last_frame_flag = 1;
									_gp_CANunit->tx_buffer[tx_buffer_push_index].last_frame_byte = k;		/*记录最后一帧的长度*/
								}
								_gp_CANunit->tx_buffer[tx_buffer_push_index].buffer[j][k] = 0;				/*补齐0*/
							}
							break;
					}
				}
				if(count >= (len - 3))	/*存完*/
				{
					_gp_CANunit->tx_buffer[tx_buffer_push_index].buffer[j][0] |= ((end_mark == MARK_END) ? 0x80 : 0x00);	/*打上结束帧标志，如果数据未结束则不打*/
					_gp_CANunit->tx_buffer[tx_buffer_push_index].can_id = can_id;
					_gp_CANunit->tx_buffer[tx_buffer_push_index].size = j + 1;
					_gp_CANunit->tx_buffer[tx_buffer_push_index].status = TX_BUFFER_BUSY;
					OSSemPost(_gp_CANunit->tx_sem);
					return true;
				}
				else
				{
					_gp_CANunit->tx_buffer[tx_buffer_push_index].buffer[j][0] &= 0x7F;	/*打上非结束帧标志*/
				}
			}
		}
	}
			
	return false;	/*发送缓存满*/
}

/* CAN发送一个接收应答信息到发送缓存
  --------------------------------- OK
  写入缓冲区，等待发送
  入口：CAN ID，流水号
  返回值：true成功，false失败
*/
bool Drv_CANProtocol_SendResponseToBuffer(uint8_t can_id, uint8_t serial_number)
{
	uint8_t data_out[3] = {0};
	bool err = true;
	uint8_t count = 0;	/*重发记数*/
			
	data_out[0] = serial_number;			/*流水号*/
	data_out[1] = CAN_STATUS_WORD_RESPONSE;	/*状态字*/
	data_out[2] = CAN_STATUS_WORD_RESPONSE >> 8;
	
	/*中断调用，不加锁*/
	do {
		count++;
		err = _Drv_CANProtocol_SendToBuffer(can_id, data_out, 3, (((serial_number & 0x80) != 0) ? MARK_END : MARK_NO_END));
		if(err != false)
			break;
		else
		{
			OSTimeDlyHMSM(0, 0, 0, 1);	/*延时1ms再试*/
		}
	} while(count < 5);
		
	return err;
}

/* CAN发送一个报错信息到发送缓存
  --------------------------------- OK
  写入缓冲区，等待发送
  入口：CAN ID，流水号，报错信息
  返回值：true成功，false失败
*/
bool Drv_CANProtocol_SendErrorToBuffer(uint8_t can_id, uint8_t serial_number, uint16_t cmd, uint32_t frame_id, uint8_t err1, uint8_t err2, uint8_t err3)
{
	DRV_CAN_RETURN_TYPE     data_out1 = {0};
	DRV_CAN_RETURN_ERR_TYPE data_out2 = {0};
	uint8_t os_err = OS_ERR_NONE;
	bool err = true;
	uint8_t count = 0;
	
	/*上报执行失败*/
	data_out1.serial_number = serial_number;		/*流水号*/
	data_out1.cmd_status = CAN_STATUS_WORD_FAILED;	/*状态字*/
	data_out1.can_id = can_id;
	data_out1.msg_type = CAN_TYPE_WORD_RET;
	data_out1.len = 4;
	data_out1.frame_id = frame_id;
	data_out1.cmd = cmd;
	data_out1.status = CAN_STATUS_WORD_FAILED;
	
	OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
	if(os_err != OS_ERR_NONE)
	{
		return false;
	}
	do {
		count++;
		err = _Drv_CANProtocol_SendToBuffer(can_id, (uint8_t *)(&data_out1), sizeof(DRV_CAN_RETURN_TYPE), MARK_END);
		if(err != false)
			break;
		else
		{
			OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
			OSTimeDlyHMSM(0, 0, 0, 1);	/*延时1ms再试*/
			OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
			if(os_err != OS_ERR_NONE)
			{
				return false;
			}
		}
	} while(count < 5);
	OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
	
	if(count >= 5)
		return false;
	
	/*上报报警信息*/
	data_out2.serial_number = serial_number;		/*流水号*/
	data_out2.cmd_status = CAN_STATUS_WORD_FAILED;	/*状态字*/
	data_out2.can_id = can_id;
	data_out2.msg_type = CAN_TYPE_WORD_RET;
	data_out2.len = 7;
	data_out2.frame_id = frame_id;
	data_out2.cmd = 0xff04;
	data_out2.status = 4;
	data_out2.err1 = err1;
	data_out2.err2 = err2;
	data_out2.err3 = err3;
	
	OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
	if(os_err != OS_ERR_NONE)
	{
		return false;
	}
	count = 0;
	do {
		count++;
		err = _Drv_CANProtocol_SendToBuffer(can_id, (uint8_t *)(&data_out2), sizeof(DRV_CAN_RETURN_ERR_TYPE), MARK_END);
		if(err != false)
			break;
		else
		{
			OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
			OSTimeDlyHMSM(0, 0, 0, 1);	/*延时1ms再试*/
			OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
			if(os_err != OS_ERR_NONE)
			{
				return false;
			}
		}
	} while(count < 5);
	OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
		
	return err;
}

/* CAN发送一个执行成功信息到发送缓存
  --------------------------------- OK
  写入缓冲区，等待发送
  入口：CAN ID，流水号，命令字，firm id
  返回值：true成功，false失败
*/
bool Drv_CANProtocol_SendDoneToBuffer(uint8_t can_id, uint8_t serial_number, uint16_t cmd, uint32_t frame_id)
{
	DRV_CAN_RETURN_TYPE data_out = {0};
	uint8_t os_err = OS_ERR_NONE;
	bool err = true;
	uint8_t count = 0;
			
	data_out.serial_number = serial_number;		/*流水号*/
	data_out.cmd_status = CAN_STATUS_WORD_DONE;	/*状态字*/
	data_out.can_id = can_id;
	data_out.msg_type = CAN_TYPE_WORD_RET;
	data_out.len = 4;
	data_out.frame_id = frame_id;
	data_out.cmd = cmd;
	data_out.status = CAN_STATUS_WORD_DONE;
	
	OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
	if(os_err != OS_ERR_NONE)
	{
		return false;
	}
	do {
		count++;
		err = _Drv_CANProtocol_SendToBuffer(can_id, (uint8_t *)(&data_out), sizeof(DRV_CAN_RETURN_TYPE), MARK_END);
		if(err != false)
			break;
		else
		{
			OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
			OSTimeDlyHMSM(0, 0, 0, 1);	/*延时1ms再试*/
			OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
			if(os_err != OS_ERR_NONE)
			{
				return false;
			}
		}
	} while(count < 5);
	OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
		
	return err;
}

/* CAN发送一个接收完成信息到发送缓存
  --------------------------------- 停用
  写入缓冲区，等待发送
  入口：CAN ID，流水号
  返回值：true成功，false失败
*/
bool Drv_CANProtocol_SendAckToBuffer(uint8_t can_id, uint8_t serial_number, uint16_t cmd, uint32_t frame_id)
{
	uint8_t data_out[17] = {0};
	uint8_t os_err = OS_ERR_NONE;
	bool err = true;
	uint8_t count = 0;
	
	data_out[0] = serial_number;		/*流水号*/
	data_out[1] = CAN_STATUS_WORD_ACK;	/*状态字*/
	data_out[2] = CAN_STATUS_WORD_ACK >> 8;
	data_out[3] = 0;
	data_out[4] = 0;
	data_out[5] = can_id;
	data_out[6] = CAN_TYPE_WORD_ACK;
	data_out[7] = 4;
	data_out[8] = 0;
	data_out[9] = frame_id;
	data_out[10] = frame_id >> 8;
	data_out[11] = frame_id >> 16;
	data_out[12] = frame_id >> 24;
	data_out[13] = cmd;
	data_out[14] = cmd >> 8;
	data_out[15] = 0;
	data_out[16] = 0;
	
	OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
	if(os_err != OS_ERR_NONE)
	{
		return false;
	}
	do {
		count++;
		err = _Drv_CANProtocol_SendToBuffer(can_id, data_out, 17, MARK_END);
		if(err != false)
			break;
		else
		{
			OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
			OSTimeDlyHMSM(0, 0, 0, 1);	/*延时1ms再试*/
			OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
			if(os_err != OS_ERR_NONE)
			{
				return false;
			}
		}
	} while(count < 5);
	OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
	
	return err;
}

/* CAN发送一个数据信息到发送缓存
  ---------------------------------
  写入缓冲区，等待发送
  入口：CAN ID，流水号，待发送的数据6个字节
  返回值：true成功，false失败
*/
bool Drv_CANProtocol_SendDataToBuffer(uint8_t can_id, uint8_t serial_number, uint16_t cmd, uint32_t frame_id, uint32_t data, uint8_t status)
{
	DRV_CAN_RETURN_TYPE      data_out1 = {0};
	DRV_CAN_RETURN_DATA_TYPE data_out2 = {0};
	uint8_t os_err = OS_ERR_NONE;
	bool err = true;
	uint8_t count = 0;
	
	/*上报执行成功*/
	data_out1.serial_number = serial_number;		/*流水号*/
	data_out1.cmd_status = CAN_STATUS_WORD_DONE;	/*状态字*/
	data_out1.can_id = can_id;
	data_out1.msg_type = CAN_TYPE_WORD_RET;
	data_out1.len = 4;
	data_out1.frame_id = frame_id;
	data_out1.cmd = cmd;
	data_out1.status = CAN_STATUS_WORD_DONE;
	
	OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
	if(os_err != OS_ERR_NONE)
	{
		return false;
	}
	do {
		count++;
		err = _Drv_CANProtocol_SendToBuffer(can_id, (uint8_t *)(&data_out1), sizeof(DRV_CAN_RETURN_TYPE), MARK_END);
		if(err != false)
			break;
		else
		{
			OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
			OSTimeDlyHMSM(0, 0, 0, 1);	/*延时1ms再试*/
			OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
			if(os_err != OS_ERR_NONE)
			{
				return false;
			}
		}
	} while(count < 5);
	OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
	
	if(count >= 5)
		return false;
	
	/*上报数据信息*/
	data_out2.serial_number = serial_number;		/*流水号*/
	data_out2.cmd_status = CAN_STATUS_WORD_DONE;	/*状态字*/
	data_out2.can_id = can_id;
	data_out2.msg_type = CAN_TYPE_WORD_ASK;
	data_out2.len = 8;
	data_out2.frame_id = frame_id;
	data_out2.cmd = 0xff04;
	data_out2.status = status;
	data_out2.data = data;
	
	OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
	if(os_err != OS_ERR_NONE)
	{
		return false;
	}
	count = 0;
	do {
		count++;
		err = _Drv_CANProtocol_SendToBuffer(can_id, (uint8_t *)(&data_out2), sizeof(DRV_CAN_RETURN_DATA_TYPE), MARK_END);
		if(err != false)
			break;
		else
		{
			OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
			OSTimeDlyHMSM(0, 0, 0, 1);	/*延时1ms再试*/
			OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
			if(os_err != OS_ERR_NONE)
			{
				return false;
			}
		}
	} while(count < 5);
	OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
		
	return err;
}

/* CAN发送一串数据信息到发送缓存
  ---------------------------------
  写入缓冲区，等待发送
  入口：CAN ID，流水号，待发送的数据
  返回值：true成功，false失败
*/
bool Drv_CANProtocol_SendDatasToBuffer(uint8_t can_id, uint8_t serial_number, uint16_t cmd, uint32_t frame_id, uint8_t* datas, uint16_t len, uint8_t status)
{
	DRV_CAN_RETURN_TYPE	data_out1 = {0};
	uint8_t				data_out2[17 + SEND_MAX_NUMBER] = {0};
	uint8_t os_err = OS_ERR_NONE;
	bool err = true;
	uint8_t i = 0;
	uint16_t con = 0;	/*已发计数*/
	uint8_t count = 0;
	
	/*上报执行成功*/
	data_out1.serial_number = serial_number;		/*流水号*/
	data_out1.cmd_status = CAN_STATUS_WORD_DONE;	/*状态字*/
	data_out1.can_id = can_id;
	data_out1.msg_type = CAN_TYPE_WORD_RET;
	data_out1.len = 4;
	data_out1.frame_id = frame_id;
	data_out1.cmd = cmd;
	data_out1.status = CAN_STATUS_WORD_DONE;
	
	OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
	if(os_err != OS_ERR_NONE)
	{
		return false;
	}
	do {
		count++;
		err = _Drv_CANProtocol_SendToBuffer(can_id, (uint8_t *)(&data_out1), sizeof(DRV_CAN_RETURN_TYPE), MARK_END);
		if(err != false)
			break;
		else
		{
			OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
			OSTimeDlyHMSM(0, 0, 0, 1);	/*延时1ms再试*/
			OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
			if(os_err != OS_ERR_NONE)
			{
				return false;
			}
		}
	} while(count < 5);
	OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
	
	if(count >= 5)
		return false;
		
	/*上报数据信息*/
	
//	if(len > SEND_MAX_NUMBER)	/*最大一个数据包发送SEND_MAX_NUMBER个字节*/
//		return false;
			
	if(len <= SEND_MAX_NUMBER)
	{
		uint16_t len16 = 2 + len + ((((len + 2) % 4) != 0) ? (4 - (len + 2) % 4) : 0);
		
		data_out2[0] = serial_number;		/*流水号*/
		data_out2[1] = CAN_STATUS_WORD_DONE;	/*状态字*/
		data_out2[2] = CAN_STATUS_WORD_DONE >> 8;
		data_out2[3] = 0;
		data_out2[4] = 0;
		data_out2[5] = can_id;
		data_out2[6] = CAN_TYPE_WORD_ASK;
		data_out2[7] = len16;
		data_out2[8] = len16 >> 8;
		data_out2[9] = frame_id;
		data_out2[10] = frame_id >> 8;
		data_out2[11] = frame_id >> 16;
		data_out2[12] = frame_id >> 24;
		data_out2[13] = 0x04;
		data_out2[14] = 0xff;
		data_out2[15] = status;
		data_out2[16] = 0;
		for(i = 0; i < len; i++)
		{
			data_out2[17 + i] = datas[i];
		}
	
		OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
		if(os_err != OS_ERR_NONE)
		{
			return false;
		}
		count = 0;
		do {
			count++;
			err = _Drv_CANProtocol_SendToBuffer(can_id, data_out2, 17 + len + ((((len + 4) % 4) != 0) ? (4 - (len + 4) % 4) : 0), MARK_END);
			if(err != false)
				break;
			else
			{
				OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
				OSTimeDlyHMSM(0, 0, 0, 1);	/*延时1ms再试*/
				OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
				if(os_err != OS_ERR_NONE)
				{
					return false;
				}
			}
		} while(count < 5);
		OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
	}
	else		
	{
		uint16_t len16 = 2 + len + ((((len + 2) % 4) != 0) ? (4 - (len + 2) % 4) : 0);
		
		data_out2[0] = serial_number;		/*流水号*/
		data_out2[1] = CAN_STATUS_WORD_DONE;	/*状态字*/
		data_out2[2] = CAN_STATUS_WORD_DONE >> 8;
		data_out2[3] = 0;
		data_out2[4] = 0;
		data_out2[5] = can_id;
		data_out2[6] = CAN_TYPE_WORD_ASK;
		data_out2[7] = len16;
		data_out2[8] = len16 >> 8;
		data_out2[9] = frame_id;
		data_out2[10] = frame_id >> 8;
		data_out2[11] = frame_id >> 16;
		data_out2[12] = frame_id >> 24;
		data_out2[13] = 0x04;
		data_out2[14] = 0xff;
		data_out2[15] = status;
		data_out2[16] = 0;
		for(i = 0; i < SEND_MAX_NUMBER; i++)
		{
			data_out2[17 + i] = datas[i];
		}
		con = SEND_MAX_NUMBER;
		OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
		if(os_err != OS_ERR_NONE)
		{
			return false;
		}
		count = 0;
		do {
			count++;
			err = _Drv_CANProtocol_SendToBuffer(can_id, data_out2, 17 + SEND_MAX_NUMBER, MARK_NO_END);
			if(err != false)
				break;
			else
			{
				OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
				OSTimeDlyHMSM(0, 0, 0, 1);	/*延时1ms再试*/
				OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
				if(os_err != OS_ERR_NONE)
				{
					return false;
				}
			}
		} while(count < 5);
		OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
		
		if(count >= 5)
			return false;
		
		len -= SEND_MAX_NUMBER;	/*待发倒计数*/
		
		while(len > 0)	/*还有数据要发*/
		{
			if(len > SEND_MAX_NUMBER + 16)	/*还有下一包数据*/
			{
				data_out2[0] = serial_number;		/*流水号*/
				for(i = 0; i < (SEND_MAX_NUMBER + 16); i++)
				{
					data_out2[1 + i] = datas[con + i];
				}
				con += (SEND_MAX_NUMBER + 16);
				OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
				if(os_err != OS_ERR_NONE)
				{
					return false;
				}
				count = 0;
				do {
					count++;
					err = _Drv_CANProtocol_SendToBuffer(can_id, data_out2, 1 + (SEND_MAX_NUMBER + 16), MARK_NO_END);
					if(err != false)
						break;
					else
					{
						OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
						OSTimeDlyHMSM(0, 0, 0, 1);	/*延时1ms再试*/
						OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
						if(os_err != OS_ERR_NONE)
						{
							return false;
						}
					}
				} while(count < 5);
				OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
				
				len -= SEND_MAX_NUMBER + 16;	/*待发倒计数*/
			}
			else	/*没有下一包数据*/
			{
				data_out2[0] = serial_number;		/*流水号*/
				for(i = 0; i < len; i++)
				{
					data_out2[1 + i] = datas[con + i];
				}
				con += len;
				OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
				if(os_err != OS_ERR_NONE)
				{
					return false;
				}
				count = 0;
				do {
					count++;
					err = _Drv_CANProtocol_SendToBuffer(can_id, data_out2, 1 + len, MARK_END);
					if(err != false)
						break;
					else
					{
						OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
						OSTimeDlyHMSM(0, 0, 0, 1);	/*延时1ms再试*/
						OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
						if(os_err != OS_ERR_NONE)
						{
							return false;
						}
					}
				} while(count < 5);
				OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
				
				len = 0;
			}
		}
	}
	
	return err;
}

/* CAN发送执行成功带数据到发送缓存
  ---------------------------------
  写入缓冲区，等待发送
  入口：CAN ID，流水号，待发送的数据
  返回值：true成功，false失败
*/
bool Drv_CANProtocol_SendDoneAndDatasToBuffer(uint8_t can_id, uint8_t serial_number, uint16_t cmd, uint32_t frame_id, uint8_t* datas, uint16_t len)
{
	DRV_CAN_RETURN_TYPE	data_out1 = {0};
	uint8_t				data_out2[17 + SEND_MAX_NUMBER] = {0};
	uint8_t os_err = OS_ERR_NONE;
	bool err = true;
	uint8_t i = 0;
	uint16_t con = 0;	/*已发计数*/
	uint8_t count = 0;
	
	if(len <= SEND_MAX_NUMBER)
	{
		uint16_t len16 = 4 + len + (((len % 4) != 0) ? (4 - len % 4): 0);
		
		data_out2[0] = serial_number;		/*流水号*/
		data_out2[1] = CAN_STATUS_WORD_DONE;	/*状态字*/
		data_out2[2] = CAN_STATUS_WORD_DONE >> 8;
		data_out2[3] = 0;
		data_out2[4] = 0;
		data_out2[5] = can_id;
		data_out2[6] = CAN_TYPE_WORD_RET;
		data_out2[7] = len16;
		data_out2[8] = len16 >> 8;
		data_out2[9] = frame_id;
		data_out2[10] = frame_id >> 8;
		data_out2[11] = frame_id >> 16;
		data_out2[12] = frame_id >> 24;
		data_out2[13] = cmd;
		data_out2[14] = cmd >> 8;
		data_out2[15] = CAN_STATUS_WORD_DONE;
		data_out2[16] = CAN_STATUS_WORD_DONE >> 8;
		for(i = 0; i < len; i++)
		{
			data_out2[17 + i] = datas[i];
		}
	
		OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
		if(os_err != OS_ERR_NONE)
		{
			return false;
		}
		count = 0;
		do {
			count++;
			err = _Drv_CANProtocol_SendToBuffer(can_id, data_out2, 17 + len + ((((len + 4) % 4) != 0) ? (4 - (len + 4) % 4) : 0), MARK_END);
			if(err != false)
				break;
			else
			{
				OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
				OSTimeDlyHMSM(0, 0, 0, 1);	/*延时1ms再试*/
				OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
				if(os_err != OS_ERR_NONE)
				{
					return false;
				}
			}
		} while(count < 5);
		OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
	}
	else		
	{
		uint16_t len16 = 2 + len + ((((len + 2) % 4) != 0) ? (4 - (len + 2) % 4) : 0);
		
		data_out2[0] = serial_number;		/*流水号*/
		data_out2[1] = CAN_STATUS_WORD_DONE;	/*状态字*/
		data_out2[2] = CAN_STATUS_WORD_DONE >> 8;
		data_out2[3] = 0;
		data_out2[4] = 0;
		data_out2[5] = can_id;
		data_out2[6] = CAN_TYPE_WORD_RET;
		data_out2[7] = len16;
		data_out2[8] = len16 >> 8;
		data_out2[9] = frame_id;
		data_out2[10] = frame_id >> 8;
		data_out2[11] = frame_id >> 16;
		data_out2[12] = frame_id >> 24;
		data_out2[13] = cmd;
		data_out2[14] = cmd >> 8;
		data_out2[15] = CAN_STATUS_WORD_DONE;
		data_out2[16] = CAN_STATUS_WORD_DONE >> 8;
		for(i = 0; i < SEND_MAX_NUMBER; i++)
		{
			data_out2[17 + i] = datas[i];
		}
		con = SEND_MAX_NUMBER;
		OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
		if(os_err != OS_ERR_NONE)
		{
			return false;
		}
		count = 0;
		do {
			count++;
			err = _Drv_CANProtocol_SendToBuffer(can_id, data_out2, 17 + SEND_MAX_NUMBER, MARK_NO_END);
			if(err != false)
				break;
			else
			{
				OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
				OSTimeDlyHMSM(0, 0, 0, 1);	/*延时1ms再试*/
				OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
				if(os_err != OS_ERR_NONE)
				{
					return false;
				}
			}
		} while(count < 5);
		OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
		
		if(count >= 5)
			return false;
		
		len -= SEND_MAX_NUMBER;	/*待发倒计数*/
		
		while(len > 0)	/*还有数据要发*/
		{
			if(len > SEND_MAX_NUMBER + 16)	/*还有下一包数据*/
			{
				data_out2[0] = serial_number;		/*流水号*/
				for(i = 0; i < (SEND_MAX_NUMBER + 16); i++)
				{
					data_out2[1 + i] = datas[con + i];
				}
				con += (SEND_MAX_NUMBER + 16);
				OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
				if(os_err != OS_ERR_NONE)
				{
					return false;
				}
				count = 0;
				do {
					count++;
					err = _Drv_CANProtocol_SendToBuffer(can_id, data_out2, 1 + (SEND_MAX_NUMBER + 16), MARK_NO_END);
					if(err != false)
						break;
					else
					{
						OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
						OSTimeDlyHMSM(0, 0, 0, 1);	/*延时1ms再试*/
						OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
						if(os_err != OS_ERR_NONE)
						{
							return false;
						}
					}
				} while(count < 5);
				OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
				
				len -= SEND_MAX_NUMBER + 16;	/*待发倒计数*/
			}
			else	/*没有下一包数据*/
			{
				data_out2[0] = serial_number;		/*流水号*/
				for(i = 0; i < len; i++)
				{
					data_out2[1 + i] = datas[con + i];
				}
				con += len;
				OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
				if(os_err != OS_ERR_NONE)
				{
					return false;
				}
				count = 0;
				do {
					count++;
					err = _Drv_CANProtocol_SendToBuffer(can_id, data_out2, 1 + len, MARK_END);
					if(err != false)
						break;
					else
					{
						OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
						OSTimeDlyHMSM(0, 0, 0, 1);	/*延时1ms再试*/
						OSMutexPend(_gp_CANunit->send_to_buffer_mutex, 100, &os_err);
						if(os_err != OS_ERR_NONE)
						{
							return false;
						}
					}
				} while(count < 5);
				OSMutexPost(_gp_CANunit->send_to_buffer_mutex);
				
				len = 0;
			}
		}
	}
	
	return err;
}

/* CAN释放指定接收缓冲区
  --------------------------------- OK
  入口：CAN ID编号（不是CAN ID），缓冲区地址
  返回值：true成功，false失败
*/
bool Drv_CANProtocol_RXBufferFree(uint8_t id_number, DRV_CAN_RX_BUFFER_TYPE* p_rx_buffer)
{
	uint32_t i;
	
	for(i = 0; i < CAN_RX_CMD_MAX_NUMBER; i++)
	{
		if(p_rx_buffer == &(_gp_CANunit->rx_buffer[id_number][i]))	/*如果找到*/
		{
			memset((void*)(&(_gp_CANunit->rx_buffer[id_number][i])), 0, sizeof(DRV_CAN_RX_BUFFER_TYPE));
			_gp_CANunit->rx_buffer[id_number][i].status = RX_BUFFER_NULL;
			return true;
		}
	}
	return false;
}

/* CAN释放指定发送缓冲区
  --------------------------------- OK
  入口：缓冲区地址
  返回值：true成功，false失败
*/
bool Drv_CANProtocol_TXBufferFree(DRV_CAN_TX_BUFFER_TYPE* p_tx_buffer)
{
	uint32_t i;
	
	for(i = 0; i < CAN_TX_CMD_MAX_NUMBER; i++)
	{
		if(p_tx_buffer == &(_gp_CANunit->tx_buffer[i]))	/*如果找到*/
		{
			memset((void*)(&(_gp_CANunit->tx_buffer[i])), 0, sizeof(DRV_CAN_TX_BUFFER_TYPE));
			_gp_CANunit->tx_buffer[i].status = TX_BUFFER_NULL;
			return true;
		}
	}
	return false;
}

/* CAN获取一个数据包缓存的指针
  --------------------------------- OK
  写入缓冲区，等待发送
  入口：CAN ID编号(不是CAN ID)
  返回值：接收缓存地址
*/
DRV_CAN_RX_BUFFER_TYPE* Drv_CANProtocol_GetPacket(uint8_t id_number)
{
	uint32_t i;
	
	for(i = 0; i < CAN_RX_CMD_MAX_NUMBER; i++)	/*查找指定ID编号的接收完成缓冲区*/
	{
		if(_gp_CANunit->rx_buffer[id_number][i].status == RX_BUFFER_READY)
		{
			return &(_gp_CANunit->rx_buffer[id_number][i]);
		}
	}
	return NULL;
}

/* CAN发送一个数据帧
  --------------------------------- OK
  从发送缓冲区发送一个数据帧
  入口：
  返回值：true成功，false失败
*/
bool Drv_CANProtocol_PutFrame(void)
{
	uint32_t j;
	
	/*判断待发送的数据缓存*/
	if(_gp_CANunit->tx_buffer[_gp_CANunit->tx_buffer_pop_index].status == TX_BUFFER_BUSY)
	{
		for(j = 0; j < CAN_TX_CMD_MAX_SIZE; j++)
		{
			if(j > (_gp_CANunit->tx_buffer[_gp_CANunit->tx_buffer_pop_index].size - 1))	/*发送了指定个数的帧，认为发送结束*/
			{
				Drv_CANProtocol_TXBufferFree(&(_gp_CANunit->tx_buffer[_gp_CANunit->tx_buffer_pop_index]));	/*释放该发送缓冲区*/
				_gp_CANunit->tx_buffer_pop_index = (_gp_CANunit->tx_buffer_pop_index + 1) % CAN_TX_CMD_MAX_NUMBER;	/*发送缓存下标循环自增*/
				return true;
			}
			else if(j == (_gp_CANunit->tx_buffer[_gp_CANunit->tx_buffer_pop_index].size - 1))	/*发送最后一帧*/
			{
				BSP_CAN1_TX(_gp_CANunit->tx_buffer[_gp_CANunit->tx_buffer_pop_index].can_id, &(_gp_CANunit->tx_buffer[_gp_CANunit->tx_buffer_pop_index].buffer[j][0]), _gp_CANunit->tx_buffer[_gp_CANunit->tx_buffer_pop_index].last_frame_byte);	/*发送数据帧*/
			}
			else
			{
				BSP_CAN1_TX(_gp_CANunit->tx_buffer[_gp_CANunit->tx_buffer_pop_index].can_id, &(_gp_CANunit->tx_buffer[_gp_CANunit->tx_buffer_pop_index].buffer[j][0]), 8);	/*发送数据帧*/
			}
		}
		if(j >= CAN_TX_CMD_MAX_SIZE)	/*缓冲区发完，也认为发送结束*/
		{
			Drv_CANProtocol_TXBufferFree(&(_gp_CANunit->tx_buffer[_gp_CANunit->tx_buffer_pop_index]));	/*释放该发送缓冲区*/
			_gp_CANunit->tx_buffer_pop_index = (_gp_CANunit->tx_buffer_pop_index + 1) % CAN_TX_CMD_MAX_NUMBER;	/*发送缓存下标循环自增*/
			return true;
		}
	}
	else
	{
//		if(_gp_CANunit->tx_buffer_pop_index != _gp_CANunit->tx_buffer_push_index)
//		{
//			_gp_CANunit->tx_buffer_pop_index = (_gp_CANunit->tx_buffer_pop_index + 1) % CAN_TX_CMD_MAX_NUMBER;	/*发送缓存下标循环自增*/
//		}
	}
	
	return false;	/*未找到要发送的数据帧*/
}

/* CAN接收缓冲区超时检测
  --------------------------------- OK
  接收缓冲区中长帧超时检测
  入口：
  返回值：true成功，false失败
*/
bool Drv_CANProtocol_RX_TimeOutCheck(uint8_t id_number, DRV_CAN_RX_BUFFER_TYPE* p_rx_buffer)
{
	uint32_t i;
	
	/*查找等待结束帧的缓冲区*/
	for(i = 0; i < CAN_RX_CMD_MAX_NUMBER; i++)
	{
		/*找到*/
		if((p_rx_buffer + i)->status == RX_BUFFER_WAIT)
		{
			if((p_rx_buffer + i)->timer == 1)	/*计时结束，报错*/
			{
				Drv_CANProtocol_SendErrorToBuffer((p_rx_buffer + i)->can_id, (p_rx_buffer + i)->buffer[0][0] & 0x7F, 
													((uint16_t)((p_rx_buffer + i)->buffer[0][2]) << 8 | (uint16_t)((p_rx_buffer + i)->buffer[0][1])), 
													0xFFFFFFFF, 
													0xFF, 0xFF, 0x07);		/*报错，接收长帧超时*/
				Drv_CANProtocol_RXBufferFree(id_number, p_rx_buffer + i);				/*释放该接收缓冲区*/
			}
			else
			{
				((p_rx_buffer + i)->timer)--;	/*时间递减*/
			}
		}
	}
	
	return true;
}

/* 内部函数
 ********************************************************/

/* CAN接收一个数据帧到接收缓存
  --------------------------------- OK
  写入缓冲区，等待发送
  入口：CAN ID，接收缓冲区，接收完成信号量，数据指针
  返回值：true成功，false失败
*/
static bool _Drv_CANProtocol_GetFrame(uint8_t can_id, \
									  DRV_CAN_RX_BUFFER_TYPE* p_rx_buffer, OS_EVENT* p_rx_sem, \
									  uint8_t* data)
{
	uint32_t i, j, k;
	
	/*查找等待结束帧的缓冲区*/
	for(i = 0; i < CAN_RX_CMD_MAX_NUMBER; i++)
	{
		/*找到，并且其流水号相等的，可存入*/
		if((p_rx_buffer + i)->status == RX_BUFFER_WAIT)
		{
			if(((p_rx_buffer + i)->buffer[0][0] & 0x7F) == (data[0] & 0x7F))
			{
				/*查找存入位置*/
				for(j = 0; j < CAN_RX_CMD_MAX_SIZE; j++)
				{
					/*找到存入位置，存入，流水号为0表示该存储区为空*/
					if(((p_rx_buffer + i)->buffer[j][0]) == 0)	
					{
						/*更新超时时间(ms)*/
						(p_rx_buffer + i)->timer = 200;	/*200ms超时*/
						/*再存数据*/
						for(k = 0; k < 8; k++)
						{
							(p_rx_buffer + i)->buffer[j][k] = data[k];
						}
						if((data[0] & 0x80) != 0)	/*是结束帧*/
						{
							(p_rx_buffer + i)->status = RX_BUFFER_READY;	/*存入结束帧后，切换缓冲区状态为数据准备好*/
							(p_rx_buffer + i)->size++;						/*记录帧个数*/
							_Drv_CANProtocol_Formatting(p_rx_buffer + i);	/*去掉中间的流水号*/
							OSSemPost(p_rx_sem);
						}
						else
						{
							(p_rx_buffer + i)->status = RX_BUFFER_WAIT;		/*存入非结束帧后，切换缓冲区状态为数据等待*/
							(p_rx_buffer + i)->size++;						/*记录帧个数*/
						}
						Drv_CANProtocol_SendResponseToBuffer(can_id, data[0]);	/*发送应答帧，注意，上位机发的流水号未结束，回的应答流水号也是未结束*/
						return true;	/*存好后退出*/
					}
				}
				/*没找到存入位置*/
				Drv_CANProtocol_RXBufferFree(0, p_rx_buffer + i);				/*释放该接收缓冲区*/
				Drv_CANProtocol_SendErrorToBuffer(can_id, data[0] & 0x7F, 
													((uint16_t)(data[2]) << 8 | (uint16_t)(data[1])), 
													0xFFFFFFFF, 
													0xFF, 0xFF, 0x03);		/*报错，长帧缓存满*/
				return false;
			}
		}
	}
	/*没找到，继续查找空闲缓冲区*/
	if(i == CAN_RX_CMD_MAX_NUMBER)
	{
		for(i = 0; i < CAN_RX_CMD_MAX_NUMBER; i++)
		{
			if((p_rx_buffer + i)->status == RX_BUFFER_NULL)
			{	
				/*找到空闲缓冲区，直接存入*/
				/*先存can_id，和超时时间(ms)*/
				(p_rx_buffer + i)->can_id = can_id;
				(p_rx_buffer + i)->timer = 200;	/*200ms超时*/
				/*再存数据*/
				for(k = 0; k < 8; k++)
				{
					(p_rx_buffer + i)->buffer[0][k] = data[k];
				}
				if((data[0] & 0x80) != 0)	/*是结束帧*/
				{
					(p_rx_buffer + i)->status = RX_BUFFER_READY;	/*存入结束帧后，切换缓冲区状态为数据准备好*/
					(p_rx_buffer + i)->size++;						/*记录帧个数*/
					_Drv_CANProtocol_Formatting(p_rx_buffer + i);	/*去掉中间的流水号*/
					OSSemPost(p_rx_sem);
				}
				else
				{
					(p_rx_buffer + i)->status = RX_BUFFER_WAIT;		/*存入非结束帧后，切换缓冲区状态为数据等待*/
					(p_rx_buffer + i)->size++;						/*记录帧个数*/
				}
				Drv_CANProtocol_SendResponseToBuffer(can_id, data[0]);	/*发送应答帧*/
				return true;	/*存好后退出*/
			}
		}
		/*没找到，回复缓冲区满*/
		Drv_CANProtocol_SendErrorToBuffer(can_id, data[0] & 0x7F, 
											((uint16_t)(data[2]) << 8 | (uint16_t)(data[1])), 
											0xFFFFFFFF, 
											0xFF, 0xFF, 0x04);		/*报错，缓冲区满，退出*/
		return false;
	}
	return true;
}

/* 缓冲区格式化
  --------------------------------- 
  去掉非头帧流水号
*/
static void _Drv_CANProtocol_Formatting(DRV_CAN_RX_BUFFER_TYPE* p_rx_buffer)
{
	uint16_t i, j;
	
	/*依次处理每一个帧*/
	for(i = 1; i < CAN_RX_CMD_MAX_SIZE; i++)
	{
		for(j = 1; j < 8; j++)
		{
			*(&(p_rx_buffer->buffer[i][j]) - i) = p_rx_buffer->buffer[i][j];
		}
	}
}

/* 外部函数
 ********************************************************/
 
/* CAN通信模块接收中断
  --------------------------------- OK
*/
void BSP_CAN1_RX_IRQHandler(uint32_t* p_std_id,	/*标准ID*/
							uint32_t* p_ext_id,	/*扩展ID*/
							uint8_t*  p_ide,	/*ID类型*/
							uint8_t*  p_rtr,	/*帧类型*/
							uint8_t*  p_dlc,	/*帧长度*/
							uint8_t*  p_data,	/*接收的数据，8个字节*/
							uint8_t*  p_fmi)
{
	uint8_t i;
	uint8_t id = (uint8_t)(*p_std_id & 0x0000007F);		/*获取ID号，只保留低7位，其中，上位机标识1位，ID号6位*/
	uint8_t serial_number = p_data[0] & 0x7F;			/*获取流水号,去掉结束帧标志*/
	
	DRV_CAN_RX_BUFFER_TYPE* p_rx_buffer = NULL;			/*用于指向选定的缓冲区、信号量*/
	OS_EVENT*               p_rx_sem    = NULL;
	
	if((id == 0) || (serial_number == 0x00))
	{
		Drv_CANProtocol_SendErrorToBuffer(id, serial_number, 
											((uint16_t)(p_data[2]) << 8 | (uint16_t)(p_data[1])), 
											0xFFFFFFFF, 0xFF, 0xFF, 0x01);		/*报错，接收ID为0或流水号为0*/
		return;	/*ID不能为0，流水号不能为0*/
	}
	
	/*第一步，判断ID号是否相符，不相符的不做处理*/	
	for(i = 0; i < 4; i++)
	{
		if(id == (_gp_CANunit->can_id[i] | 0x40))		/*id相符的，记录下来，且只接收上位机发的数据*/
		{
			p_rx_buffer = &(_gp_CANunit->rx_buffer[i][0]);
			p_rx_sem    = _gp_CANunit->rx_sem[i];
			break;
		}
	}
	if(i == 4)	/*没有相符的，退出不处理*/
	{
		Drv_CANProtocol_SendErrorToBuffer(id, serial_number, 
											((uint16_t)(p_data[2]) << 8 | (uint16_t)(p_data[1])), 
											0xFFFFFFFF, 0xFF, 0xFF, 0x02);		/*报错，接收ID不相符*/
		return;
	}
	else
	{
		/*第二步，将新收到的帧存入缓冲区，如果存不下，需要报错*/
		_Drv_CANProtocol_GetFrame(id & 0x03F, p_rx_buffer, p_rx_sem, p_data);	/*ID号去掉上位机标志*/
	}	
}
