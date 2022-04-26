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

#include "Drv_NtcTemp.h"

DRV_NTC_TYPE *_gp_NTCUnits[NTC_BUTT] = {0};	/*NTC的注册表*/

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
		Am					: 模块电路下恒流电流值
返回值：DRV_NTC_TYPE指针
*/

DRV_NTC_TYPE *Drv_NtcTempInit(DRV_NTC_ENUM id, BSP_ADC_ENUM adc_number, BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number, \
														double Rd, double Rp, double Bx,double V, double Am)
{
	DRV_NTC_TYPE* p_unit = (DRV_NTC_TYPE*)calloc(1, sizeof(DRV_NTC_TYPE));	/*申请内存*/
	
	/*分配失败返回NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id错误返回NULL*/
	if(id >= NTC_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*如果当前模块存在，取消分配新存储区*/
	if(_gp_NTCUnits[id] != NULL)
	{
		free(p_unit);
		p_unit = _gp_NTCUnits[id];
	}
	else
	{
		_gp_NTCUnits[id] = p_unit;
	}
	
	/*判读端口号的ADC通道*/
	switch(port_number)
	{
		case _PA_:
		{	
			if(pin_number >= _P0_ && pin_number <= _P7_)
			{
				p_unit->adc_ch = (BSP_CH_ENUM)pin_number;  /*计算ADC通道号*/
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
			if(pin_number >= _P0_ && pin_number <= _P1_)
			{
				p_unit->adc_ch = (BSP_CH_ENUM)(pin_number + 8); /*计算ADC通道号*/
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
			if(pin_number >= _P0_ && pin_number <= _P5_)
			{
				p_unit->adc_ch = (BSP_CH_ENUM)(pin_number + 10); /*计算ADC通道号*/
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
	
	p_unit->id = id;
	p_unit->adc_num = adc_number;
	p_unit->port_number = port_number;				/*引脚端口号*/
	p_unit->pin_number  = pin_number;					/*引脚序号*/
	
	p_unit->Rd = Rd;													/*与NTC相接的分电阻 单位K*/
	p_unit->Rp = Rp;													/*NTC常温(25°C)阻值 单位K*/
	p_unit->Bx = Bx;													/*NTC重要参数*/
	p_unit->V	 = V;
	p_unit->Am = Am;
	p_unit->T2 = 273.15 + 25.0; 							/*NTC常温下的开尔文温度*/
	p_unit->Ka = 273.15;
	
	if(Drv_NtcAdcInit(adc_number, port_number, pin_number) != true)
	{
		free(p_unit);
		return NULL;
	}
	/*------------初始化默认参数-----------*/
	p_unit->status    = NTC_OPEN;        /*默认开启NTC采集计算*/
	p_unit->temp 	    = 0.0f;
	p_unit->adc_value = 0;	
	p_unit->R 				= 0;	
	return p_unit;
}

/*
描述：
		返回NTC模块计算出的温度值
函数名：
		Drv_GetNtcTemp
参数：
		p_unit：DRV_NTC_TYPE指针
返回值：模块的温度值
*/
double Drv_GetNtcTemp(DRV_NTC_TYPE *p_unit)
{
	return p_unit->temp;
}

/*
描述：
		返回NTC模块计算出的ADC值
函数名：
		Drv_GetNtcAdc
参数：
		p_unit：DRV_NTC_TYPE指针
返回值：模块的ADC
*/
uint32_t Drv_GetNtcAdc(DRV_NTC_TYPE *p_unit)
{
	return p_unit->adc_value;
}

/*
描述：
		返回NTC模块计算出的电阻值
函数名：
		Drv_GetNtcR
参数：
		p_unit：DRV_NTC_TYPE指针
返回值：模块NTC电阻值
*/
double Drv_GetNtcR(DRV_NTC_TYPE *p_unit)
{
	return p_unit->R;
}

/*
描述：
		开启NTC模块采集计算
函数名：
		Drv_OpenNtc
参数：
		p_unit：DRV_NTC_TYPE指针
返回值：无
*/
void Drv_OpenNtc(DRV_NTC_TYPE *p_unit)
{
	p_unit->status = NTC_OPEN;
}

/*
描述：
		关闭NTC模块采集计算
函数名：
		Drv_CloseNtc
参数：
		p_unit：DRV_NTC_TYPE指针
返回值：无
*/
void Drv_CloseNtc(DRV_NTC_TYPE *p_unit)
{
	p_unit->status = NTC_CLOSE;
}

/*
描述：
		判断NTC模块是否打开采集计算
函数名：
		Drv_NtcIsOpen
参数：
		p_unit：DRV_NTC_TYPE指针
返回值：true or false
*/
bool Drv_NtcIsOpen(DRV_NTC_TYPE *p_unit)
{
	if(p_unit->status == NTC_OPEN)
		return true;
	else
		return false;
}


/*
描述：
		采集并且计算模块变量参数
函数名：
		Drv_NtcChaAndCalLoop
参数：
		无
返回值：无
*/
void Drv_NtcChaAndCalLoop(void)
{
	uint8_t i;
	for(i = 0; i < NTC_BUTT; i++)
	{
		if(_gp_NTCUnits[i] != NULL)
//		if(_gp_NTCUnits[i]->status == NTC_OPEN) /*判断是否开启采样计算*/
		{
			#if defined XC300
			
			_gp_NTCUnits[i]->adc_value = BSP_ReadAdc(_gp_NTCUnits[i]->adc_num,_gp_NTCUnits[i]->adc_ch);
			_gp_NTCUnits[i]->temp = 3.3 * _gp_NTCUnits[i]->adc_value / 4096.0; /*计算NTC电压值*/
			_gp_NTCUnits[i]->R = (_gp_NTCUnits[i]->adc_value * _gp_NTCUnits[i]->Rd) / (_gp_NTCUnits[i]->V - _gp_NTCUnits[i]->adc_value);
			_gp_NTCUnits[i]->temp =_gp_NTCUnits[i]->R / _gp_NTCUnits[i]->Rp;
			_gp_NTCUnits[i]->temp = log(_gp_NTCUnits[i]->temp);
			_gp_NTCUnits[i]->temp /= _gp_NTCUnits[i]->Bx;
			_gp_NTCUnits[i]->temp += (1/_gp_NTCUnits[i]->T2);
			_gp_NTCUnits[i]->temp = 1/_gp_NTCUnits[i]->temp;
			_gp_NTCUnits[i]->temp -= _gp_NTCUnits[i]->Ka;
			
			#elif defined XC8002C
			_gp_NTCUnits[i]->adc_value = BSP_ReadAdc(_gp_NTCUnits[i]->adc_num,_gp_NTCUnits[i]->adc_ch);
			_gp_NTCUnits[i]->temp = 2 * 3.3 * _gp_NTCUnits[i]->adc_value / 4096.0; /*计算NTC电压值*/
			_gp_NTCUnits[i]->R = _gp_NTCUnits[i]->temp / _gp_NTCUnits[i]->Am;
			_gp_NTCUnits[i]->temp =_gp_NTCUnits[i]->R / _gp_NTCUnits[i]->Rp;
			_gp_NTCUnits[i]->temp = log(_gp_NTCUnits[i]->temp);
			_gp_NTCUnits[i]->temp /= _gp_NTCUnits[i]->Bx;
			_gp_NTCUnits[i]->temp += (1/_gp_NTCUnits[i]->T2);
			_gp_NTCUnits[i]->temp = 1/_gp_NTCUnits[i]->temp;
			_gp_NTCUnits[i]->temp -= _gp_NTCUnits[i]->Ka;
			#endif
			
		}
	}
}

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
static bool Drv_NtcAdcInit(BSP_ADC_ENUM adc_number,BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number)
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

