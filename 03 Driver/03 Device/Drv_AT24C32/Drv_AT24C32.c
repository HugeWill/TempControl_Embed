/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

			嵌入式开发平台——AT24C32 EEP模块

  -------------------------------------------------------
   文 件 名   : Drv_AT24C32.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2020年03月02日
   功能描述   : 定义AT24C32 EEP的驱动。
   使用方法   ：上层模块通过调用At24c32Init获得一个AT24C32
				的指针，通过调用At24c32Read读取数据，通过调用
				At24c32Write写入数据。
   依赖于     : 
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#include "Drv_AT24C32.h"

DRV_AT24C32_TYPE* _gp_At24c32Units[AT24C32_BUTT] = {0};	/*AT24C32 EEP模块的注册表*/

/* 接口函数
 ********************************************************/

/* 初始化AT24C32模块
  -----------------------------
  入口：AT24C32模块的ID号，引脚端口
  返回值：初始化好的AT24C32模块结构体
*/
DRV_AT24C32_TYPE* At24c32Init(DRV_AT24C32_ENUM id, \
								BSP_PORT_ENUM scl_port_number, BSP_PIN_ENUM scl_pin_number, \
								BSP_PORT_ENUM sda_port_number, BSP_PIN_ENUM sda_pin_number)
{	
	DRV_AT24C32_TYPE* p_unit = (DRV_AT24C32_TYPE*)calloc(1, sizeof(DRV_AT24C32_TYPE));	/*申请内存*/

	/*分配失败返回NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id错误返回NULL*/
	if(id >= AT24C32_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*如果当前模块存在，取消分配新存储区*/
	if(_gp_At24c32Units[id] != NULL)
	{
		free(p_unit);
		p_unit = _gp_At24c32Units[id];
	}
	else
	{
		_gp_At24c32Units[id] = p_unit;
	}
	
	/*初始化*/
	
	/*-------------------------------常量*/
	p_unit->id             = id;				/*模块id*/
	p_unit->iic_port = SimulateIicInit(SIMULATE_IIC_1, 4, \
										sda_port_number, sda_pin_number, \
										scl_port_number, scl_pin_number);	/*引脚端口号*/
	
	/*-------------------------------变量*/
	p_unit->status = AT24C32_READY;
	p_unit->read_buffer = 0;
	p_unit->write_buffer = 0;
		
	return p_unit;
}

/* AT24C32模块读一个字节
  -----------------------------
  入口：AT24C32模块的ID号，地址
  返回值：数据
*/
uint8_t At24c32ReadByte(DRV_AT24C32_TYPE* p_unit, uint16_t address)
{
	uint8_t temp8 = 0;
	
	Drv_SimulateIicStart(p_unit->iic_port);						/*发送start信号*/
	
	Drv_SimulateIicSendByte(p_unit->iic_port, 0xA0);			/*发送写命令0xA0*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	Drv_SimulateIicSendByte(p_unit->iic_port, address >> 8);	/*发送读取地址高位*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	Drv_SimulateIicSendByte(p_unit->iic_port, (uint8_t)address);	/*发送读取地址低位*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	Drv_SimulateIicStart(p_unit->iic_port);						/*发送start信号*/
	
	Drv_SimulateIicSendByte(p_unit->iic_port, 0xA1);			/*发送读命令0xA1*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	temp8 = Drv_SimulateIicReadByte(p_unit->iic_port);			/*读取一个字节*/
	Drv_SimulateIicNack(p_unit->iic_port);						/*发送NACK*/
	
	Drv_SimulateIicStop(p_unit->iic_port);						/*发送stop信号*/
	
	return temp8;
}

/* AT24C32模块读一个双字
  -----------------------------
  入口：AT24C32模块的ID号，地址
  返回值：数据
*/
uint16_t At24c32ReadTwoByte(DRV_AT24C32_TYPE* p_unit, uint16_t address)
{
	uint16_t temp16 = 0;
	
	Drv_SimulateIicStart(p_unit->iic_port);						/*发送start信号*/
	
	Drv_SimulateIicSendByte(p_unit->iic_port, 0xA0);			/*发送写命令0xA0*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	Drv_SimulateIicSendByte(p_unit->iic_port, address >> 8);	/*发送读取地址高位*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	Drv_SimulateIicSendByte(p_unit->iic_port, (uint8_t)address);	/*发送读取地址低位*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	Drv_SimulateIicStart(p_unit->iic_port);						/*发送start信号*/
	
	Drv_SimulateIicSendByte(p_unit->iic_port, 0xA1);			/*发送读命令0xA0*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	temp16 = Drv_SimulateIicReadByte(p_unit->iic_port);			/*读取一个字节*/
	Drv_SimulateIicAck(p_unit->iic_port);						/*发送ACK*/
	
	temp16 <<= 8;
	temp16 |= Drv_SimulateIicReadByte(p_unit->iic_port);		/*读取一个字节*/
	Drv_SimulateIicNack(p_unit->iic_port);						/*发送NACK*/
	
	Drv_SimulateIicStop(p_unit->iic_port);						/*发送stop信号*/
	
	return temp16;
}

/* AT24C32模块写一个字节
  -----------------------------
  入口：AT24C32模块的ID号，地址，数据
  返回值：执行状态，成功true，失败false
*/
bool At24c32WriteByte(DRV_AT24C32_TYPE* p_unit, uint16_t address, uint8_t data)
{	
	Drv_SimulateIicStart(p_unit->iic_port);						/*发送start信号*/
	
	Drv_SimulateIicSendByte(p_unit->iic_port, 0xA0);			/*发送写命令0xA0*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	Drv_SimulateIicSendByte(p_unit->iic_port, address >> 8);	/*发送读取地址高位*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	Drv_SimulateIicSendByte(p_unit->iic_port, (uint8_t)address);	/*发送读取地址低位*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	Drv_SimulateIicSendByte(p_unit->iic_port, data);			/*发送data*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	Drv_SimulateIicStop(p_unit->iic_port);						/*发送stop信号*/
			
	/*校验*/
	if(At24c32ReadByte(p_unit, address) != data)
	{
		return false;
	}
	else
	{
		return true;
	}
}

/* AT24C32模块写两个字节
  -----------------------------
  入口：AT24C32模块的ID号，地址，数据
  返回值：执行状态，成功true，失败false
*/
bool At24c32WriteTwoByte(DRV_AT24C32_TYPE* p_unit, uint16_t address, uint16_t data)
{
	Drv_SimulateIicStart(p_unit->iic_port);						/*发送start信号*/
	
	Drv_SimulateIicSendByte(p_unit->iic_port, 0xA0);			/*发送写命令0xA0*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	Drv_SimulateIicSendByte(p_unit->iic_port, address >> 8);	/*发送读取地址高位*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	Drv_SimulateIicSendByte(p_unit->iic_port, (uint8_t)address);	/*发送读取地址低位*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	Drv_SimulateIicSendByte(p_unit->iic_port, data >> 8);		/*发送data高位*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	Drv_SimulateIicSendByte(p_unit->iic_port, (uint8_t)data);	/*发送data*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	Drv_SimulateIicStop(p_unit->iic_port);						/*发送stop信号*/
		
	/*校验*/
	if(At24c32ReadTwoByte(p_unit, address) != data)
		return false;
	else
		return true;
}

/* AT24C32模块写一页（32Byte）
  -----------------------------
  入口：AT24C32模块的ID号，页地址，数据指针
  返回值：执行状态，成功true，失败false
*/
bool At24c32WritePage(DRV_AT24C32_TYPE* p_unit, uint16_t page_address, uint8_t* datas)
{
	uint8_t i = 0;
	
	Drv_SimulateIicStart(p_unit->iic_port);						/*发送start信号*/
	
	Drv_SimulateIicSendByte(p_unit->iic_port, 0xA0);			/*发送写命令0xA0*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	Drv_SimulateIicSendByte(p_unit->iic_port, (page_address * 32) >> 8);		/*发送读取地址高位*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	Drv_SimulateIicSendByte(p_unit->iic_port, (uint8_t)(page_address * 32));	/*发送读取地址低位*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	for(i = 0; i < 32; i++)
	{
		Drv_SimulateIicSendByte(p_unit->iic_port, datas[i]);	/*发送datas*/
		if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)	/*等待ACK信号*/
		{
			Drv_SimulateIicStop(p_unit->iic_port);				/*发送stop信号*/
			return false;
		}
	}
	
	Drv_SimulateIicStop(p_unit->iic_port);						/*发送stop信号*/
		
	/*校验*/
	for(i = 0; i < 32; i++)
	{
		if(At24c32ReadByte(p_unit, (page_address * 32) + i) != datas[i])
			return false;
	}
	
	return true;
}


/* AT24C32模块读一页（32Byte）
  -----------------------------
  入口：AT24C32模块的ID号，页地址，数据指针
  返回值：执行状态，成功true，失败false
*/
bool At24c32ReadPage(DRV_AT24C32_TYPE* p_unit, uint16_t page_address, uint8_t* datas)
{
	uint8_t i = 0;
	
	Drv_SimulateIicStart(p_unit->iic_port);						/*发送start信号*/
	
	Drv_SimulateIicSendByte(p_unit->iic_port, 0xA0);			/*发送写命令0xA0*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	Drv_SimulateIicSendByte(p_unit->iic_port, (page_address * 32) >> 8);		/*发送读取地址高位*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	Drv_SimulateIicSendByte(p_unit->iic_port, (uint8_t)(page_address * 32));	/*发送读取地址低位*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	Drv_SimulateIicStart(p_unit->iic_port);						/*发送start信号*/
	
	Drv_SimulateIicSendByte(p_unit->iic_port, 0xA1);			/*发送读命令0xA1*/
	if(Drv_SimulateIicWaitAck(p_unit->iic_port) == false)		/*等待ACK信号*/
	{
		Drv_SimulateIicStop(p_unit->iic_port);					/*发送stop信号*/
		return false;
	}
	
	for(i = 0; i < (32 - 1); i++)
	{
		datas[i] = Drv_SimulateIicReadByte(p_unit->iic_port);	/*读取datas*/
		Drv_SimulateIicAck(p_unit->iic_port);					/*发送ACK*/
	}
	
	datas[i] = Drv_SimulateIicReadByte(p_unit->iic_port);		/*读取datas*/
	Drv_SimulateIicNack(p_unit->iic_port);						/*发送NACK*/
	
	Drv_SimulateIicStop(p_unit->iic_port);						/*发送stop信号*/
	
	return true;
}
