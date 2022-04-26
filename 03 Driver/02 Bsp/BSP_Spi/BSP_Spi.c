/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

			嵌入式开发平台——BSP层 SPI接口封装

  -------------------------------------------------------
   文 件 名   : BSP_Spi.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年10月15日
   功能描述   : 定义一种基于STM32的板级支持包。
				该层依赖于硬件电路。
   依赖于     : STM32F10x_StdPeriph_Lib_V3.5.0
 ********************************************************/
 
#include "BSP_Spi.h"

/* 初始化
  ----------------------------------------------------------------------*/

/* 初始化SPI接口
  -----------------------------------------
  入口：SPI模块编号，引脚端口
  返回值：成功true，失败false
*/
bool BSP_InitSpi(BSP_SPI_ENUM spi_number, \
					BSP_PORT_ENUM nss_port_number, BSP_PIN_ENUM nss_pin_number, \
					BSP_PORT_ENUM sck_port_number, BSP_PIN_ENUM sck_pin_number, \
					BSP_PORT_ENUM miso_port_number, BSP_PIN_ENUM miso_pin_number, \
					BSP_PORT_ENUM mosi_port_number, BSP_PIN_ENUM mosi_pin_number)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;
	
	if(spi_number >= _SPI_BUTT_)
		return false;
	
	/*开外设时钟*/
	RCC_APB2PeriphClockCmd((RCC_APB2Periph_GPIOA << nss_port_number) | \
							(RCC_APB2Periph_GPIOA << sck_port_number) | \
							(RCC_APB2Periph_GPIOA << miso_port_number) | \
							(RCC_APB2Periph_GPIOA << mosi_port_number), ENABLE);
	switch(spi_number)
	{
		case _SPI1_:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
			break;
		case _SPI2_:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
			break;
		case _SPI3_:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);
			break;
		default:
			return false;
	}

	/*NSS*/
    GPIO_InitStructure.GPIO_Pin  = PIN(nss_pin_number);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PORT(nss_port_number), &GPIO_InitStructure);
	
	/*SCK*/
    GPIO_InitStructure.GPIO_Pin  = PIN(sck_pin_number);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PORT(sck_port_number), &GPIO_InitStructure);
	
	/*MISO*/
    GPIO_InitStructure.GPIO_Pin  = PIN(miso_pin_number);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PORT(miso_port_number), &GPIO_InitStructure);
	
	/*MOSI*/
    GPIO_InitStructure.GPIO_Pin  = PIN(mosi_pin_number);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PORT(mosi_port_number), &GPIO_InitStructure);
	
	/*SPI配置*/
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	/*通信方式*/
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		/*主从选择*/
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;	/*数据长度*/
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;							/*时钟极型选择*/
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;						/*时钟边沿选择*/
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			/*NSS软件控制/硬件控制*/
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;	/*设置波特率*/
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	/*大端/小端模式*/
    SPI_InitStructure.SPI_CRCPolynomial = 7;			/*CRC校验多项式*/
    
	switch(spi_number)
	{
		case _SPI1_: 
			SPI_Init(SPI1,&SPI_InitStructure);
			SPI_Cmd(SPI1,ENABLE);
			break;
		case _SPI2_: 
			SPI_Init(SPI2,&SPI_InitStructure);
			SPI_Cmd(SPI2,ENABLE);
			break;
		case _SPI3_: 
			SPI_Init(SPI3,&SPI_InitStructure);
			SPI_Cmd(SPI3,ENABLE);
			break;
		default:
			return false;
	}
	
	return true;
}

/* 操作
  ----------------------------------------------------------------------*/

/* SPI接口NSS独立控制
  -----------------------------------------
  入口：SPI模块编号，NSS状态
  返回值：成功true，失败false
*/
bool BSP_SpiNssSet(BSP_PORT_ENUM nss_port_number, BSP_PIN_ENUM nss_pin_number, \
					BSP_SPI_NSS_STATUS_ENUM status)
{
	GPIO_WriteBit(PORT(nss_port_number), PIN(nss_pin_number), (BitAction)status);
    return true;
}

/* SPI接口发送数据
  -----------------------------------------
  入口：SPI模块编号，数据值
  返回值：成功true，失败false
*/
bool BSP_SpiSendData(BSP_SPI_ENUM spi_number, uint16_t data)
{
	switch(spi_number)
	{
		case _SPI1_:
			SPI_I2S_SendData(SPI1, data);
			break;
		case _SPI2_:
			SPI_I2S_SendData(SPI2, data);
			break;
		case _SPI3_:
			SPI_I2S_SendData(SPI3, data);
			break;
		default:
			return false;
	}
    return true;
}

/* SPI接口接收数据
  -----------------------------------------
  入口：SPI模块编号
  返回值：接收到的数据
*/
uint16_t BSP_SpiGetData(BSP_SPI_ENUM spi_number)
{
	uint16_t data = 0;
	uint8_t  retry = 0;	
	
	while(BSP_SpiGetStatus(spi_number, TXE_STATUS) == false)
	{
		retry++;
		if(retry > 200)
			return 0;
	}
	BSP_SpiSendData(spi_number, 0xFFFF);
	retry = 0;
	while(BSP_SpiGetStatus(spi_number, RXNE_STATUS) == false)
	{
		retry++;
		if(retry > 200)
			return 0;
	}
	switch(spi_number)
	{
		case _SPI1_:
			data = SPI_I2S_ReceiveData(SPI1);
			break;
		case _SPI2_:
			data = SPI_I2S_ReceiveData(SPI2);
			break;
		case _SPI3_:
			data = SPI_I2S_ReceiveData(SPI3);
			break;
		default:
			return 0;
	}
	
	return data;
}

/* SPI接口状态读取
  -----------------------------------------
  入口：SPI模块编号，要读取的状态类型
  返回值：true 完成，false 未完成
*/
bool BSP_SpiGetStatus(BSP_SPI_ENUM spi_number, BSP_SPI_STATUS_TYPE_ENUM type)
{
	bool status = 0;
	
	switch(type)
	{
		case TXE_STATUS:
			switch(spi_number)
			{
				case _SPI1_:
					status = SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE);
					break;
				case _SPI2_:
					status = SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE);
					break;
				case _SPI3_:
					status = SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE);
					break;
				default:
					return false;
			}
			break;
		case RXNE_STATUS:
			switch(spi_number)
			{
				case _SPI1_:
					status = SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE);
					break;
				case _SPI2_:
					status = SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE);
					break;
				case _SPI3_:
					status = SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE);
					break;
				default:
					return false;
			}
			break;
		default:
			return false;
	}
	return status;
}
