/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——BSP层 CAN封装

  -------------------------------------------------------
   文 件 名   : BSP_Can.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年07月17日
   功能描述   : 定义一种基于STM32的板级支持包。
				该层依赖于硬件电路。
   依赖于     : STM32F10x_StdPeriph_Lib_V3.5.0
 ********************************************************/
 
#include "BSP_Can.h"

static CanTxMsg g_BSP_CANTxMessage;	/*发送缓存*/
static CanRxMsg g_BSP_CANRxMessage;	/*接收缓存*/

/*CAN接收中断函数注册表*/
void(* BSP_CanRxIRQHandler)(uint32_t* p_std_id,	/*标准ID*/
							uint32_t* p_ext_id,	/*扩展ID*/
							uint8_t*  p_ide,	/*ID类型*/
							uint8_t*  p_rtr,	/*帧类型*/
							uint8_t*  p_dlc,	/*帧长度*/
							uint8_t*  p_data,	/*接收的数据，8个字节*/
							uint8_t*  p_fmi) = {0};	/*CAN接收中断中断函数指针列表*/

/* 初始化
  ----------------------------------------------------------------------*/

/* 初始化CAN及接收中断
  -----------------------------------------
  最多可同时支持4个can id的接收，can_id为0的不初始化
  入口：CAN口编号、波特率（单位K），CAN ID，TX、RX端口
  返回值：成功true，失败false
*/
bool BSP_Init_CANinterrupt(BSP_CAN_ENUM can_number, uint32_t baud_rate, \
						   uint8_t can_id1, uint8_t can_id2, uint8_t can_id3, uint8_t can_id4, \
						   BSP_PORT_ENUM tx_port_number, BSP_PIN_ENUM tx_pin_number, \
						   BSP_PORT_ENUM rx_port_number, BSP_PIN_ENUM rx_pin_number)
{
    GPIO_InitTypeDef      GPIO_InitStructure;
    CAN_InitTypeDef       CAN_InitStructure;
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
	NVIC_InitTypeDef      NVIC_InitStructure;

	if(can_number != _CAN1_)
		return false;
		
	/*can id 1 不能为0*/
	if(can_id1 == 0)
		return false;
	
	/*开外设时钟*/
	RCC_APB2PeriphClockCmd((RCC_APB2Periph_GPIOA << tx_port_number) | (RCC_APB2Periph_GPIOA << rx_port_number) | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	/*IO设置*/
	if(tx_port_number == _PD_)
		GPIO_PinRemapConfig(GPIO_Remap2_CAN1, ENABLE);
	else if(tx_port_number == _PB_)
		GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
	else if(tx_port_number == _PA_)
		;
	else
		return false;
    
	/*RX*/
    GPIO_InitStructure.GPIO_Pin  = PIN(rx_pin_number);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PORT(rx_port_number), &GPIO_InitStructure);
    
	/*TX*/
    GPIO_InitStructure.GPIO_Pin = PIN(tx_pin_number);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PORT(tx_port_number), &GPIO_InitStructure);

	/*中断配置*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	/*2位主优先级，2位次优先级*/
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
		
    /*CAN配置*/
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
    CAN_InitStructure.CAN_TTCM = DISABLE;		/*禁止 时间触发通信模式*/
    CAN_InitStructure.CAN_ABOM = DISABLE;		/*禁止 自动离线管理*/
    CAN_InitStructure.CAN_AWUM = ENABLE;		/*使能 自动唤醒模式*/
    CAN_InitStructure.CAN_NART = DISABLE;		/*禁止 禁止自动重传*/
    CAN_InitStructure.CAN_RFLM = DISABLE;		/*禁止 接收FIFO锁定模式，溢出时新报文覆盖原有报文*/
    CAN_InitStructure.CAN_TXFP = DISABLE;		/*禁止 发送FIFO优先级，优先级取决于报文标识符*/
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;	/*正常工作模式*/
    /*设置波特率*/
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;	/*重新同步跳跃宽度，1个时间单元*/
    CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;	/*时间段1占用3个时间单元*/
    CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;	/*时间段2占用2个时间单元*/
    CAN_InitStructure.CAN_Prescaler = 6000 / baud_rate;		/*波特率分频器，Tq = 1 / (FAPB1 / Prescaler)，其中APB1 36M*/
    CAN_Init(CAN1, &CAN_InitStructure);			/*波特率 = 1 / (Tq + BS1_Tq + BS2_Tq)*/

    /*设置滤波器*/
    CAN_FilterInitStructure.CAN_FilterNumber = 0;	/*过滤器组0*/
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;		/*标识符屏蔽位模式*/
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;	/*过滤器位宽为单个32位*/
    CAN_FilterInitStructure.CAN_FilterIdHigh = (((uint32_t)(can_id1|0x40) << 21) & 0xFFFF0000) >> 16;
    CAN_FilterInitStructure.CAN_FilterIdLow = (((uint32_t)(can_id1|0x40) << 21) | CAN_ID_STD | CAN_RTR_DATA) & 0xFFFF;	/*将CAN ID1 写入通道0的标识符寄存器 标准ID 数据帧*/
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0FE0;	/*最低6位表示ID号*/
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;	/*关联到FIFO0*/
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;	/*使能过滤器*/
    CAN_FilterInit(&CAN_FilterInitStructure);

	if(can_id2 != 0)	/*CAN ID 2 不为0 才使能过滤组1*/
	{
		/*设置滤波器*/
		CAN_FilterInitStructure.CAN_FilterNumber = 1;	/*过滤器组1*/
		CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;		/*标识符屏蔽位模式*/
		CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;	/*过滤器位宽为单个32位*/
		CAN_FilterInitStructure.CAN_FilterIdHigh = (((uint32_t)(can_id2|0x40) << 21) & 0xFFFF0000) >> 16;
		CAN_FilterInitStructure.CAN_FilterIdLow = (((uint32_t)(can_id2|0x40) << 21) | CAN_ID_STD | CAN_RTR_DATA) & 0xFFFF;	/*将CAN ID2 写入通道1的标识符寄存器 扩展ID 数据帧*/
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0FE0;	/*最低6位表示ID号*/
		CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;	/*关联到FIFO0*/
		CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;	/*使能过滤器*/
		CAN_FilterInit(&CAN_FilterInitStructure);
		
		if(can_id3 != 0)	/*CAN ID 3 不为0 才使能过滤组2*/
		{
			/*设置滤波器*/
			CAN_FilterInitStructure.CAN_FilterNumber = 2;	/*过滤器组2*/
			CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;		/*标识符屏蔽位模式*/
			CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;	/*过滤器位宽为单个32位*/
			CAN_FilterInitStructure.CAN_FilterIdHigh = (((uint32_t)(can_id3|0x40) << 21) & 0xFFFF0000) >> 16;
			CAN_FilterInitStructure.CAN_FilterIdLow = (((uint32_t)(can_id3|0x40) << 21) | CAN_ID_STD | CAN_RTR_DATA) & 0xFFFF;	/*将CAN ID2 写入通道1的标识符寄存器 扩展ID 数据帧*/
			CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0FE0;	/*最低6位表示ID号*/
			CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
			CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;	/*关联到FIFO0*/
			CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;	/*使能过滤器*/
			CAN_FilterInit(&CAN_FilterInitStructure);
			
			if(can_id4 != 0)	/*CAN ID 4 不为0 才使能过滤组5*/
			{
				/*设置滤波器*/
				CAN_FilterInitStructure.CAN_FilterNumber = 3;	/*过滤器组3*/
				CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;		/*标识符屏蔽位模式*/
				CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;	/*过滤器位宽为单个32位*/
				CAN_FilterInitStructure.CAN_FilterIdHigh = (((uint32_t)(can_id4|0x40) << 21) & 0xFFFF0000) >> 16;
				CAN_FilterInitStructure.CAN_FilterIdLow = (((uint32_t)(can_id4|0x40) << 21) | CAN_ID_STD | CAN_RTR_DATA) & 0xFFFF;	/*将CAN ID2 写入通道1的标识符寄存器 扩展ID 数据帧*/
				CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0FE0;	/*最低6位表示ID号*/
				CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
				CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;	/*关联到FIFO0*/
				CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;	/*使能过滤器*/
				CAN_FilterInit(&CAN_FilterInitStructure);
			}
		}
	}
    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);	/*使能FIFO0接收中断*/
	
	return true;
}

/* 操作
  ----------------------------------------------------------------------*/

/* CAN TX 发送数据帧
  -----------------------------------------
  入口：CAN ID, 待发送的数据，帧长度
  返回值：成功true，失败false
*/
bool BSP_CAN1_TX(uint8_t can_id, uint8_t* data, uint8_t dlc)
{
	uint32_t i;
	uint8_t ret_msg_box = 0;
	
	g_BSP_CANTxMessage.StdId = can_id;
	g_BSP_CANTxMessage.ExtId = 0x00;
	g_BSP_CANTxMessage.IDE   = CAN_ID_STD;
	g_BSP_CANTxMessage.RTR   = CAN_RTR_DATA;
	g_BSP_CANTxMessage.DLC   = dlc;
	
	for(i = 0; i < 8; i++)
		g_BSP_CANTxMessage.Data[i] = data[i];
	
	ret_msg_box = CAN_Transmit(CAN1, &g_BSP_CANTxMessage);
	if(ret_msg_box == CAN_TxStatus_NoMailBox)
		return false;
	i = 0;
	while((CAN_TransmitStatus(CAN1, ret_msg_box) != CAN_TxStatus_Ok) && (i < 10))	/*等待发送结束*/
	{
		OSTimeDlyHMSM(0, 0, 0, 1);	/*挂起1ms*/
		i++;
	}
	if(i >= 10)
	{
		return false;	/*超时失败返回*/
	}
	
	return true;
}

/* CAN TX 接收中断使能
  -----------------------------------------
*/
void BSP_CAN1_IT_ENABLE(void)
{
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);	/*使能FIFO0接收中断*/
}

/* CAN TX 接收中断不使能
  -----------------------------------------
*/
void BSP_CAN1_IT_DISENABLE(void)
{
	CAN_ITConfig(CAN1, CAN_IT_FMP0, DISABLE);	/*不使能FIFO0接收中断*/
}

/* 中断
  ----------------------------------------------------------------------*/
  
/* CAN RX0接收中断
  -----------------------------------------
*/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	CAN_Receive(CAN1, CAN_FIFO0, &g_BSP_CANRxMessage);
	if(BSP_CanRxIRQHandler != 0)
	{
		(*BSP_CanRxIRQHandler)(&g_BSP_CANRxMessage.StdId, 	/*标准ID*/
						   &g_BSP_CANRxMessage.ExtId, 	/*扩展ID*/
						   &g_BSP_CANRxMessage.IDE, 	/*ID类型*/
						   &g_BSP_CANRxMessage.RTR, 	/*帧类型*/
						   &g_BSP_CANRxMessage.DLC, 	/*帧长度*/
						   g_BSP_CANRxMessage.Data,		/*接收的数据，8个字节*/
						   &g_BSP_CANRxMessage.FMI);	/*接收处理*/
	}
}
