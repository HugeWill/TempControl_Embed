/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——BSP层 UART封装

  -------------------------------------------------------
   文 件 名   : BSP_Uart.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年11月14日
   功能描述   : 定义一种基于STM32的板级支持包。
				该层依赖于硬件电路。
   依赖于     : STM32F10x_StdPeriph_Lib_V3.5.0
 ********************************************************/
 
#include "BSP_Uart.h"


#if 1
//#pragma import(__use_no_semihosting)             
//±ê×¼¿âÐèÒªµÄÖ§³Öº¯Êý                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//¶¨Òå_sys_exit()ÒÔ±ÜÃâÊ¹ÓÃ°ëÖ÷»úÄ£Ê½    
_sys_exit(int x) 
{ 
	x = x; 
} 
//ÖØ¶¨Òåfputcº¯Êý 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//Ñ­»··¢ËÍ,Ö±µ½·¢ËÍÍê±Ï   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*串口中断函数注册表*/
void(* BSP_UartIRQHandler[4])(void) = {0};	/*串口中断函数指针列表*/
uint8_t receive_data[100];

/* 初始化
  ----------------------------------------------------------------------*/

/*初始化DMA，内部函数*/
void DMA_Buffer_Init(void)
{
	DMA_InitTypeDef			DMA_Initstructure;
	NVIC_InitTypeDef    NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	
	DMA_Initstructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);
	DMA_Initstructure.DMA_MemoryBaseAddr = (u32)receive_data;
	DMA_Initstructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_Initstructure.DMA_BufferSize = 100;
	DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_Initstructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;
	DMA_Initstructure.DMA_Priority = DMA_Priority_High;
	DMA_Initstructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel5,&DMA_Initstructure);
	
	DMA_Cmd(DMA1_Channel5,ENABLE);
}

/* 初始化UART及接收中断
  -----------------------------------------
  入口：串口编号、波特率，TX、RX端口
  返回值：成功true，失败false
*/
bool BSP_Init_UsartInterrupt(BSP_USART_ENUM usart_number, uint32_t baud_rate, \
							 BSP_PORT_ENUM tx_port_number, BSP_PIN_ENUM tx_pin_number, \
							 BSP_PORT_ENUM rx_port_number, BSP_PIN_ENUM rx_pin_number)
{
	GPIO_InitTypeDef    GPIO_InitStructure;  
	USART_InitTypeDef   USART_InitStructure; 
	NVIC_InitTypeDef    NVIC_InitStructure;
	
	
	uint8_t irq;

	/*初始化串口时钟*/
	switch(usart_number)
	{
		case _USART1_:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
			break;
			
		case _USART2_:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
			break;
			
		case _USART3_:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
			break;

#ifdef STM32F10X_HD
		case _UART4_:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
			break;
#endif
			
		default:
			return false;
	}
	/*引脚时钟初始化*/
	RCC_APB2PeriphClockCmd((RCC_APB2Periph_GPIOA << tx_port_number) | (RCC_APB2Periph_GPIOA << rx_port_number) | RCC_APB2Periph_AFIO, ENABLE);
	
	/*使能串口重映射*/
	switch(usart_number)
	{
		case _USART1_:
			if(tx_port_number == _PB_)		/*需要重映射*/
				GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
			break;
			
		case _USART2_:
			if(tx_port_number == _PD_)		/*需要重映射*/
				GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
			break;
			
		case _USART3_:
			if(tx_port_number == _PC_)		/*需要重映射*/
				GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);
			else if(tx_port_number == _PD_)	/*需要重映射*/
				GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
			break;

#ifdef STM32F10X_HD
		case _UART4_:
			break;
#endif
			
		default:
			return false;
	}
		
	/*管脚配置*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 << tx_pin_number;			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
	GPIO_Init(PORT(tx_port_number), &GPIO_InitStructure);			/* TXIO */
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 << rx_pin_number;			 	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(PORT(rx_port_number), &GPIO_InitStructure); 			/* RXIO */
	
	/*串口工作模式配置*/
	USART_InitStructure.USART_BaudRate = baud_rate;						        	    /*设置波特率*/	
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 		                /*8位数据位*/		 	
	USART_InitStructure.USART_StopBits = USART_StopBits_1;					            /*1位停止位*/	
	USART_InitStructure.USART_Parity = USART_Parity_No; 						        /*无校验*/
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     /*硬件流控 None*/
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;        				/*设置为收发模式*/
	
	switch(usart_number)
	{
		case _USART1_:
			USART_Init(USART1, &USART_InitStructure);			                   		/*初始化UART*/
			USART_Cmd(USART1, ENABLE);	                                           		/*开启串口*/
			irq = USART1_IRQn;
			break;
			
		case _USART2_:
			USART_Init(USART2, &USART_InitStructure);			                   		/*初始化UART*/
			USART_Cmd(USART2, ENABLE);	                                           		/*开启串口*/
			irq = USART2_IRQn;
			break;
			
		case _USART3_:
			USART_Init(USART3, &USART_InitStructure);			                   		/*初始化UART*/
			USART_Cmd(USART3, ENABLE);	                                           		/*开启串口*/
			irq = USART3_IRQn;
			break;

#ifdef STM32F10X_HD
		case _UART4_:
			USART_Init(UART4, &USART_InitStructure);			                   		/*初始化UART*/
			USART_Cmd(UART4, ENABLE);	                                           		/*开启串口*/
			irq = UART4_IRQn;
			break;			
#endif
			
		default:
			return false;
	}                             

	/*中断设置*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		
	NVIC_InitStructure.NVIC_IRQChannel = irq;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;       					/*低优先级别的中断*/
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			    					/*响应中断等级为0*/
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  
	
	switch(usart_number)
	{
		case _USART1_:
			USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);	                 					/*Enable UART IRQ*/
			break;
			
		case _USART2_:
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	                 					/*Enable UART IRQ*/
			break;
			
		case _USART3_:
			USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	                 					/*Enable UART IRQ*/
			break;

#ifdef STM32F10X_HD
		case _UART4_:
			USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	                 					/*Enable UART IRQ*/
			break;
#endif
			
		default:
			return false;
	}
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
	DMA_Buffer_Init();
	return true;
}

/* 操作
  ----------------------------------------------------------------------*/

/* 串口发送一个字节
  -----------------------------
  入口：串口编号，发送内容
  返回值：成功true，失败false
*/
bool BSP_UsartSendByte(BSP_USART_ENUM usart_number, uint8_t data)
{
	OS_CPU_SR  cpu_sr;
	OS_ENTER_CRITICAL();
	switch(usart_number)
	{
		case _USART1_: 
			USART_SendData(USART1, data); 			/*发送数据*/
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
			break;
		case _USART2_:
			USART_SendData(USART2, data); 			/*发送数据*/
			while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
			break;
		case _USART3_:
			USART_SendData(USART3, data); 			/*发送数据*/
			while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
			break;
			
#ifdef STM32F10X_HD
		case _UART4_:
			USART_SendData(UART4, data); 			/*发送数据*/
			while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
			break;
#endif

		default: 
			return false;
	}
	OS_EXIT_CRITICAL();
	return true;
}

/* 串口接收一个字节
-----------------------------ok
入口：串口编号
返回值：串口接收到的数据
*/
uint8_t BSP_UsartGetByte(BSP_USART_ENUM usart_number)
{
	switch(usart_number)
	{
		case _USART1_:
			return USART_ReceiveData(USART1);
		case _USART2_:
			return USART_ReceiveData(USART2);
		case _USART3_:
			return USART_ReceiveData(USART3);
			
#ifdef STM32F10X_HD
		case _UART4_:
			return USART_ReceiveData(UART4);
#endif

		default:
			return 0;
	}
}

/* 中断
  ----------------------------------------------------------------------*/
 
/* 串口1中断
  -------------------------------
*/
void USART1_IRQHandler(void)
{
//   if(USART_GetITStatus(USART1, USART_IT_RXNE)!=RESET)
//   {
//		USART_ClearITPendingBit(USART1 , USART_IT_RXNE); 
		if(BSP_UartIRQHandler[0] != 0)
			(*BSP_UartIRQHandler[0])();
//   }
}

/* 串口2中断
  -------------------------------
*/
void USART2_IRQHandler(void)
{
   if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
   {
		USART_ClearITPendingBit(USART2 , USART_IT_RXNE);
		if(BSP_UartIRQHandler[1] != 0)
			(*BSP_UartIRQHandler[1])();
   }
}

/* 串口3中断
  -------------------------------
*/
void USART3_IRQHandler(void)
{
 if(USART_GetITStatus(USART3, USART_IT_RXNE)!=RESET)
 {	   
	USART_ClearFlag(USART3, USART_FLAG_RXNE); 
	USART_ClearITPendingBit(USART3, USART_IT_RXNE); 
	if(BSP_UartIRQHandler[2] != 0)	/*有处理函数的由处理函数处理*/
	{
		(*BSP_UartIRQHandler[2])();
	}
	else	/*否者读一次，以清空缓存*/
	{
		USART_ReceiveData(USART3);
	}
 }
	if(USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)
	{
		USART_ReceiveData(USART3);
		USART_ClearFlag(USART3, USART_FLAG_ORE); 
	}
}

/* 串口4中断
  -------------------------------
*/
void UART4_IRQHandler(void)
{
   if(USART_GetITStatus(UART4,USART_IT_RXNE)!=RESET)
   {	   
		USART_ClearITPendingBit(UART4 , USART_IT_RXNE); 
		if(BSP_UartIRQHandler[3] != 0)
			(*BSP_UartIRQHandler[3])();
   }
}