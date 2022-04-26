/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——X100m模块

  -------------------------------------------------------
   文 件 名   : Drv_X100m.c
   版 本 号   : V1.0.0.0
   作    者   : xsh
   生成日期   : 2022年04月14日
   功能描述   : 定义一种X100m温度压力采集模块。
   使用方法   ：上层模块通过调用Drv_X100mInit获得一个制冷模块
								的指针，通过调用Drv_GetX100mTemp获取计算的温度，
								通过调用Drv_GetX100mTemp获取计算的温度，通过调用
								Drv_SetTempPara修改温度参数。
   依赖于     : BSP_Uart V1.0.0.0
								ucosii操作系统
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/
#ifndef __DRV_X100M_H
#define __DRV_X100M_H

#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "BSP_Uart.h"
#include "ucos_ii.h"

#define DTADLEN 10 /*接收数据长度*/
extern uint8_t Temp_Data[10][3];
/*X100m模块枚举*/
typedef enum
{
	X100M_1 = 0,
	X100M_BUTT,
}DRV_X100M_ENUM;

/*X100m模块接收类型枚举*/
typedef enum
{
	X100M_UNDEFINE = 0,
	X100M_TEMP,
	X100M_PRES,
}DRV_X100M_STATUS_ENUM;

/*X100m模块温度系数*/
typedef struct
{
	float a;
	float b;
	float c;
	float d;
	float e;
}DRV_X100M_PARA_TYPE;

typedef struct
{
	/*----------变量------------*/
	double temp;  												/*温度值*/
	double pres;													/*压力值*/
	uint16_t index;											/*数据压栈值*/
	uint8_t rx_buffer[DTADLEN];					/*数据缓存区*/
	OS_EVENT *x100msem;									/*接收完成信号量*/
	DRV_X100M_STATUS_ENUM status;       /*接收状态类型*/
	
	/*---------常量------------*/
	uint32_t baud_rate;									/*波特率*/
	uint16_t version;										/*版本号*/
	DRV_X100M_ENUM id;									/*ID号*/
	BSP_USART_ENUM usart;								/*串口端号*/
	DRV_X100M_PARA_TYPE temp_para;			/*温度系数值*/
	BSP_PORT_ENUM tx_port_number;			/*所控制的引脚端口号*/
	BSP_PIN_ENUM tx_pin_number;				/*所控制的引脚编号*/
	BSP_PORT_ENUM rx_port_number;			/*所控制的引脚端口号*/
	BSP_PIN_ENUM rx_pin_number;				/*所控制的引脚编号*/
}DRV_X100M_TYPE;

/*
描述：
		初始化X100m模块
函数名：
		Drv_X100mInit
参数：
		id							：模块ID号
		baud_rate				：模块波特率
		usart			  		：模块串口号
		tx_port_number	：模块引脚端口号
		tx_pin_number		：模块引脚号
		rx_port_number	：模块引脚端口号
		rx_pin_number		：模块引脚号
		a								：模块温度参数
		b								：模块温度参数
		c								：模块温度参数
		d								：模块温度参数
		e								：模块温度参数
返回值：DRV_X100M_TYPE指针
*/
DRV_X100M_TYPE *Drv_X100mInit(DRV_X100M_ENUM id, uint32_t baud_rate, BSP_USART_ENUM usart,    	\
											BSP_PORT_ENUM tx_port_number, BSP_PIN_ENUM tx_pin_number, 								\
											BSP_PORT_ENUM rx_port_number, BSP_PIN_ENUM rx_pin_number,									\
											float a, float b, float c, float d, float e);

/*
描述：
		设置模块温度参数
函数名：
		Drv_SetTempPara
参数：
		p_unit：DRV_X100M_TYPE指针
		para  ：DRV_X100M_PARA_TYPE结构体
返回值：无
*/
void Drv_SetTempPara(DRV_X100M_TYPE *p_unit, DRV_X100M_PARA_TYPE para);

/*
描述：
		返回X100m模块计算出的温度值
函数名：
		Drv_GetX100mTemp
参数：
		p_unit：DRV_X100M_TYPE指针
返回值：模块温度值
*/
double Drv_GetX100mTemp(DRV_X100M_TYPE *p_unit);

/*
描述：
		返回X100m模块计算出的压力值
函数名：
		Drv_GetX100mPres
参数：
		p_unit：DRV_X100M_TYPE指针
返回值：模块压力值
*/
double Drv_GetX100mPres(DRV_X100M_TYPE *p_unit);

/*-----------------------------静态函数-----------------------------------*/
/*
描述：
		使能X100m模块发送腔长值
函数名：
		Drv_X100m1Enable
参数：
		p_unit：DRV_X100M_TYPE指针
返回值：无
*/
static void Drv_X100m1Enable(DRV_X100M_TYPE *p_unit);

/*
描述：
		失能X100m模块发送腔长值
函数名：
		Drv_X100mDisable
参数：
		p_unit：DRV_X100M_TYPE指针
返回值：无
*/
static void Drv_X100mDisable(DRV_X100M_TYPE *p_unit);

/*
描述：
		查询模块版本号
函数名：
		Drv_X100mQueryVersion
参数：
		p_unit：DRV_X100M_TYPE指针
返回值：无
*/
static void Drv_X100mQueryVersion(DRV_X100M_TYPE *p_unit);

/*
描述：
		查询模块环境压力
函数名：
		Drv_X100mPresH
参数：
		p_unit：DRV_X100M_TYPE指针
返回值：无
*/
static void Drv_X100mPresH(DRV_X100M_TYPE *p_unit);

/*
描述：
		查询模块环境压力
函数名：
		Drv_X100mPresL
参数：
		p_unit：DRV_X100M_TYPE指针
返回值：无
*/
static void Drv_X100mPresL(DRV_X100M_TYPE *p_unit);

/*
描述：
		模块中断服务函数
函数名：
		Drv_X100m1_IRQHandler
参数：
		无
返回值：无
*/
static void Drv_X100m1_IRQHandler(void);

#endif
