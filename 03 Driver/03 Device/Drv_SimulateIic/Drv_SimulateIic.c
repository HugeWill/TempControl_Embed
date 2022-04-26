/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

			嵌入式开发平台——软件模拟IIC通信模块

  -------------------------------------------------------
   文 件 名   : Drv_SimulateIic.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年10月25日
   功能描述   : 定义一种软件模拟IIC通信模块。
   使用方法   ：上层模块通过调用IicInit获得一个IIC通信接口
				的指针，通过调用IicSend发送，通过调用IicGet
				接收。
   依赖于     : BSP_Gpio V1.0.0.0
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#include "Drv_SimulateIic.h"

DRV_SIMULATE_IIC_TYPE* _gp_SimulateIicUnits[SIMULATE_IIC_BUTT] = {0};	/*软件模拟IIC接口的注册表*/

/* 接口函数
 ********************************************************/

/* 初始化软件模拟IIC接口模块
  -----------------------------
  入口：软件模拟IIC接口模块的ID号，IO端口
  返回值：初始化好的软件模拟IIC接口模块结构体
*/
DRV_SIMULATE_IIC_TYPE* SimulateIicInit(DRV_SIMULATE_IIC_ENUM id, \
												uint32_t theta, \
												BSP_PORT_ENUM sda_port_number, BSP_PIN_ENUM sda_pin_number, \
												BSP_PORT_ENUM scl_port_number, BSP_PIN_ENUM scl_pin_number)
{	
	DRV_SIMULATE_IIC_TYPE* p_unit = (DRV_SIMULATE_IIC_TYPE*)calloc(1, sizeof(DRV_SIMULATE_IIC_TYPE));	/*申请内存*/

	/*分配失败返回NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id错误返回NULL*/
	if(id >= SIMULATE_IIC_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*如果当前模块存在，取消分配新存储区*/
	if(_gp_SimulateIicUnits[id] != NULL)
	{
		free(p_unit);
		p_unit = _gp_SimulateIicUnits[id];
	}
	else
	{
		_gp_SimulateIicUnits[id] = p_unit;
	}
	
	/*初始化*/
	
	/*-------------------------------常量*/
	p_unit->id          = id;						/*模块id*/
	p_unit->theta   	= theta;					/*时钟速度参数*/
	p_unit->sda_port_number = sda_port_number;		/*引脚端口号*/
	p_unit->sda_pin_number  = sda_pin_number;		/*引脚序号*/
	BSP_Init_Pin(sda_port_number, sda_pin_number, _OUT_OD_);	/*初始化硬件*/
	p_unit->scl_port_number = scl_port_number;		/*引脚端口号*/
	p_unit->scl_pin_number  = scl_pin_number;		/*引脚序号*/
	BSP_Init_Pin(scl_port_number, scl_pin_number, _OUT_OD_);	/*初始化硬件*/
	
	/*-------------------------------变量*/
	p_unit->status = SIMULATE_IIC_READY;
	p_unit->sda_pin_set_buffer = 0;
	p_unit->sda_pin_read_buffer = 0;
	p_unit->scl_pin_buffer = 0;
	
	return p_unit;
}

/*
  发送一个字节
  --------------------------------
*/
bool Drv_SimulateIicSendByte(DRV_SIMULATE_IIC_TYPE* p_unit, unsigned char txd)
{                        
    unsigned char t;  
	
    _Drv_SimulateIicSetScl(p_unit, LOW);
    for(t = 0; t < 8; t++)
    {     
		if((txd&0x80) >> 7)
		{
			_Drv_SimulateIicSetSda(p_unit, HIGH);
		}
		else
		{
			_Drv_SimulateIicSetSda(p_unit, LOW);
		}
        
		txd <<= 1; 
		
		Drv_SimulateIicDelayUs(p_unit, 5);   
        
		_Drv_SimulateIicSetScl(p_unit, HIGH);
        
		Drv_SimulateIicDelayUs(p_unit, 5);
        
		_Drv_SimulateIicSetScl(p_unit, LOW);	
    }	 
	
	return true;
} 

/*
  读取一个字节
  --------------------------------
*/
unsigned char Drv_SimulateIicReadByte(DRV_SIMULATE_IIC_TYPE* p_unit)
{
	unsigned char i, receive = 0;
    
    for(i = 0; i < 8; i++ )
	{
		_Drv_SimulateIicSetScl(p_unit, LOW);
		
        Drv_SimulateIicDelayUs(p_unit, 5);
        
		_Drv_SimulateIicSetScl(p_unit, HIGH);
        
        receive = (receive << 1) | _Drv_SimulateIicReadSda(p_unit);
        
		Drv_SimulateIicDelayUs(p_unit, 5); 
    }
    
	return receive;
}
/*
  发送起始信号
  --------------------------------
*/
bool Drv_SimulateIicStart(DRV_SIMULATE_IIC_TYPE* p_unit)
{	
	_Drv_SimulateIicSetSda(p_unit, HIGH);
	_Drv_SimulateIicSetScl(p_unit, HIGH);
    
	Drv_SimulateIicDelayUs(p_unit, 5);
    
 	_Drv_SimulateIicSetSda(p_unit, LOW);
	
	Drv_SimulateIicDelayUs(p_unit, 5);
    
	_Drv_SimulateIicSetScl(p_unit, LOW);
			
	return true;
}
 
/*
  发送停止信号
  --------------------------------
*/
bool Drv_SimulateIicStop(DRV_SIMULATE_IIC_TYPE* p_unit)
{
	_Drv_SimulateIicSetScl(p_unit, LOW);	
	_Drv_SimulateIicSetSda(p_unit, LOW);
    
 	Drv_SimulateIicDelayUs(p_unit, 5);
	
	_Drv_SimulateIicSetScl(p_unit, HIGH);
	
 	Drv_SimulateIicDelayUs(p_unit, 5);
	
	_Drv_SimulateIicSetSda(p_unit, HIGH);
	
	Drv_SimulateIicDelayUs(p_unit, 50000);		
	
	return true;			   	
}

/*
  等待ACK信号
  --------------------------------
*/
bool Drv_SimulateIicWaitAck(DRV_SIMULATE_IIC_TYPE* p_unit)
{
	unsigned short uc_err_time = 0; 
	
	_Drv_SimulateIicSetSda(p_unit, HIGH);
	
    Drv_SimulateIicDelayUs(p_unit, 5);
    
	_Drv_SimulateIicSetScl(p_unit, HIGH);
	
    Drv_SimulateIicDelayUs(p_unit, 5);
    
	while(_Drv_SimulateIicReadSda(p_unit))
	{
		uc_err_time++;
		if(uc_err_time > UCERRTIME)
		{
			return false;
		}
	}
	
	_Drv_SimulateIicSetScl(p_unit, LOW);
    
	return true;  
}

/*
  发送ACK信号
  --------------------------------
*/
bool Drv_SimulateIicAck(DRV_SIMULATE_IIC_TYPE* p_unit)
{
	_Drv_SimulateIicSetScl(p_unit, LOW);    
	_Drv_SimulateIicSetSda(p_unit, LOW);
    
	Drv_SimulateIicDelayUs(p_unit, 5);
    
	_Drv_SimulateIicSetScl(p_unit, HIGH);
    
	Drv_SimulateIicDelayUs(p_unit, 5);
    
	_Drv_SimulateIicSetScl(p_unit, LOW);
    
	Drv_SimulateIicDelayUs(p_unit, 5);
	
	_Drv_SimulateIicSetSda(p_unit, HIGH);
    
	Drv_SimulateIicDelayUs(p_unit, 5);
	
	return true;
}

/*
  发送NACK信号
  --------------------------------
*/
bool Drv_SimulateIicNack(DRV_SIMULATE_IIC_TYPE* p_unit)
{
	_Drv_SimulateIicSetScl(p_unit, LOW);    
	_Drv_SimulateIicSetSda(p_unit, LOW);
	
	Drv_SimulateIicDelayUs(p_unit, 5);
	
	_Drv_SimulateIicSetSda(p_unit, HIGH);
	
	Drv_SimulateIicDelayUs(p_unit, 5);
    
	_Drv_SimulateIicSetScl(p_unit, HIGH);
	
	Drv_SimulateIicDelayUs(p_unit, 5);
	
	_Drv_SimulateIicSetScl(p_unit, LOW);
    
	Drv_SimulateIicDelayUs(p_unit, 5);
	
	_Drv_SimulateIicSetSda(p_unit, LOW);
	
	Drv_SimulateIicDelayUs(p_unit, 5);
	
	return true;
}

/* 内部函数
 ********************************************************/
  
/*
  延时n个us
  --------------------------------
*/
bool Drv_SimulateIicDelayUs(DRV_SIMULATE_IIC_TYPE* p_unit, unsigned short time)
{    
   unsigned short i = 0; 
   
   while(time--)
   {
      i = p_unit->theta;
      while(i--);    
   }
	
	return true;
}
 

/*
  SCL状态设置
  --------------------------------
  入口：IIC模块结构体，SCL引脚状态
  返回：true 执行成功，false 执行失败
*/
static bool _Drv_SimulateIicSetScl(DRV_SIMULATE_IIC_TYPE* p_unit, uint8_t state)
{
	p_unit->scl_pin_buffer = state;
	BSP_WritePin(p_unit->scl_port_number, p_unit->scl_pin_number, p_unit->scl_pin_buffer);
	
	return true;
}

/*
  SDA状态设置
  --------------------------------
  入口：IIC模块结构体，SDA引脚状态
  返回：true 执行成功，false 执行失败
*/
static bool _Drv_SimulateIicSetSda(DRV_SIMULATE_IIC_TYPE* p_unit, uint8_t state)
{
	p_unit->sda_pin_set_buffer = state;
	BSP_WritePin(p_unit->sda_port_number, p_unit->sda_pin_number, p_unit->sda_pin_set_buffer);
	
	return true;
}

/*
  SDA状态读取
  --------------------------------
  入口：IIC模块结构体
  返回：SDA引脚状态, HIGH或者LOW
*/
static uint8_t _Drv_SimulateIicReadSda(DRV_SIMULATE_IIC_TYPE* p_unit)
{
	return p_unit->sda_pin_read_buffer = BSP_ReadPin(p_unit->sda_port_number, p_unit->sda_pin_number);
}
