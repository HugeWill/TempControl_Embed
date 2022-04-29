/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

               嵌入式开发平台——串口数据接口模块

  -------------------------------------------------------
   文 件 名   : Drv_UartDataInterface.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2020年05月13日
   功能描述   : 
   使用方法   ：
				
   依赖于     : BSP.h
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/
 
#include "Drv_UartDataInterface.h"

DRV_UARTDATAINTERFACE_TYPE* _gp_usart1 = NULL;
uint8_t Usart_Rx_Buffer[MAX_CMD_BUFFER][USART_RX_MAX_NUMBER];		//串口接收数据缓存，最多缓存五条指令
uint8_t Usart_Tx_Buffer[MAX_CMD_BUFFER][USART_RX_MAX_NUMBER];		//串口发送缓存，最多缓存五百条指令
DRV_UARTDATAINTERFACE_TYPE* _gp_UartDataInterface[UARTDATAINTERFACE_BUTT] = {0};	/*串口数据接口模块的注册表*/
/* 接口函数
 ********************************************************/

/* 初始化串口数据接口模块
  -----------------------------
  入口：串口数据接口模块的ID号，波特率，TX、RX端口
  返回值：初始化好的串口数据接口模块结构体
*/
DRV_UARTDATAINTERFACE_TYPE* Drv_UartDataInterfaceInit(DRV_UARTDATAINTERFACE_ENUM id, \
										BSP_USART_ENUM usart_number, uint32_t baud_rate, \
										BSP_PORT_ENUM tx_port_number, BSP_PIN_ENUM tx_pin_number, \
										BSP_PORT_ENUM rx_port_number, BSP_PIN_ENUM rx_pin_number)
{	
	uint8_t i = 0;
	DRV_UARTDATAINTERFACE_TYPE* p_unit = (DRV_UARTDATAINTERFACE_TYPE*)calloc(1, sizeof(DRV_UARTDATAINTERFACE_TYPE));	/*申请内存*/
	
	/*分配失败返回NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id错误返回NULL*/
	if(id >= UARTDATAINTERFACE_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*如果当前模块存在，取消分配新存储区*/
	if(_gp_UartDataInterface[id] != NULL)
	{
		free(p_unit);
		p_unit = _gp_UartDataInterface[id];
	}
	else
	{
		_gp_UartDataInterface[id] = p_unit;
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
	if(p_unit->usart_number == _USART1_)
		BSP_UartIRQHandler[p_unit->usart_number] = _Drv_Usart1ReceiveHandler;	/*注册串口中断处理函数*/
	if(p_unit->usart_number == _USART2_)
		BSP_UartIRQHandler[p_unit->usart_number] = _Drv_Usart2ReceiveHandler;
	p_unit->one_packet_sem = OSSemCreate(0);	/*数据包接收完成信号量*/
	
	/*------------------------------变常*/
	
	/*------------------------------变量*/
	for(i = 0; i < USART_RX_MAX_NUMBER; i++)
	{
		p_unit->rx_buffer[i] = 0;	/*接收缓存*/
	}
	for(i=0;i<MAX_CMD_BUFFER;i++)
	{
		memset(&Usart_Rx_Buffer[i][0],0,USART_RX_MAX_NUMBER);
		memset(&Usart_Tx_Buffer[i][0],0,USART_RX_MAX_NUMBER);
	}
	p_unit->tx_status = USART_TX_IDLE;
	Drv_UsartGetEnable(p_unit);	/*使能接收*/
	
	return p_unit;
}

/* 串口接收使能
  ------------------------------------
*/
void Drv_UsartGetEnable(DRV_UARTDATAINTERFACE_TYPE* p_unit)
{
	p_unit->rx_push = 0;
	p_unit->status = UARTDATAINTERFACE_READY;
}

/* 串口接收禁用
  ------------------------------------
*/
void Drv_UsartGetDisable(DRV_UARTDATAINTERFACE_TYPE* p_unit)
{
	p_unit->rx_push = 0;
	p_unit->status = UARTDATAINTERFACE_BUSY;
}

/* 串口打包发送数据
  ------------------------------------
*/
void Drv_UsartSendPacket(DRV_UARTDATAINTERFACE_TYPE* p_unit, uint32_t data1, uint32_t data2, uint32_t data3)
{
	uint8_t datas_send[11] = {0}, i = 0;
	
	datas_send[0] = 0xAA;
	datas_send[1] = 0xAA;
	datas_send[2] = 0x02;
	datas_send[3] = 0x06;
	datas_send[4] = (data1 >> 8) & 0XFF;
	datas_send[5] = data1 & 0XFF;
	datas_send[6] = (data2 >> 8) & 0XFF;
	datas_send[7] = data2 & 0XFF;
	datas_send[8] = (data3 >> 8) & 0XFF;
	datas_send[9] = data3 & 0XFF;
	
    for(i = 0 ; i < 10 ; i++)
    {
		datas_send[10] += datas_send[i];
		BSP_UsartSendByte(p_unit->usart_number, datas_send[i]);
    }
	BSP_UsartSendByte(p_unit->usart_number, datas_send[i]);
}

/* 串口打包发送命令
  ------------------------------------
*/
void Drv_UsartSendCmd(DRV_UARTDATAINTERFACE_TYPE* p_unit, uint8_t cmd, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5)
{
	uint8_t datas_send[11] = {0}, i = 0;
	
	datas_send[0] = 0xAA;
	datas_send[1] = 0xAA;
	datas_send[2] = 0x02;
	datas_send[3] = 0x06;
	datas_send[4] = cmd;
	datas_send[5] = data1;
	datas_send[6] = data2;
	datas_send[7] = data3;
	datas_send[8] = data4;
	datas_send[9] = data5;
	
    for(i = 0 ; i < 10 ; i++)
    {
		datas_send[10] += datas_send[i];
		BSP_UsartSendByte(p_unit->usart_number, datas_send[i]);
    }
	BSP_UsartSendByte(p_unit->usart_number, datas_send[i]);
}

void _Drv_Usart1ReceiveHandler(void)
{
	uint8_t num;
	uint8_t i;
	if(USART_GetITStatus(USART1,USART_IT_IDLE) == SET)
	{
		num = USART1->SR;
		num = USART1->DR;
		DMA_Cmd(DMA1_Channel5,DISABLE);
		num = 100 - DMA_GetCurrDataCounter(DMA1_Channel5);
		receive_data[num] = '\0';
		for(i=0;i<MAX_CMD_BUFFER;i++)
		{
			if(Usart_Rx_Buffer[i][5] == 0)
			{
				memcpy(&Usart_Rx_Buffer[i][0],receive_data,USART_RX_MAX_NUMBER); //指令放入缓存区
				OSSemPost(_gp_usart1->one_packet_sem);
				break;
			}
		}
		DMA1_Channel5->CNDTR = 100;
		DMA_Cmd(DMA1_Channel5,ENABLE);
	}
//	uint8_t recv;
//	uint8_t i;
//	recv = BSP_UsartGetByte(_USART1_);
//	if(_gp_usart1 == NULL )
//		return;
//	if(recv == 0x0a)
//	{
//		if(_gp_usart1->rx_push&0x0400)	//接收到了0x0a
//		{
//			for(i=0;i<MAX_CMD_BUFFER;i++)
//			{
//				if(Usart_Rx_Buffer[0][i] == 0)
//				{
//					memcpy(&Usart_Rx_Buffer[0][i],_gp_usart1->rx_buffer,USART_RX_MAX_NUMBER); //指令放入缓存区
//					OSSemPost(_gp_usart1->one_packet_sem);
//					break;
//				}
//			}
//			_gp_usart1->rx_push = 0;			//接收完成，下标清零

//		}
//		else
//		{
//			_gp_usart1->rx_push = 0;  //接收错误，下标清零
//		}
//	}
//	else
//	{
//		if(recv == 0x0d)
//			_gp_usart1->rx_push = _gp_usart1->rx_push|0x0400;
//		else
//		{
//			if(_gp_usart1->rx_push > 100)
//				_gp_usart1->rx_push = 0;		//接收错误下标清零
//			else
//			{	
//				_gp_usart1->rx_buffer[_gp_usart1->rx_push&0xff] = recv;
//				_gp_usart1->rx_push++;
//			}
//		}
//	}

}

void _Drv_Usart2ReceiveHandler(void)
{
	return;
}
/*	指令缓冲队列前移
	入口：void
	返回值：void
*/
void _Drv_UsartCmdFifoForward(void)
{
	uint8_t i = 0;
	for(i=1;i<MAX_CMD_BUFFER;i++)
	{
		memcpy(&Usart_Rx_Buffer[i - 1][0],&Usart_Rx_Buffer[i][0],USART_RX_MAX_NUMBER);
	}
	memset(&Usart_Rx_Buffer[4][0],0,USART_RX_MAX_NUMBER);

}

/*	发送缓冲队列前移
	入口：void
	返回值：void
*/
void _Drv_UartTxFifoForward(void)
{
	OS_CPU_SR  cpu_sr;
	uint8_t i = 0;
	OS_ENTER_CRITICAL();
	for(i=1;i<MAX_CMD_BUFFER;i++)
	{
		memcpy(&Usart_Tx_Buffer[i-1][0],&Usart_Rx_Buffer[i][0],USART_RX_MAX_NUMBER);
	}
	memset(&Usart_Tx_Buffer[4][0],0,USART_RX_MAX_NUMBER);
	if(Usart_Tx_Buffer[0][5] == 0)
		_gp_usart1->tx_status = USART_TX_IDLE;				//队列缓存区全部清空
	OS_EXIT_CRITICAL();
}

/* 发送缓存区数据发送
  ------------------------------------
*/
void Drv_UartSendDatas(uint8_t p_buffer[])
{
	OS_CPU_SR  cpu_sr;
	uint8_t length = p_buffer[5] + 8;   //获取数据包长度
	uint8_t i = 0;
	OS_ENTER_CRITICAL();
	for(i=0;i<length;i++)
	{
//		printf("%x",(uint8_t)p_buffer[i]);
		BSP_UsartSendByte(_gp_usart1->usart_number,p_buffer[i]);
	}
	OS_EXIT_CRITICAL();
//	BSP_UsartSendByte(_gp_usart1->usart_number,0x0d);
//	BSP_UsartSendByte(_gp_usart1->usart_number,0x0a);
	memset(p_buffer,0,length);
}

/* 串口回复成功到发送缓存
  ------------------------------------
  入口：	cmd 命令字
			length 包含命令字的数据长度
			datas 数据存放地址
  返回值：无
*/
void _Drv_UsartReturnDoneToBuffer(uint32_t frame_head, uint16_t cmd, uint8_t length, uint8_t* datas)
{
	OS_CPU_SR  cpu_sr;
	uint8_t i = 0;
	COMMON_REP_DATA* p_msg = NULL;
	p_msg = (COMMON_REP_DATA*)malloc(sizeof(COMMON_REP_DATA) + length);
	if(p_msg == NULL)
	{
		free(p_msg);
		return;			//申请内存出错，直接退出
	}
	p_msg->frame_head = frame_head;
	p_msg->status = RETURN_DONE_WORD;
	p_msg->length = length;
	p_msg->cmd = cmd;
	memcpy(p_msg->data,datas,length);
	OS_ENTER_CRITICAL();
	for(i=0;i<MAX_CMD_BUFFER;i++)
	{
		if(Usart_Tx_Buffer[i][5] == 0)
		{
			memcpy(&Usart_Tx_Buffer[i][0],p_msg,length + sizeof(COMMON_REP_DATA));
			break;
		}
	}
	OS_EXIT_CRITICAL();
	if(5 == i&& Usart_Tx_Buffer[4][5] == 0)
	{
		free(p_msg);
		p_msg = NULL;
		return;				//发送缓冲区满，直接退出
	}
	free(p_msg);
	p_msg = NULL;
	_gp_usart1->tx_status = USART_TX_SEND;		//待发送状态置起
}
/* 串口回复失败到发送缓存
  ------------------------------------
  入口：	cmd 命令字
			length 包含命令字的数据长度
			datas 数据存放地址
  返回值：无
*/
void _Drv_UsartReturnFailToBuffer(uint32_t frame_head, uint16_t cmd, uint8_t length, uint8_t* datas)
{
	OS_CPU_SR  cpu_sr;
	uint8_t i = 0;
	COMMON_REP_DATA* p_msg = NULL;
	p_msg = (COMMON_REP_DATA*)malloc(sizeof(COMMON_REP_DATA) + length);
	if(p_msg == NULL)
	{
		free(p_msg);
		return;			//申请内存出错，直接退出
	}
	p_msg->frame_head = frame_head;
	p_msg->status = RETURN_FAIL_WORD;
	p_msg->length = length;
	p_msg->cmd = cmd;
	memcpy(p_msg->data,datas,length);
	OS_ENTER_CRITICAL();
	for(i=0;i<MAX_CMD_BUFFER;i++)
	{
		if(Usart_Tx_Buffer[i][5] == 0)
		{
			memcpy(&Usart_Tx_Buffer[i][0],p_msg,length + sizeof(COMMON_REP_DATA));
			break;
		}
	}
	OS_EXIT_CRITICAL();
	if(5 == i&& Usart_Tx_Buffer[4][5] == 0)
	{
		free(p_msg);
		p_msg = NULL;
		return;				//发送缓冲区满，直接退出
	}
	free(p_msg);
	p_msg = NULL;
	_gp_usart1->tx_status = USART_TX_SEND;		//待发送状态置起
}

/* 串口回复失败到发送缓存
  ------------------------------------
  入口：	err_code 错误编码

  返回值：无
*/
void _Drv_UsartReturnOnlyErrToBuffer(uint32_t frame_head,uint16_t cmd, uint16_t err_code)
{
	OS_CPU_SR  cpu_sr;
	uint8_t i = 0;
	COMMON_REP_DATA* p_msg = NULL;
	p_msg = (COMMON_REP_DATA*)malloc(sizeof(COMMON_REP_DATA) + 4);
	if(p_msg == NULL)
	{
		free(p_msg);
		return;			//申请内存出错，直接退出
	}
	p_msg->frame_head = frame_head;
	p_msg->status = RETURN_FAIL_WORD;
	p_msg->length = 4;
	p_msg->cmd = cmd;
	p_msg->data[0] = err_code&0xff;
	p_msg->data[1] = (err_code>>8)&0xff;
	p_msg->data[2] = 0;
	p_msg->data[3] = 0;
	OS_ENTER_CRITICAL();
	for(i=0;i<MAX_CMD_BUFFER;i++)
	{
		if(Usart_Tx_Buffer[i][5] == 0)
		{
			memcpy(&Usart_Tx_Buffer[i][0],p_msg,4 + sizeof(COMMON_REP_DATA));
			break;
		}
	}
	OS_EXIT_CRITICAL();
	if(5 == i&& Usart_Tx_Buffer[4][5] == 0)
	{
		free(p_msg);
		p_msg = NULL;
		return;				//发送缓冲区满，直接退出
	}
	free(p_msg);
	p_msg = NULL;
	_gp_usart1->tx_status = USART_TX_SEND;		//待发送状态置起
}
/* 串口主动上报失败到缓存
  ------------------------------------
  入口：	err_code 错误编码

  返回值：无
*/
void _Drv_UsartReportErrToBuffer(uint32_t frame_head, uint16_t err_code, uint8_t length, uint8_t* datas)
{
	OS_CPU_SR  cpu_sr;
	uint8_t i = 0;
	COMMON_REP_DATA* p_msg = NULL;
	p_msg = (COMMON_REP_DATA*)malloc(sizeof(COMMON_REP_DATA) + length);
	if(p_msg == NULL)
	{
		free(p_msg);
		return;			//申请内存出错，直接退出
	}
	p_msg->frame_head = frame_head;
	p_msg->status = REPORT_ERROR_WORD;
	p_msg->length = length;
	p_msg->cmd = 0;
	memcpy(p_msg->data,datas,length);
	OS_ENTER_CRITICAL();
	for(i=0;i<MAX_CMD_BUFFER;i++)
	{
		if(Usart_Tx_Buffer[i][5] == 0)
		{
			memcpy(&Usart_Tx_Buffer[i][0],p_msg,length + sizeof(COMMON_REP_DATA));
			break;
		}
	}
	OS_EXIT_CRITICAL();
	if(5 == i&& Usart_Tx_Buffer[4][5] == 0)
	{
		free(p_msg);
		p_msg = NULL;
		return;				//发送缓冲区满，直接退出
	}
	free(p_msg);
	p_msg = NULL;
	_gp_usart1->tx_status = USART_TX_SEND;		//待发送状态置起
}

void _Drv_UsartReportEventToBuffer(uint32_t frame_head, uint16_t cmd, uint8_t length, uint8_t* datas)
{
	OS_CPU_SR  cpu_sr;
	uint8_t i = 0;
	COMMON_REP_DATA* p_msg = NULL;
	p_msg = (COMMON_REP_DATA*)malloc(sizeof(COMMON_REP_DATA)+length);
	if(p_msg == NULL)
	{
		free(p_msg);
		return;			//申请内存出错，直接退出
	}
	p_msg->frame_head = frame_head;
	p_msg->status = REPORT_EVENT_WORD;
	p_msg->length = length;
	p_msg->cmd = 0;
	memcpy(p_msg->data,datas,length);
	OS_ENTER_CRITICAL();
	for(i=0;i<MAX_CMD_BUFFER;i++)
	{
		if(Usart_Tx_Buffer[i][5] == 0)
		{
			memcpy(&Usart_Tx_Buffer[i][0],p_msg,length + sizeof(COMMON_REP_DATA));
			break;
		}
	}
	OS_EXIT_CRITICAL();
	if(5 == i&& Usart_Tx_Buffer[4][5] == 0)
	{
		free(p_msg);
		p_msg = NULL;
		return;				//发送缓冲区满，直接退出
	}
	free(p_msg);
	p_msg = NULL;
	_gp_usart1->tx_status = USART_TX_SEND;		//待发送状态置起
}

