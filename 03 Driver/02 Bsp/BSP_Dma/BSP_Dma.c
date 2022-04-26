/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——BSP层 ADC封装

  -------------------------------------------------------
   文 件 名   : BSP_Adc.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2020年02月17日
   功能描述   : 定义一种基于STM32的板级支持包。
				该层依赖于硬件电路。
   依赖于     : STM32F10x_StdPeriph_Lib_V3.5.0
 ********************************************************/
 
#include "BSP_Adc.h"

/*外部中断函数注册表*/
void(* BSP_ExtiIRQHandler[16])(void) = {0};	/*IO中断函数指针列表*/

/* 初始化
  ----------------------------------------------------------------------*/

/* 初始化ADC
  -----------------------------------------
  入口：引脚端口、引脚编号、输入/推挽/开漏
  返回值：成功true，失败false
*/
bool BSP_Init_Adc(BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number, BSP_IO_TYPE_ENUM type)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	if(port_number >= _PORT_BUTT_)
		return false;
	if(pin_number  >= _PIN_BUTT_)
		return false;
	if(type >= _IO_TYPE_BUTT_)
		return false;
		
	/*使能端口时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA << port_number, ENABLE);
	
	/*初始化端口配置*/
	if((port_number == _PC_) && ((pin_number == _P13_) || (pin_number == _P14_)))	/*初始化PC13、PC14*/
	{
		PWR_BackupAccessCmd(ENABLE);	/*允许修改RTC和后备寄存器*/
		RCC_LSEConfig(RCC_LSE_OFF); 	/*关闭外部低速时钟,PC14+PC15可以用作普通IO*/
		BKP_TamperPinCmd(DISABLE); 		/*关闭入侵检测功能,PC13可以用作普通IO*/			
	}
	
	if(((port_number == _PB_) && ((pin_number == _P3_) || (pin_number == _P4_))) || ((port_number == _PA_) && (pin_number == _P15_)))		/*初始化PB3、PB4、PA15需要禁用JTAG*/
	{
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);			
	}
	
	GPIO_InitStructure.GPIO_Pin = PIN(pin_number);
	switch(type)
	{
		case _IN_FLOATING_:
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			break;
		case _OUT_PP_:
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			break;
		case _OUT_OD_:
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
			break;
		default:
			return false;
	}
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PORT(port_number), &GPIO_InitStructure);
	
	if((port_number == _PC_) && ((pin_number == _P13_) || (pin_number == _P14_)))	/*初始化PC13、PC14*/
	{
		PWR_BackupAccessCmd(DISABLE);	/*禁止修改RTC和后备寄存器*/
	}

	return true;
}

/* 初始化引脚中断
  -----------------------------------------
  入口：引脚端口、引脚编号
  返回值：成功true，失败false
*/
bool BSP_Init_PinInterrupt(BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};
	IRQn_Type irq;
	uint32_t line;
	uint8_t port_source;
	uint8_t pin_source;

	if(port_number >= _PORT_BUTT_)
		return false;
	if(pin_number  >= _PIN_BUTT_)
		return false;
		
	/*使能端口时钟*/
	RCC_APB2PeriphClockCmd((RCC_APB2Periph_GPIOA << port_number) | RCC_APB2Periph_AFIO, ENABLE);
	
	/*初始化端口配置*/
	if((port_number == _PC_) && ((pin_number == _P13_) || (pin_number == _P14_)))	/*初始化PC13、PC14*/
	{
		PWR_BackupAccessCmd(ENABLE);	/*允许修改RTC和后备寄存器*/
		RCC_LSEConfig(RCC_LSE_OFF); 	/*关闭外部低速时钟,PC14+PC15可以用作普通IO*/
		BKP_TamperPinCmd(DISABLE); 		/*关闭入侵检测功能,PC13可以用作普通IO*/			
	}
	
	GPIO_InitStructure.GPIO_Pin   = PIN(pin_number);
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PORT(port_number), &GPIO_InitStructure);
	
	if((port_number == _PC_) && ((pin_number == _P13_) || (pin_number == _P14_)))	/*初始化PC13、PC14*/
	{
		PWR_BackupAccessCmd(DISABLE);	/*禁止修改RTC和后备寄存器*/
	}
	
	/*初始化外部中断优先级*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		/*2位抢占优先级，2位响应优先级*/
	switch(port_number)
	{
		case _PA_:
			port_source = GPIO_PortSourceGPIOA;
			break;
		case _PB_:
			port_source = GPIO_PortSourceGPIOB;
			break;
		case _PC_:
			port_source = GPIO_PortSourceGPIOC;
			break;
		case _PD_:
			port_source = GPIO_PortSourceGPIOD;
			break;
		case _PE_:
			port_source = GPIO_PortSourceGPIOE;
			break;
		case _PF_:
			port_source = GPIO_PortSourceGPIOF;
			break;
		case _PG_:
			port_source = GPIO_PortSourceGPIOG;
			break;
		default:
			return false;
	}
	switch(pin_number)
	{
		case _P0_: 
			irq = EXTI0_IRQn;     line = EXTI_Line0;  pin_source = GPIO_PinSource0; 
			break;
		case _P1_: 
			irq = EXTI1_IRQn;     line = EXTI_Line1;  pin_source = GPIO_PinSource1; 
			break;
		case _P2_: 
			irq = EXTI2_IRQn;     line = EXTI_Line2;  pin_source = GPIO_PinSource2; 
			break;
		case _P3_: 
			irq = EXTI3_IRQn;     line = EXTI_Line3;  pin_source = GPIO_PinSource3; 
			break;
		case _P4_: 
			irq = EXTI4_IRQn;     line = EXTI_Line4;  pin_source = GPIO_PinSource4; 
			break;
		case _P5_: 
			irq = EXTI9_5_IRQn;   line = EXTI_Line5;  pin_source = GPIO_PinSource5; 
			break;
		case _P6_: 
			irq = EXTI9_5_IRQn;   line = EXTI_Line6;  pin_source = GPIO_PinSource6; 
			break;
		case _P7_: 
			irq = EXTI9_5_IRQn;   line = EXTI_Line7;  pin_source = GPIO_PinSource7; 
			break;
		case _P8_: 
			irq = EXTI9_5_IRQn;   line = EXTI_Line8;  pin_source = GPIO_PinSource8; 
			break;
		case _P9_: 
			irq = EXTI9_5_IRQn;   line = EXTI_Line9;  pin_source = GPIO_PinSource9; 
			break;
		case _P10_: 
			irq = EXTI15_10_IRQn; line = EXTI_Line10; pin_source = GPIO_PinSource10; 
			break;
		case _P11_: 
			irq = EXTI15_10_IRQn; line = EXTI_Line11; pin_source = GPIO_PinSource11; 
			break;
		case _P12_: 
			irq = EXTI15_10_IRQn; line = EXTI_Line12; pin_source = GPIO_PinSource12; 
			break;
		case _P13_: 
			irq = EXTI15_10_IRQn; line = EXTI_Line13; pin_source = GPIO_PinSource13; 
			break;
		case _P14_: 
			irq = EXTI15_10_IRQn; line = EXTI_Line14; pin_source = GPIO_PinSource14; 
			break;
		case _P15_: 
			irq = EXTI15_10_IRQn; line = EXTI_Line15; pin_source = GPIO_PinSource15; 
			break;
		default:
			return false;
	}
	NVIC_InitStructure.NVIC_IRQChannel = irq;			/*初始化优先级*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/*初始化外部中断*/
	EXTI_ClearITPendingBit(line);
	GPIO_EXTILineConfig(port_source, pin_source);
	EXTI_InitStructure.EXTI_Line = line;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;	/*上升、下降*/
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	return true;
}

/* 操作
  ----------------------------------------------------------------------*/

/* 读引脚状态
  -----------------------------------------
  入口：引脚端口、引脚编号
  返回值：高电平true，低电平false
*/
bool BSP_ReadPin(BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number)
{
	return (bool)GPIO_ReadInputDataBit(PORT(port_number), PIN(pin_number));
}

/* 写引脚状态
  -----------------------------------------
  入口：引脚端口、引脚编号
*/
void BSP_WritePin(BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number, bool status)
{
	GPIO_WriteBit(PORT(port_number), PIN(pin_number), (BitAction)status);
}

/* 中断
  ----------------------------------------------------------------------*/
  
/* 外部中断0
  -----------------------------------------
*/
void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line0);
		if(BSP_ExtiIRQHandler[0] != 0)
			(*BSP_ExtiIRQHandler[0])();
	}
}

/* 外部中断1
  -----------------------------------------
*/
void EXTI1_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line1);
		if(BSP_ExtiIRQHandler[1] != 0)
			(*BSP_ExtiIRQHandler[1])();
	}
}

/* 外部中断2
  -----------------------------------------
*/
void EXTI2_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line2);
		if(BSP_ExtiIRQHandler[2] != 0)
			(*BSP_ExtiIRQHandler[2])();
	}
}

/* 外部中断3
  -----------------------------------------
*/
void EXTI3_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line3);
		if(BSP_ExtiIRQHandler[3] != 0)
			(*BSP_ExtiIRQHandler[3])();
	}
}

/* 外部中断4
  -----------------------------------------
*/
void EXTI4_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line4) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line4);
		if(BSP_ExtiIRQHandler[4] != 0)
			(*BSP_ExtiIRQHandler[4])();
	}
}

/* 外部中断5-9
  -----------------------------------------
*/
void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line5);
		if(BSP_ExtiIRQHandler[5] != 0)
			(*BSP_ExtiIRQHandler[5])();
	}
    if(EXTI_GetITStatus(EXTI_Line6) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line6);
		if(BSP_ExtiIRQHandler[6] != 0)
			(*BSP_ExtiIRQHandler[6])();
	}
    if(EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line7);
		if(BSP_ExtiIRQHandler[7] != 0)
			(*BSP_ExtiIRQHandler[7])();
	}
    if(EXTI_GetITStatus(EXTI_Line8) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line8);
		if(BSP_ExtiIRQHandler[8] != 0)
			(*BSP_ExtiIRQHandler[8])();
	}
    if(EXTI_GetITStatus(EXTI_Line9) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line9);
		if(BSP_ExtiIRQHandler[9] != 0)
			(*BSP_ExtiIRQHandler[9])();
	}
}

/* 外部中断10-15
  -----------------------------------------
*/
void EXTI15_10_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line10) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line10);
		if(BSP_ExtiIRQHandler[10] != 0)
			(*BSP_ExtiIRQHandler[10])();
	}
    if(EXTI_GetITStatus(EXTI_Line11) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line11);
		if(BSP_ExtiIRQHandler[11] != 0)
			(*BSP_ExtiIRQHandler[11])();
	}
    if(EXTI_GetITStatus(EXTI_Line12) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line12);
		if(BSP_ExtiIRQHandler[12] != 0)
			(*BSP_ExtiIRQHandler[12])();
	}
    if(EXTI_GetITStatus(EXTI_Line13) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line13);
		if(BSP_ExtiIRQHandler[13] != 0)
			(*BSP_ExtiIRQHandler[13])();
	}
    if(EXTI_GetITStatus(EXTI_Line14) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line14);
		if(BSP_ExtiIRQHandler[14] != 0)
			(*BSP_ExtiIRQHandler[14])();
	}
    if(EXTI_GetITStatus(EXTI_Line15) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line15);
		if(BSP_ExtiIRQHandler[15] != 0)
			(*BSP_ExtiIRQHandler[15])();
	}
}
