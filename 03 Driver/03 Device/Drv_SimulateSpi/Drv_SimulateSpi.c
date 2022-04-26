/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

			嵌入式开发平台——软件模拟SPI通信模块

  -------------------------------------------------------
   文 件 名   : Drv_SimulateSpi.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年10月14日
   功能描述   : 定义一种软件模拟SPI通信模块。
   使用方法   ：上层模块通过调用SPI_Init获得一个SPI通信接口
				的指针，通过调用Spi_Send发送，通过调用Spi_Get
				接收。
   依赖于     : BSP_Gpio V1.0.0.0
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#include "Drv_SimulateSpi.h"

DRV_SOFTWARE_SPI_TYPE* _gp_SoftwareSpiUnits[SOFTWARE_SPI_BUTT] = {0};	/*软件模拟SPI接口的注册表*/

/* 接口函数
 ********************************************************/

/* 初始化软件模拟SPI接口模块
  -----------------------------
  入口：软件模拟SPI接口模块的ID号，IO端口
  返回值：初始化好的软件模拟SPI接口模块结构体
*/
DRV_SOFTWARE_SPI_TYPE* SoftwareSpiInit(DRV_SOFTWARE_SPI_ENUM id, \
										uint8_t theta, \
										BSP_PORT_ENUM sdi_port_number, BSP_PIN_ENUM sdi_pin_number, \
										BSP_PORT_ENUM sdo_port_number, BSP_PIN_ENUM sdo_pin_number, \
										BSP_PORT_ENUM sck_port_number, BSP_PIN_ENUM sck_pin_number, \
										BSP_PORT_ENUM cs_port_number, BSP_PIN_ENUM cs_pin_number)
{	
	DRV_SOFTWARE_SPI_TYPE* p_unit = (DRV_SOFTWARE_SPI_TYPE*)calloc(1, sizeof(DRV_SOFTWARE_SPI_TYPE));	/*申请内存*/

	/*分配失败返回NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id错误返回NULL*/
	if(id >= SOFTWARE_SPI_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*如果当前模块存在，取消分配新存储区*/
	if(_gp_SoftwareSpiUnits[id] != NULL)
	{
		free(p_unit);
		p_unit = _gp_SoftwareSpiUnits[id];
	}
	else
	{
		_gp_SoftwareSpiUnits[id] = p_unit;
	}
	
	/*初始化*/
	
	/*-------------------------------常量*/
	p_unit->id          = id;						/*模块id*/
	p_unit->theta   	= theta;					/*时钟速度参数*/
	p_unit->sdi_port_number = sdi_port_number;		/*引脚端口号*/
	p_unit->sdi_pin_number  = sdi_pin_number;		/*引脚序号*/
	BSP_Init_Pin(sdi_port_number, sdi_pin_number, _IN_FLOATING_);	/*初始化硬件*/
	p_unit->sdo_port_number = sdo_port_number;		/*引脚端口号*/
	p_unit->sdo_pin_number  = sdo_pin_number;		/*引脚序号*/
	BSP_Init_Pin(sdo_port_number, sdo_pin_number, _OUT_PP_);	/*初始化硬件*/
	p_unit->sck_port_number = sck_port_number;		/*引脚端口号*/
	p_unit->sck_pin_number  = sck_pin_number;		/*引脚序号*/
	BSP_Init_Pin(sck_port_number, sck_pin_number, _OUT_PP_);	/*初始化硬件*/
	p_unit->cs_port_number = cs_port_number;		/*引脚端口号*/
	p_unit->cs_pin_number  = cs_pin_number;			/*引脚序号*/
	BSP_Init_Pin(cs_port_number, cs_pin_number, _OUT_PP_);	/*初始化硬件*/
	
	/*-------------------------------变量*/
	p_unit->status = SOFTWARE_SPI_READY;
	p_unit->sdi_pin_buffer = 0;
	p_unit->sdo_pin_buffer = 0;
	p_unit->sck_pin_buffer = 0;
	p_unit->cs_pin_buffer = 0;
	
	return p_unit;
}

/* 软件模拟SPI接口模块写一个字节
  -----------------------------
  入口：软件模拟SPI接口模块的ID号，数据
  返回值：true 成功，false 失败
*/
bool SoftwareSpiWriteByte(DRV_SOFTWARE_SPI_TYPE* p_unit, uint8_t data)
{
	uint8_t i;
	
	p_unit->status = SOFTWARE_SPI_BUSY;
	if(p_unit->cs_port_number != _PORT_BUTT_)	/*有片选才控制*/
	{
		p_unit->cs_pin_buffer = 0;
		BSP_WritePin(p_unit->cs_port_number, p_unit->cs_pin_number, p_unit->cs_pin_buffer);
		_SoftwareSpiDelayUs(p_unit, 1);		/*延时1us*/
	}
	for(i = 0; i < 8; i++)				/*依次发送8位数据*/
	{
		p_unit->sck_pin_buffer = 1;
		BSP_WritePin(p_unit->sck_port_number, p_unit->sck_pin_number, p_unit->sck_pin_buffer);
		_SoftwareSpiDelayUs(p_unit, 1);	/*延时1us*/
		if(data & 0x80)					/*取最高位输出*/
		{
			p_unit->sdo_pin_buffer = 1;
			BSP_WritePin(p_unit->sdo_port_number, p_unit->sdo_pin_number, p_unit->sdo_pin_buffer);
		} 
		else
		{
			p_unit->sdo_pin_buffer = 0;
			BSP_WritePin(p_unit->sdo_port_number, p_unit->sdo_pin_number, p_unit->sdo_pin_buffer);
		}
		_SoftwareSpiDelayUs(p_unit, 1);	/*延时1us*/
		p_unit->sck_pin_buffer = 0;
		BSP_WritePin(p_unit->sck_port_number, p_unit->sck_pin_number, p_unit->sck_pin_buffer);
		data <<= 1; 					/*数据左移*/
		_SoftwareSpiDelayUs(p_unit, 1);	/*延时1us*/
	}
	if(p_unit->cs_port_number != _PORT_BUTT_)	/*有片选才控制*/
	{
		p_unit->cs_pin_buffer = 1;
		BSP_WritePin(p_unit->cs_port_number, p_unit->cs_pin_number, p_unit->cs_pin_buffer);
	}
	p_unit->status = SOFTWARE_SPI_READY;
	
	return true;
}

/* 软件模拟SPI接口模块读一个字节
  -----------------------------
  入口：软件模拟SPI接口模块的ID号
  返回值：数据
*/
uint8_t SoftwareSpiReadByte(DRV_SOFTWARE_SPI_TYPE* p_unit)
{
	uint8_t data = 0, i = 0;
	  	
	p_unit->status = SOFTWARE_SPI_BUSY;
	if(p_unit->cs_port_number != _PORT_BUTT_)	/*有片选才控制*/
	{
		p_unit->cs_pin_buffer = 0;
		BSP_WritePin(p_unit->cs_port_number, p_unit->cs_pin_number, p_unit->cs_pin_buffer);
		_SoftwareSpiDelayUs(p_unit, 1);		/*延时1us*/
	}
	for(i = 0; i < 8; i++)				/*依次读取8位数据*/
	{
		p_unit->sck_pin_buffer = 1;
		BSP_WritePin(p_unit->sck_port_number, p_unit->sck_pin_number, p_unit->sck_pin_buffer);
		_SoftwareSpiDelayUs(p_unit, 1);	/*延时1us*/
		if(BSP_ReadPin(p_unit->sdi_port_number, p_unit->sdi_pin_number) == 1)
		{
			data += 1;
		}
		_SoftwareSpiDelayUs(p_unit, 1);	/*延时1us*/
		p_unit->sck_pin_buffer = 0;
		BSP_WritePin(p_unit->sck_port_number, p_unit->sck_pin_number, p_unit->sck_pin_buffer);
		if(i < 7)
		{
			data <<= 1; 				/*数据左移*/
		}
		_SoftwareSpiDelayUs(p_unit, 1);	/*延时1us*/
	}
	if(p_unit->cs_port_number != _PORT_BUTT_)	/*有片选才控制*/
	{
		p_unit->cs_pin_buffer = 1;
		BSP_WritePin(p_unit->cs_port_number, p_unit->cs_pin_number, p_unit->cs_pin_buffer);
	}
	p_unit->status = SOFTWARE_SPI_READY;
	
	return data;
}

/* 软件模拟SPI接口模块延时n微秒
  -----------------------------
  入口：延时时间
  返回值：true 成功，false 失败
*/
bool _SoftwareSpiDelayUs(DRV_SOFTWARE_SPI_TYPE* p_unit, uint8_t time)
{    
   unsigned short i = 0; 
   
   while(time--)
   {
      i = p_unit->theta;
      while(i--);    
   }
   
   return true;
}
 