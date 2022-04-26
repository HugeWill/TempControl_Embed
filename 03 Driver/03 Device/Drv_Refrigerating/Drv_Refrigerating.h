/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——加热膜模块

  -------------------------------------------------------
   文 件 名   : Drv_Refrigerating.c
   版 本 号   : V1.0.0.0
   作    者   : xc
   生成日期   : 2021年05月17日
   功能描述   : 定义一种制冷控制模块。
   使用方法   ：上层模块通过调用Drv_RefrigeratingInit获得一个制冷模块
				的指针，通过调用Drv_RefrigeratingOpen开启制冷片，通过
				调用Drv_RefrigeratingClose关闭制冷片。
   依赖于     : BSP_Gpio V1.0.0.0
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#ifndef _DRV_REFRIGERATING_H_
#define _DRV_REFRIGERATING_H_

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "BSP_Gpio.h"
#include "BSP_Adc.h"


/*制冷片模块枚举*/
typedef enum{
  REFRIGERATING_1  = 0,
	REFRIGERATING_2,
	REFRIGERATING_3,
	REFRIGERATING_4,
	REFRIGERATING_5,
	REFRIGERATING_6,
	REFRIGERATING_BUTT
} DRV_REFRIGERATING_ENUM;

/*制冷片模块状态枚举*/
typedef enum{
	REFRIGERATING_UNDEFINED = 0,				/*未初始化*/
	REFRIGERATING_OPEN     = 1,					/*打开*/
	REFRIGERATING_CLOSE    = 2,					/*关闭*/
} DRV_REFRIGERATING_STATUS_ENUM;

/*制冷片模块结构体*/
typedef struct{
	/*------------------------------变量*/
	DRV_REFRIGERATING_STATUS_ENUM status;			/*该制冷片模块的状态*/
	bool pin_buffer;						/*引脚输出缓存*/
	float Vc;										/*采集电压值*/
	float Ic;										/*采集电流值*/
	/*------------------------------变常*/
	/*------------------------------常量*/
	uint16_t Rc;													/*采样电阻值 单位毫欧*/
	DRV_REFRIGERATING_ENUM id;						/*制冷片模块id*/
	BSP_ADC_ENUM adc_num;						/*Ref模块adc编号*/
	BSP_CH_ENUM adc_ch;							/*Ref模块adc通道号*/
	BSP_PORT_ENUM port_number; 				/*所控制的引脚端口号*/
	BSP_PIN_ENUM pin_number;				/*所控制的引脚编号*/
	BSP_PORT_ENUM adc_port_number; 				/*adc所控制的引脚端口号*/
	BSP_PIN_ENUM adc_pin_number;				/*adc所控制的引脚编号*/
} DRV_REFRIGERATING_TYPE;

/* 接口函数
 ********************************************************/

/* 初始化制冷片模块
  -----------------------------
  入口：制冷片的ID号，IO端口
  返回值：初始化好的制冷片模块结构体
*/
extern DRV_REFRIGERATING_TYPE* Drv_RefrigeratingInit(DRV_REFRIGERATING_ENUM id, 						\
								  BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number,												\
									BSP_ADC_ENUM adc_num, 																										\
									BSP_PORT_ENUM adc_port_number, BSP_PIN_ENUM adc_pin_number, uint16_t Rc);

/* 打开制冷片
  -----------------------------
  入口：制冷片结构体
*/
extern void Drv_RefrigeratingOpen(DRV_REFRIGERATING_TYPE* p_unit);

/* 关闭制冷片
  -----------------------------
 入口：制冷片结构体
*/
extern void Drv_RefrigeratingClose(DRV_REFRIGERATING_TYPE* p_unit);

/* 判断加热膜是否开启
  -----------------------------
  入口：加热膜结构体
  返回值：如果加热膜开启则返回true
*/
extern bool Drv_RefrigeratingIsOpen(DRV_REFRIGERATING_TYPE* p_unit);

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
static bool Drv_RefAdcInit(BSP_ADC_ENUM adc_number,BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number);

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

extern void Drv_RefChaAndCalLoop(void);
#endif


