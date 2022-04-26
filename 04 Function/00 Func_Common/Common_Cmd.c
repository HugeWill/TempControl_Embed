/*  File: Common_Cmd.h
    Discription: Define command words and command 
    format structure between upper machine and 
    lower machine 
    Author: HYC
    Date: 22/04/01
    Version: V1.0
    Release note: First edition
*/
#include "Common_Cmd.h"

#define MCU_UNIQUE_ID_ADDR 0x1FFFF7E8 /*STM32F1唯一ID起始地址*/
#define flag_update_address 0x807C000
#define flag_start_boot 0x0101
#define FirmwareVersion "V1.0.0.0"

DRV_MCC_TYPE*   _gp_StepPump = NULL; /*蠕动泵步进电机*/
DRV_VALVE_TYPE* _gp_V = NULL;	/*电磁阀/电磁铁*/
DRV_FAN_TYPE*	_gp_Fan[FAN_MAX] = {NULL};	/*风扇*/
DRV_PUMP_TYPE*  _gp_P[DCPUMP_MAX]  = {NULL};	/*泵/直流电机*/
DRV_HEAT_TYPE* _gp_H[HEAT_MAX] = {NULL};		/*加热膜*/
DRV_REFRIGERATING_TYPE* _gp_C[COLD_MAX] = {NULL};	/*制冷片*/
DRV_NTC_TYPE*	_gp_N[TEMP_SENSOR_MAX] = {NULL};		/*NTC温度传感器*/
DRV_SWITCHINGSENSOR_TYPE* _gp_S[LIQUIDE_MAX] = {NULL};		/*液位传感器*/
DRV_X100M_TYPE* _gp_X;								/*光纤温度传感器*/
DRV_SWITCHINGSENSOR_TYPE* _gp_B[SS_MAX];	/*旋钮传感器*/
DRV_AT24C32_TYPE* _gp_E = NULL;	/*参数存储器*/
DRV_LED_TYPE* _gp_LED[LED_MAX];

/*  业务层初始化接口
    入口：dlist 链表节点，ch模块id
    返回值：无
*/
void Func_Common_Init(DList* dlist)
{
    Func_PrivateInit1(dlist);
}



/* 握手
  ------------------------------
  返回值：true，成功；false，失败
*/
bool Func_Cmd_Com_Hello(void* p_buffer)
{
	COMMON_CMD_DATA* p_msg = (COMMON_CMD_DATA*)p_buffer;
	HANDSHAKE_TYPE* p_data = (HANDSHAKE_TYPE*)p_msg->data;
	COMMON_RETURN_DATA_TYPE* p_return = (COMMON_RETURN_DATA_TYPE*)malloc(sizeof(COMMON_RETURN_DATA_TYPE));

	uint32_t frame_head = p_msg->frame_head;
	uint16_t cmd = p_data->cmd;
	uint8_t length = sizeof(COMMON_RETURN_DATA_TYPE);
	p_return->cmd = p_data->cmd;
	p_return->rsv[0] = 0;
	p_return->rsv[1] = 0;
  _Drv_UsartReturnDoneToBuffer(frame_head,cmd,length,(uint8_t *)p_return);
	free(p_return);
	return true;
}

/* 泵/直流电机控制
  ------------------------------
  返回值：true，成功；false，失败
*/
bool Func_Cmd_Com_Pump_Control(void* p_buffer)
{
	uint8_t i = 0;
	COMMON_CMD_DATA* p_msg = (COMMON_CMD_DATA*)p_buffer;
	DCPUMPCONTROL_TYPE* p_data = (DCPUMPCONTROL_TYPE*)p_msg->data;
	COMMON_RETURN_DATA_TYPE* p_return = (COMMON_RETURN_DATA_TYPE*)malloc(sizeof(COMMON_RETURN_DATA_TYPE));
	RETURN_ERR_DATA_TYPE* p_err = (RETURN_ERR_DATA_TYPE*)malloc(sizeof(RETURN_ERR_DATA_TYPE));
	
	uint32_t frame_id = p_msg->frame_head;
	uint16_t cmd = p_data->cmd;
	p_return->cmd = cmd;
	p_err->err_code = ERR_CMD_FAIL;
	p_err->module_id = MODULE_DCPUMP;
	p_err->device_id = p_data->pump_num;
	
	if(p_data->pump_num<=DCPUMP_MAX)
	{
		if(p_data->pump_num<DCPUMP_MAX)
		{
			if(p_data->on_off)
			{
				PumpStart(_gp_P[p_data->pump_num]);
			}
			else
			{
				PumpStop(_gp_P[p_data->pump_num]);
			}
		}
		else
		{
			if(p_data->on_off)
			{
				for(i=0;i<DCPUMP_MAX;i++)
					PumpStart(_gp_P[i]);
			}
			else
			{
				for(i=0;i<DCPUMP_MAX;i++)
					PumpStop(_gp_P[i]);
			}
		}
	}
	else
	{
		_Drv_UsartReturnFailToBuffer(frame_id,cmd,sizeof(RETURN_ERR_DATA_TYPE),(uint8_t*)p_err);
		free(p_err);
		free(p_return);
		return false;
	}
	_Drv_UsartReturnDoneToBuffer(frame_id,cmd,sizeof(COMMON_RETURN_DATA_TYPE),(uint8_t*)p_return);
	free(p_err);
	free(p_return);
	return true;
}

/* 阀控制指令
  ------------------------------
  返回值：true，成功；false，失败
*/
bool Func_Cmd_Com_Valve_Control(void* p_buffer)
{
	COMMON_CMD_DATA* p_msg = (COMMON_CMD_DATA*)p_buffer;
	VALVECONTROL_TYPE* p_data = (VALVECONTROL_TYPE*)p_msg->data;
	COMMON_RETURN_DATA_TYPE* p_return = (COMMON_RETURN_DATA_TYPE*)malloc(sizeof(COMMON_RETURN_DATA_TYPE));
	RETURN_ERR_DATA_TYPE* p_err = (RETURN_ERR_DATA_TYPE*)malloc(sizeof(RETURN_ERR_DATA_TYPE));
	
	uint32_t frame_id = p_msg->frame_head;
	uint16_t cmd = p_data->cmd;
	p_return->cmd = cmd;
	p_err->err_code = ERR_CMD_FAIL;
	p_err->module_id = MODULE_VALVE;
	p_err->device_id = p_data->valve_num;
	
	if(p_data->valve_num<VALVE_MAX)
	{
		if(p_data->on_off)
			Valve_Open(_gp_V);
		else
			Valve_Close(_gp_V);
	}
	else
	{
		_Drv_UsartReturnFailToBuffer(frame_id,cmd,sizeof(RETURN_ERR_DATA_TYPE),(uint8_t*)p_err);
		free(p_err);
		free(p_return);
		return false;
	}
	_Drv_UsartReturnDoneToBuffer(frame_id,cmd,sizeof(COMMON_RETURN_DATA_TYPE),(uint8_t*)p_return);
	free(p_err);
	free(p_return);
	return true;

}

/* 风扇控制指令
  ------------------------------
  返回值：true，成功；false，失败
*/
bool Func_Cmd_Com_Fan_Control(void* p_buffer)
{
	uint8_t i = 0;
	COMMON_CMD_DATA* p_msg = (COMMON_CMD_DATA*)p_buffer;
	FANCONTROL_TYPE* p_data = (FANCONTROL_TYPE*)p_msg->data;
	COMMON_RETURN_DATA_TYPE* p_return = (COMMON_RETURN_DATA_TYPE*)malloc(sizeof(COMMON_RETURN_DATA_TYPE));
	RETURN_ERR_DATA_TYPE* p_err = (RETURN_ERR_DATA_TYPE*)malloc(sizeof(RETURN_ERR_DATA_TYPE));
	
	uint32_t frame_id = p_msg->frame_head;
	uint16_t cmd = p_data->cmd;
	p_return->cmd = cmd;
	p_err->err_code = ERR_CMD_FAIL;
	p_err->module_id = MODULE_FAN;
	p_err->device_id = p_data->fan_num;
	
	if(p_data->fan_num<=FAN_MAX)
	{
		if(p_data->fan_num<FAN_MAX)
		{
			if(p_data->on_off)
			{
				Drv_FanON(_gp_Fan[p_data->fan_num]);
			}
			else
			{
				Drv_FanOFF(_gp_Fan[p_data->fan_num]);
			}
		}
		else
		{
			if(p_data->on_off)
			{
				for(i=0;i<FAN_MAX;i++)
					Drv_FanON(_gp_Fan[i]);
			}
			else
			{
				for(i=0;i<FAN_MAX;i++)
					Drv_FanOFF(_gp_Fan[i]);
			}
		}
	}
	else
	{
		_Drv_UsartReturnFailToBuffer(frame_id,cmd,sizeof(RETURN_ERR_DATA_TYPE),(uint8_t*)p_err);
		free(p_err);
		free(p_return);
		return false;
	}
	_Drv_UsartReturnDoneToBuffer(frame_id,cmd,sizeof(COMMON_RETURN_DATA_TYPE),(uint8_t*)p_return);
	free(p_err);
	free(p_return);
	return true;

}

/* 制冷片控制指令
  ------------------------------
  返回值：true，成功；false，失败
*/
bool Func_Cmd_Com_Freezer_Control(void* p_buffer)
{
	uint8_t i = 0;
	COMMON_CMD_DATA* p_msg = (COMMON_CMD_DATA*)p_buffer;
	COLDING_TYPE* p_data = (COLDING_TYPE*)p_msg->data;
	COMMON_RETURN_DATA_TYPE* p_return = (COMMON_RETURN_DATA_TYPE*)malloc(sizeof(COMMON_RETURN_DATA_TYPE));
	RETURN_ERR_DATA_TYPE* p_err = (RETURN_ERR_DATA_TYPE*)malloc(sizeof(RETURN_ERR_DATA_TYPE));
	
	uint32_t frame_id = p_msg->frame_head;
	uint16_t cmd = p_data->cmd;
	p_return->cmd = cmd;
	p_err->err_code = ERR_CMD_FAIL;
	p_err->module_id = MODULE_COLDING;
	p_err->device_id = p_data->cold_num;
	
	if(p_data->cold_num<=COLD_MAX)
	{
		if(p_data->cold_num<COLD_MAX)
		{
			if(p_data->on_off)
			{
				Drv_RefrigeratingOpen(_gp_C[p_data->cold_num]);
				Drv_FanON(_gp_Fan[i]);
			}
			else
			{
				Drv_RefrigeratingClose(_gp_C[p_data->cold_num]);
//				Drv_FanOFF(_gp_Fan[i]);
			}
		}
		else
		{
			if(p_data->on_off)
			{
				for(i=0;i<COLD_MAX;i++)
				{	
					Drv_RefrigeratingOpen(_gp_C[i]);
					Drv_FanON(_gp_Fan[i]);
				}
			}
			else
			{
				for(i=0;i<COLD_MAX;i++)
				{	
					Drv_RefrigeratingClose(_gp_C[i]);
					Drv_FanOFF(_gp_Fan[i]);
				}
			}
		}
	}
	else
	{
		_Drv_UsartReturnFailToBuffer(frame_id,cmd,sizeof(RETURN_ERR_DATA_TYPE),(uint8_t*)p_err);
		free(p_err);
		free(p_return);
		return false;
	}
	_Drv_UsartReturnDoneToBuffer(frame_id,cmd,sizeof(COMMON_RETURN_DATA_TYPE),(uint8_t*)p_return);
	free(p_err);
	free(p_return);
	return true;

}

/* 加热膜控制指令
  ------------------------------
  返回值：true，成功；false，失败
*/
bool Func_Cmd_Com_Heater_Control(void* p_buffer)
{
	uint8_t i = 0;
	COMMON_CMD_DATA* p_msg = (COMMON_CMD_DATA*)p_buffer;
	HEATING_TYPE* p_data = (HEATING_TYPE*)p_msg->data;
	COMMON_RETURN_DATA_TYPE* p_return = (COMMON_RETURN_DATA_TYPE*)malloc(sizeof(COMMON_RETURN_DATA_TYPE));
	RETURN_ERR_DATA_TYPE* p_err = (RETURN_ERR_DATA_TYPE*)malloc(sizeof(RETURN_ERR_DATA_TYPE));
	
	uint32_t frame_id = p_msg->frame_head;
	uint16_t cmd = p_data->cmd;
	p_return->cmd = cmd;
	p_err->err_code = ERR_CMD_FAIL;
	p_err->module_id = MODULE_HEATING;
	p_err->device_id = p_data->heat_num;
	
	if(p_data->heat_num<=HEAT_MAX)
	{
		if(p_data->heat_num<HEAT_MAX)
		{
			if(p_data->on_off)
			{
				Drv_HeatOpen(_gp_H[p_data->heat_num]);
			}
			else
			{
				Drv_HeatClose(_gp_H[p_data->heat_num]);
			}
		}
		else
		{
			if(p_data->on_off)
			{
				for(i=0;i<COLD_MAX;i++)
					Drv_HeatOpen(_gp_H[i]);
			}
			else
			{
				for(i=0;i<COLD_MAX;i++)
					Drv_HeatClose(_gp_H[i]);
			}
		}
	}
	else
	{
		_Drv_UsartReturnFailToBuffer(frame_id,cmd,sizeof(RETURN_ERR_DATA_TYPE),(uint8_t*)p_err);
		free(p_err);
		free(p_return);
		return false;
	}
	_Drv_UsartReturnDoneToBuffer(frame_id,cmd,sizeof(COMMON_RETURN_DATA_TYPE),(uint8_t*)p_return);
	free(p_err);
	free(p_return);
	return true;
}

/* 读液位传感器指令
  ------------------------------
  返回值：true，成功；false，失败
*/
bool Func_Cmd_Com_Liquid_Read(void* p_buffer)
{
	COMMON_CMD_DATA* p_msg = (COMMON_CMD_DATA*)p_buffer;
	LIQUIDE_TYPE* p_data = (LIQUIDE_TYPE*)p_msg->data;
	LIQUIDE_RETURN_DATA_TYPE* p_return = (LIQUIDE_RETURN_DATA_TYPE*)malloc(sizeof(LIQUIDE_RETURN_DATA_TYPE));
	RETURN_ERR_DATA_TYPE* p_err = (RETURN_ERR_DATA_TYPE*)malloc(sizeof(RETURN_ERR_DATA_TYPE));
	
	uint32_t frame_id = p_msg->frame_head;
	uint16_t cmd = p_data->cmd;
	p_return->cmd = cmd;
	p_err->err_code = ERR_CMD_FAIL;
	p_err->module_id = MODULE_LIQUID;
	p_err->device_id = p_data->liquid_num&0xff;
	
	if(p_data->liquid_num<=LIQUIDE_MAX)
	{
		p_return->sensor_num = p_data->liquid_num;
		p_return->status = BSP_ReadPin(_gp_S[p_data->liquid_num]->port_number,_gp_S[p_data->liquid_num]->pin_number);
	}
	else
	{
		_Drv_UsartReturnFailToBuffer(frame_id,cmd,sizeof(RETURN_ERR_DATA_TYPE),(uint8_t*)p_err);
		free(p_err);
		free(p_return);
		return false;
	}
	_Drv_UsartReturnDoneToBuffer(frame_id,cmd,sizeof(LIQUIDE_RETURN_DATA_TYPE),(uint8_t*)p_return);
	free(p_err);
	free(p_return);
	return true;
}
/* 台面指示灯控制指令
  ------------------------------
  返回值：true，成功；false，失败
*/
bool Func_Cmd_Com_Led_Control(void* p_buffer)
{

	COMMON_CMD_DATA* p_msg = (COMMON_CMD_DATA*)p_buffer;
	LED_TYPE* p_data = (LED_TYPE*)p_msg->data;
	COMMON_RETURN_DATA_TYPE* p_return = (COMMON_RETURN_DATA_TYPE*)malloc(sizeof(COMMON_RETURN_DATA_TYPE));
	RETURN_ERR_DATA_TYPE* p_err = (RETURN_ERR_DATA_TYPE*)malloc(sizeof(RETURN_ERR_DATA_TYPE));
	
	uint32_t frame_id = p_msg->frame_head;
	uint16_t cmd = p_data->cmd;
	p_return->cmd = cmd;
	p_err->err_code = ERR_CMD_FAIL;
	p_err->module_id = MODULE_LED;
	p_err->device_id = p_data->status;
	
	if(p_data->status<LED_MAX)
	{
		switch(p_data->status)
		{
			case LED_RED:
			{
				Led_Off(_gp_LED[LED_GREEN]);
				Led_Off(_gp_LED[LED_YELLOW]);
				Led_On(_gp_LED[p_data->status]);
				break;
			}
			case LED_GREEN:
			{
				Led_Off(_gp_LED[LED_RED]);
				Led_Off(_gp_LED[LED_YELLOW]);
				Led_On(_gp_LED[p_data->status]);
				break;
			}
			case LED_YELLOW:
			{
				Led_Off(_gp_LED[LED_RED]);
				Led_Off(_gp_LED[LED_GREEN]);
				Led_On(_gp_LED[p_data->status]);
				break;
			}
			default:
			{
				_Drv_UsartReturnFailToBuffer(frame_id,cmd,sizeof(RETURN_ERR_DATA_TYPE),(uint8_t*)p_err);
				free(p_err);
				free(p_return);
				return false;
			}
		}	
	}
	else
	{
		_Drv_UsartReturnFailToBuffer(frame_id,cmd,sizeof(RETURN_ERR_DATA_TYPE),(uint8_t*)p_err);
		free(p_err);
		free(p_return);
		return false;
	}
	_Drv_UsartReturnDoneToBuffer(frame_id,cmd,sizeof(COMMON_RETURN_DATA_TYPE),(uint8_t*)p_return);
	free(p_err);
	free(p_return);
	return true;

}

/* 版本查询指令
  ------------------------------
  返回值：true，成功；false，失败
*/
bool Func_Cmd_Com_Inquiry_Version(void* p_buffer)
{

	COMMON_CMD_DATA* p_msg = (COMMON_CMD_DATA*)p_buffer;
	LED_TYPE* p_data = (LED_TYPE*)p_msg->data;
	VERSION_RETURN* p_return = (VERSION_RETURN*)malloc(sizeof(VERSION_RETURN));
	uint32_t frame_id;
	uint16_t cmd;
	if(p_return == NULL)
	{	
		free(p_return);
		return false;
	}
	frame_id = p_msg->frame_head;
	cmd = p_data->cmd;
	p_return->cmd = cmd;
	memcpy(p_return->date,__DATE__" "__TIME__,sizeof(p_return->date));
	memcpy(p_return->version, FirmwareVersion,sizeof(p_return->version));
	
	_Drv_UsartReturnDoneToBuffer(frame_id,cmd,sizeof(VERSION_RETURN),(uint8_t*)p_return);
	free(p_return);
	return true;
}

