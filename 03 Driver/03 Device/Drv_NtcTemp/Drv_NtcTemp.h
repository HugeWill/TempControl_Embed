/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——加热膜模块

  -------------------------------------------------------
   文 件 名   : Drv_NtcTemp.c
   版 本 号   : V1.0.0.0
   作    者   : xsh
   生成日期   : 2022年04月12日
   功能描述   : 定义一种NTC温度采集模块。
   使用方法   ：上层模块通过调用Drv_NtcTempInit获得一个制冷模块
				的指针，通过调用Drv_NtcChaAndCalLoop计算相关参数，通过
				调用Drv_GetNtcTemp获取计算的温度。
								通过修改VERSION定义的值跟改不同版本。
   依赖于     : BSP_Gpio V1.0.0.0
								BSP_Adc V1.0.0.0
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/
#ifndef __DRV_NTCTEMP_H
#define __DRV_NTCTEMP_H

#include "BSP_Adc.h"
#include "stdlib.h"
#include "math.h"

#define VERSION 1

#if VERSION == 0 		/*XC300*/
#define XC300 
#elif VERSION == 1	/*XC8002C*/
#define XC8002C
#endif

/*NTC模块枚举*/
typedef enum
{
	NTC_1 = 0,
	NTC_2,
	NTC_3,
	NTC_4,
	NTC_5,
	NTC_6,
	NTC_7,
	NTC_8,
	NTC_9,
	NTC_10,
	NTC_BUTT,
}DRV_NTC_ENUM;

/*NTC模块状态枚举*/
typedef enum{
	NTC_UNDEFINED = 0,				/*未初始化*/
	NTC_OPEN     = 1,					/*打开*/
	NTC_CLOSE    = 2,					/*关闭*/
} DRV_NTC_STATUS_ENUM;

/*NTC模块结构体*/
typedef struct
{
	/*----------变量------------*/
	DRV_NTC_STATUS_ENUM status;										/*NTC模块状态  0表示关闭 1表示开启*/
	uint32_t adc_value;   						/*adc原始值*/
	double temp; 											/*温度*/
	double R;													/*NTC电阻值 单位K*/
	
	/*---------常量------------*/
	double Rd;											/*与NTC相接的分压电阻 单位K*/
	double Rp;                      /*NTC常温(25°C)阻值 单位K*/
	double Bx;											/*NTC重要参数*/
	double T2;											/*NTC常温下的开尔文温度*/
	double Ka;
	double V;												/*NTC电路中总电压值*/
	double Am;											/*NTC电路中恒流电流值 单位mA*/
	DRV_NTC_ENUM id;								/*NTC模块id*/
	BSP_ADC_ENUM adc_num;						/*NTC模块adc编号*/
	BSP_CH_ENUM adc_ch;							/*NTC模块adc通道号*/
	BSP_PORT_ENUM port_number;			/*所控制的引脚端口号*/
	BSP_PIN_ENUM pin_number;				/*所控制的引脚编号*/
}DRV_NTC_TYPE;

/*
描述：
		初始化NTC模块
函数名：
		Drv_NtcTempInit
参数：
		id					：模块ID号
		adc_number	：模块ADC号
		port_number ：模块端口号
		pin_number	：模块引脚号
		Rd					：模块电路下分压电阻值
		Rp					：模块NTC常温下电阻值
		Bx					：模块NTC重要参数值
		V						：模块电路下总电压值
返回值：DRV_NTC_TYPE指针
*/

DRV_NTC_TYPE *Drv_NtcTempInit(DRV_NTC_ENUM id, BSP_ADC_ENUM adc_number, BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number, \
														double Rd, double Rp, double Bx,double V, double Am);

/*
描述：
		返回NTC模块计算出的温度值
函数名：
		Drv_GetNtcTemp
参数：
		p_unit：DRV_NTC_TYPE指针
返回值：模块的温度值
*/
double Drv_GetNtcTemp(DRV_NTC_TYPE *p_unit);

/*
描述：
		返回NTC模块计算出的ADC值
函数名：
		Drv_GetNtcAdc
参数：
		p_unit：DRV_NTC_TYPE指针
返回值：模块的ADC
*/
uint32_t Drv_GetNtcAdc(DRV_NTC_TYPE *p_unit);

/*
描述：
		返回NTC模块计算出的电阻值
函数名：
		Drv_GetNtcR
参数：
		p_unit：DRV_NTC_TYPE指针
返回值：模块NTC电阻值
*/
double Drv_GetNtcR(DRV_NTC_TYPE *p_unit);

/*
描述：
		开启NTC模块采集计算
函数名：
		Drv_OpenNtc
参数：
		p_unit：DRV_NTC_TYPE指针
返回值：无
*/
void Drv_OpenNtc(DRV_NTC_TYPE *p_unit);

/*
描述：
		关闭NTC模块采集计算
函数名：
		Drv_CloseNtc
参数：
		p_unit：DRV_NTC_TYPE指针
返回值：无
*/
void Drv_CloseNtc(DRV_NTC_TYPE *p_unit);

/*
描述：
		判断NTC模块是否打开采集计算
函数名：
		Drv_NtcIsOpen
参数：
		p_unit：DRV_NTC_TYPE指针
返回值：true or false
*/
bool Drv_NtcIsOpen(DRV_NTC_TYPE *p_unit);

/*-------------------------------------静态函数-------------------------------------------------*/
/*
描述：
		NTC模块硬件初始化
函数名：
		Drv_NtcAdcInit
参数：
		adc_number	：模块ADC号
		port_number ：模块端口号
		pin_number	：模块引脚号
返回值：true or false
*/
static bool Drv_NtcAdcInit(BSP_ADC_ENUM adc_number,BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number);

/*
描述：
		判断ADC时钟是否开启 应属于BSP
函数名：
		Drv_AdcIsEnable
参数：
		adc_number	：模块ADC号
返回值：true or false
*/
static bool Drv_AdcIsEnable(BSP_ADC_ENUM adc_number);

/*
描述：
		采集并且计算模块变量参数
函数名：
		Drv_NtcChaAndCalLoop
参数：
		无
返回值：无
*/
extern void Drv_NtcChaAndCalLoop(void);

#endif

