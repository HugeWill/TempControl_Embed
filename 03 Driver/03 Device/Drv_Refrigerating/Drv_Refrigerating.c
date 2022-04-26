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
				的指针，通过调用Drv_Refrigerating_Open开启制冷片，通过
				调用Drv_Refrigerating_Close关闭制冷片。
   依赖于     : BSP_Gpio V1.0.0.0
   注         ：该模块不依赖于硬件电路。
	 
	 文 件 名   : Drv_Refrigerating.c
   版 本 号   : V1.0.1.0
   作    者   : xsh
   生成日期   : 2022年04月13日
   功能描述   : 定义一种制冷控制模块。
   依赖于     : BSP_Gpio V1.0.0.0
								BSP_Adc V1.0.0.0
   注         :增加电流采集
 ********************************************************/

#include "Drv_Refrigerating.h"

DRV_REFRIGERATING_TYPE* _gp_RefrigeratingUnits[REFRIGERATING_BUTT] = {0};	/*制冷片的注册表*/

/* 接口函数
 ********************************************************/

/* 初始化制冷片模块
  -----------------------------
  入口：制冷片的ID号，IO端口，ADC号，ADC IO端口， 采样电阻 mR
  返回值：初始化好的制冷片模块结构体
*/
DRV_REFRIGERATING_TYPE* Drv_RefrigeratingInit(DRV_REFRIGERATING_ENUM id, 						\
								  BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number,												\
									BSP_ADC_ENUM adc_num,																	\
									BSP_PORT_ENUM adc_port_number, BSP_PIN_ENUM adc_pin_number, uint16_t Rc)
{	
	DRV_REFRIGERATING_TYPE* p_unit = (DRV_REFRIGERATING_TYPE*)calloc(1, sizeof(DRV_REFRIGERATING_TYPE));	/*申请内存*/
	
	/*分配失败返回NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id错误返回NULL*/
	if(id >= REFRIGERATING_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*如果当前模块存在，取消分配新存储区*/
	if(_gp_RefrigeratingUnits[id] != NULL)
	{
		free(p_unit);
		p_unit = _gp_RefrigeratingUnits[id];
	}
	else
	{
		_gp_RefrigeratingUnits[id] = p_unit;
	}
	
	/*判读端口号的ADC通道*/
	switch(adc_port_number)
	{
		case _PA_:
		{	
			if(adc_pin_number >= _P0_ && adc_pin_number <= _P7_)
			{
				p_unit->adc_ch = (BSP_CH_ENUM)adc_pin_number;  /*计算ADC通道号*/
				break;
			}
			else
			{
				free(p_unit);
				return NULL;
			}
		}
		case _PB_:
		{
			if(adc_pin_number >= _P0_ && adc_pin_number <= _P1_)
			{
				p_unit->adc_ch = (BSP_CH_ENUM)(adc_pin_number + 8); /*计算ADC通道号*/
				break;
			}
			else
			{
				free(p_unit);
				return NULL;
			}
		}
		case _PC_:
		{
			if(adc_pin_number >= _P0_ && adc_pin_number <= _P5_)
			{
				p_unit->adc_ch = (BSP_CH_ENUM)(adc_pin_number + 10); /*计算ADC通道号*/
				break;
			}
			else
			{
				free(p_unit);
				return NULL;
			}
		}
		default: return NULL;
	}
	
	/*初始化*/
	
	/*-------------------------------常量*/
	p_unit->id          = id;						/*模块id*/
	p_unit->adc_num = adc_num;					/*Ref模块adc编号*/
	p_unit->Rc = Rc;										/*Ref模块采样电阻*/
	p_unit->port_number = port_number;				/*引脚端口号*/
	p_unit->pin_number  = pin_number;				/*引脚序号*/
	p_unit->adc_port_number = adc_port_number;				/*引脚端口号*/
	p_unit->adc_pin_number  = adc_pin_number;				/*引脚序号*/
	
	if(Drv_RefAdcInit(adc_num, adc_port_number, adc_pin_number) != true)
	{
		free(p_unit);
		return NULL;
	}
	BSP_Init_Pin(port_number, pin_number, _OUT_PP_);	/*初始化硬件*/
	
	/*-------------------------------变量*/
	p_unit->status = REFRIGERATING_CLOSE;	/*全闭*/
	p_unit->Vc = 0;
	p_unit->Ic = 0;
	
	p_unit->pin_buffer = 0;							/*初始化为高电平，关闭制冷片*/
	BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	
	return p_unit;
}

/* 打开制冷片
  -----------------------------
  入口：制冷片结构体
*/
void  Drv_RefrigeratingOpen(DRV_REFRIGERATING_TYPE* p_unit)
{
	p_unit->pin_buffer = 1;
	BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	p_unit->status = REFRIGERATING_OPEN;
}

/* 关闭制冷片
  -----------------------------
 入口：制冷片结构体
*/
void Drv_RefrigeratingClose(DRV_REFRIGERATING_TYPE* p_unit)
{
	p_unit->pin_buffer = 0;
	BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	p_unit->status =REFRIGERATING_CLOSE;
}

/* 判断制冷片是否开启
  -----------------------------
  入口：制冷片结构体
  返回值：如果制冷片开启则返回true
*/
bool Drv_RefrigeratingIsOpen(DRV_REFRIGERATING_TYPE* p_unit)
{
	if(p_unit->status == REFRIGERATING_OPEN)
		return true;
	else
		return false;
}

/*
描述：
		采集并且计算模块变量参数
函数名：
		Drv_RefChaAndCalLoop
参数：
		无
返回值：无
*/
void Drv_RefChaAndCalLoop(void)
{
	uint8_t i;
	for(i = 0; i < REFRIGERATING_BUTT; i++)
	{
		if(_gp_RefrigeratingUnits[i]->status == REFRIGERATING_OPEN) /*判断是否开启采样计算*/
		{
			_gp_RefrigeratingUnits[i]->Vc = BSP_ReadAdc(_gp_RefrigeratingUnits[i]->adc_num,_gp_RefrigeratingUnits[i]->adc_ch);
			_gp_RefrigeratingUnits[i]->Vc = (3.3 * _gp_RefrigeratingUnits[i]->Vc / 4096.0)/50;
			_gp_RefrigeratingUnits[i]->Ic = (_gp_RefrigeratingUnits[i]->Vc / _gp_RefrigeratingUnits[i]->Rc) * 1000 ;
		}
	}
}


/*-------------------------------------静态函数-------------------------------------------------*/
/*
描述：
		Ref模块硬件初始化
函数名：
		Drv_NtcAdcInit
参数：
		adc_number	：模块ADC号
		port_number ：模块端口号
		pin_number	：模块引脚号
返回值：true or false
*/
static bool Drv_RefAdcInit(BSP_ADC_ENUM adc_number,BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number)
{
	ADC_InitTypeDef ADC_InitStructure;
	
	BSP_Init_Pin(port_number, pin_number, _IN_AIN_); /*初始化硬件*/
	
	if(Drv_AdcIsEnable(adc_number) == true)
	{
		return true;
	}
	
	/*初始化ADC*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;			/*禁用通道扫描*/
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		/*禁用连续转换*/
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	/*不使用外部触发转换*/
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	/*采集数据右对齐*/
	ADC_InitStructure.ADC_NbrOfChannel = 1;					/*转换通道数1*/
	
	switch(adc_number)
	{
		case _ADC1_:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
			ADC_Init(ADC1, &ADC_InitStructure);
			
			RCC_ADCCLKConfig(RCC_PCLK2_Div8);			/*配置ADC时钟*/
			
			ADC_Cmd(ADC1, ENABLE);						/*使能ADC*/
			
			ADC_ResetCalibration(ADC1);					/*复位校准寄存器*/
			while(ADC_GetResetCalibrationStatus(ADC1));	/*等待校准寄存器复位完成*/
			
			ADC_StartCalibration(ADC1);					/*ADC校准*/
			while(ADC_GetCalibrationStatus(ADC1));		/*等待校准完成*/
			
			break;
			
		case _ADC2_:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
			ADC_Init(ADC2, &ADC_InitStructure);
			
			RCC_ADCCLKConfig(RCC_PCLK2_Div8);			/*配置ADC时钟*/
			
			ADC_Cmd(ADC2, ENABLE);						/*使能ADC*/
			
			ADC_ResetCalibration(ADC2);					/*复位校准寄存器*/
			while(ADC_GetResetCalibrationStatus(ADC2));	/*等待校准寄存器复位完成*/
			
			ADC_StartCalibration(ADC2);					/*ADC校准*/
			while(ADC_GetCalibrationStatus(ADC2));		/*等待校准完成*/
			
			break;
			
		case _ADC3_:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
			ADC_Init(ADC3, &ADC_InitStructure);
			
			RCC_ADCCLKConfig(RCC_PCLK2_Div8);			/*配置ADC时钟*/
			
			ADC_Cmd(ADC3, ENABLE);						/*使能ADC*/
			
			ADC_ResetCalibration(ADC3);					/*复位校准寄存器*/
			while(ADC_GetResetCalibrationStatus(ADC3));	/*等待校准寄存器复位完成*/
			
			ADC_StartCalibration(ADC3);					/*ADC校准*/
			while(ADC_GetCalibrationStatus(ADC3));		/*等待校准完成*/
			
			break;
			
		default:
			return false;		
	}
	return true;
}

/*
描述：
		判断ADC时钟是否开启 应属于BSP
函数名：
		Drv_AdcIsEnable
参数：
		adc_number	：模块ADC号
返回值：true or false
*/
static bool Drv_AdcIsEnable(BSP_ADC_ENUM adc_number)
{
	switch(adc_number)
	{
		case _ADC1_:
			if((RCC->APB2ENR & RCC_APB2ENR_ADC1EN) == 0)
			{
				return false;
			}
			break;
		case _ADC2_:
			if((RCC->APB2ENR & RCC_APB2ENR_ADC2EN) == 0)
			{
				return false;
			}
			break;
		case _ADC3_:
			if((RCC->APB2ENR & RCC_APB2ENR_ADC3EN) == 0)
			{
				return false;
			}
			break;
		default:return false;
	}
	return true;
}

