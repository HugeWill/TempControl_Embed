/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——CR100M2扫码模块

  -------------------------------------------------------
   文 件 名   : Drv_CR100M2.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2020年01月15日
   功能描述   : 定义CR100M2扫码模块的控制。
   使用方法   ：上层模块通过调用CR100M2_Init获得一个CR100M2
				模块的指针，通过调用CR100M2_Scan进行扫描，
				通过调用CR100M2_Get获取收到的所有数据。
   依赖于     : BSP.h
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#include "Drv_CR100M2.h"

DRV_CR100M2_TYPE* _gp_CR100M2[CR100M2_BUTT] = {0};	/*扫码模块的注册表*/

/* 接口函数
 ********************************************************/

/* 初始化扫码模块
  -----------------------------
  入口：扫码模块的ID号，波特率，TX、RX端口
  返回值：初始化好的扫码模块结构体
*/
DRV_CR100M2_TYPE* Drv_CR100M2_Init(DRV_CR100M2_ENUM id, \
								BSP_USART_ENUM usart_number, uint32_t baud_rate, \
								BSP_PORT_ENUM tx_port_number, BSP_PIN_ENUM tx_pin_number, \
								BSP_PORT_ENUM rx_port_number, BSP_PIN_ENUM rx_pin_number)
{	
	uint8_t i = 0;
	DRV_CR100M2_TYPE* p_unit = (DRV_CR100M2_TYPE*)calloc(1, sizeof(DRV_CR100M2_TYPE));	/*申请内存*/
	
	/*分配失败返回NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id错误返回NULL*/
	if(id >= CR100M2_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*如果当前模块存在，取消分配新存储区*/
	if(_gp_CR100M2[id] != NULL)
	{
		free(p_unit);
		p_unit = _gp_CR100M2[id];
	}
	else
	{
		_gp_CR100M2[id] = p_unit;
	}
	
	/*初始化*/
	
	/*------------------------------常量*/
	p_unit->id = id;							/*串口模块编号*/
	p_unit->usart_number = usart_number;		/*串口模块编号*/
	p_unit->baud_rate = baud_rate;				/*波特率*/
	p_unit->tx_port_number = tx_port_number; 	/*TX引脚端口号*/
	p_unit->tx_pin_number = tx_pin_number;		/*TX引脚编号*/
	p_unit->rx_port_number = rx_port_number; 	/*RX引脚端口号*/
	p_unit->rx_pin_number = rx_pin_number;		/*RX引脚编号*/
	BSP_Init_UsartInterrupt(p_unit->usart_number, p_unit->baud_rate, \
							 p_unit->tx_port_number, p_unit->tx_pin_number, \
							 p_unit->rx_port_number, p_unit->rx_pin_number);
	
	p_unit->scan_sem = OSSemCreate(0);				/*扫码完成信号*/
	
	/*------------------------------变常*/
	
	/*------------------------------变量*/
	p_unit->status = CR100M2_READY;				/*该扫码模块的状态*/
	for(i = 0; i < USART_RX_MAX_NUMBER; i++)
	{
		p_unit->rx_buffer[i] = 0;	/*接收缓存*/
	}
	p_unit->rx_push = 0;
	
	return p_unit;
}



/* 扫码头扫描一次
  -----------------------------
  入口：扫码模块结构体
  返回值：true 扫描成功，false 扫码失败
*/
bool Drv_CR100M2_Scan(DRV_CR100M2_TYPE* p_unit)
{
	uint8_t  err = OS_ERR_NONE;
	
	/*发送单次扫描命令*/
	BSP_UsartSendByte(p_unit->usart_number, 0x02);
	BSP_UsartSendByte(p_unit->usart_number, 0x2b);
	BSP_UsartSendByte(p_unit->usart_number, 0x0d);
	BSP_UsartSendByte(p_unit->usart_number, 0x0a);
	p_unit->status = CR100M2_BUSY;	/*进入扫码状态*/
	
	/*等待扫码完成*/
	OSSemPend(p_unit->scan_sem, 500, &err);
	if(err != OS_ERR_NONE)	/*执行失败*/
	{
		/*发送停止单次扫描命令*/
		BSP_UsartSendByte(p_unit->usart_number, 0x02);
		BSP_UsartSendByte(p_unit->usart_number, 0x2d);
		BSP_UsartSendByte(p_unit->usart_number, 0x0d);
		BSP_UsartSendByte(p_unit->usart_number, 0x0a);
		p_unit->status = CR100M2_READY;	/*退出扫码状态*/
		return false;
	}
	
	return true;
}

/* 获取扫码数据
  -----------------------------
  入口：扫码模块结构体，数据缓存区地址，数据缓存区大小
  返回值：true 获取成功，false 获取失败
*/
bool Drv_CR100M2_Get(DRV_CR100M2_TYPE* p_unit, uint8_t* datas, uint16_t datas_number)
{
	uint8_t i = 0;
	if((p_unit->status == CR100M2_READY) && (p_unit->rx_push != 0))	/*缓冲区有数据*/
	{
		if(datas_number < p_unit->rx_push)	/*缓存大小不足*/
			return false;
		while(i < p_unit->rx_push)
		{
			datas[i] = p_unit->rx_buffer[i];	/*拷贝*/
			i++;
		}
		p_unit->rx_push = 0;	/*接收缓存清空*/
		return true;
	}
	return false;
}

/* 串口4中断处理
  -------------------------------
  将收到的数据放入缓存
*/
void BSP_Uart4_IRQHandler(void)
{
	if(_gp_CR100M2[0]->status == CR100M2_BUSY)	/*在扫码才读数据*/
	{
		_gp_CR100M2[0]->rx_buffer[_gp_CR100M2[0]->rx_push] = BSP_UsartGetByte(_gp_CR100M2[0]->usart_number);
		_gp_CR100M2[0]->rx_push = (_gp_CR100M2[0]->rx_push + 1) % USART_RX_MAX_NUMBER;
		if(_gp_CR100M2[0]->rx_buffer[0] != 0x02)	/*如果没收到包头，就清零*/
		{
			_gp_CR100M2[0]->rx_push = 0;
			return;
		}
		if((_gp_CR100M2[0]->rx_buffer[_gp_CR100M2[0]->rx_push - 1] == 0x0A) \
		&& (_gp_CR100M2[0]->rx_buffer[_gp_CR100M2[0]->rx_push - 2] == 0x0D))	/*收到包尾 0D 0A*/
		{
			_gp_CR100M2[0]->status = CR100M2_READY;	/*退出扫码状态*/
			OSSemPost(_gp_CR100M2[0]->scan_sem);
			return;
		}
	}
}
