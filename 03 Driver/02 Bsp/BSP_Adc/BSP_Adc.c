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

/* 初始化
  ----------------------------------------------------------------------*/

/* 初始化ADC
  -----------------------------------------
  入口：ADC编号，通道号
  返回值：成功true，失败false
*/
bool BSP_Init_Adc(BSP_ADC_ENUM adc_number, BSP_CH_ENUM ch1_number, BSP_CH_ENUM ch2_number, BSP_CH_ENUM ch3_number, BSP_CH_ENUM ch4_number, BSP_CH_ENUM ch5_number, BSP_CH_ENUM ch6_number, BSP_CH_ENUM ch7_number, BSP_CH_ENUM ch8_number)
{
	ADC_InitTypeDef ADC_InitStructure;

	/*初始化引脚*/
	if((ch1_number >= _CH0_) && (ch1_number <= _CH7_))	/*在PORTA*/
	{
		if((adc_number >= _ADC1_) && (adc_number < _ADC_BUTT_))
		{
			BSP_Init_Pin(_PA_, (BSP_PIN_ENUM)ch1_number, _IN_AIN_);
		}
		else
		{
			return false;
		}
	}
	else if((ch1_number >= _CH10_) && (ch1_number <= _CH15_))	/*在PORTC*/
	{
		if((adc_number >= _ADC1_) && (adc_number < _ADC_BUTT_))
		{
			BSP_Init_Pin(_PC_, (BSP_PIN_ENUM)ch1_number, _IN_AIN_);
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	
	if((ch2_number >= _CH0_) && (ch2_number <= _CH7_))	/*在PORTA*/
	{
		if((adc_number >= _ADC1_) && (adc_number < _ADC_BUTT_))
		{
			BSP_Init_Pin(_PA_, (BSP_PIN_ENUM)ch2_number, _IN_AIN_);
		}
		else
		{
			return false;
		}
	}
	else if((ch2_number >= _CH10_) && (ch2_number <= _CH15_))	/*在PORTC*/
	{
		if((adc_number >= _ADC1_) && (adc_number < _ADC_BUTT_))
		{
			BSP_Init_Pin(_PC_, (BSP_PIN_ENUM)ch2_number, _IN_AIN_);
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	
	if((ch3_number >= _CH0_) && (ch3_number <= _CH7_))	/*在PORTA*/
	{
		if((adc_number >= _ADC1_) && (adc_number < _ADC_BUTT_))
		{
			BSP_Init_Pin(_PA_, (BSP_PIN_ENUM)ch3_number, _IN_AIN_);
		}
		else
		{
			return false;
		}
	}
	else if((ch3_number >= _CH10_) && (ch3_number <= _CH15_))	/*在PORTC*/
	{
		if((adc_number >= _ADC1_) && (adc_number < _ADC_BUTT_))
		{
			BSP_Init_Pin(_PC_, (BSP_PIN_ENUM)ch3_number, _IN_AIN_);
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	
	if((ch4_number >= _CH0_) && (ch4_number <= _CH7_))	/*在PORTA*/
	{
		if((adc_number >= _ADC1_) && (adc_number < _ADC_BUTT_))
		{
			BSP_Init_Pin(_PA_, (BSP_PIN_ENUM)ch4_number, _IN_AIN_);
		}
		else
		{
			return false;
		}
	}
	else if((ch4_number >= _CH10_) && (ch4_number <= _CH15_))	/*在PORTC*/
	{
		if((adc_number >= _ADC1_) && (adc_number < _ADC_BUTT_))
		{
			BSP_Init_Pin(_PC_, (BSP_PIN_ENUM)ch4_number, _IN_AIN_);
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	
	if((ch5_number >= _CH0_) && (ch5_number <= _CH7_))	/*在PORTA*/
	{
		if((adc_number >= _ADC1_) && (adc_number < _ADC_BUTT_))
		{
			BSP_Init_Pin(_PA_, (BSP_PIN_ENUM)ch5_number, _IN_AIN_);
		}
		else
		{
			return false;
		}
	}
	else if((ch5_number >= _CH10_) && (ch5_number <= _CH15_))	/*在PORTC*/
	{
		if((adc_number >= _ADC1_) && (adc_number < _ADC_BUTT_))
		{
			BSP_Init_Pin(_PC_, (BSP_PIN_ENUM)ch5_number, _IN_AIN_);
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	
	if((ch6_number >= _CH0_) && (ch6_number <= _CH7_))	/*在PORTA*/
	{
		if((adc_number >= _ADC1_) && (adc_number < _ADC_BUTT_))
		{
			BSP_Init_Pin(_PA_, (BSP_PIN_ENUM)ch6_number, _IN_AIN_);
		}
		else
		{
			return false;
		}
	}
	else if((ch6_number >= _CH10_) && (ch6_number <= _CH15_))	/*在PORTC*/
	{
		if((adc_number >= _ADC1_) && (adc_number < _ADC_BUTT_))
		{
			BSP_Init_Pin(_PC_, (BSP_PIN_ENUM)ch6_number, _IN_AIN_);
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
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
}

/* 操作
  ----------------------------------------------------------------------*/

/* 读AD值
  -----------------------------------------
  入口：ADC编号，通道号
  返回值：高电平true，低电平false
*/
uint32_t BSP_ReadAdc(BSP_ADC_ENUM adc_number, BSP_CH_ENUM ch_number)
{
	switch(adc_number)
	{
		case _ADC1_:
			ADC_RegularChannelConfig(ADC1, (uint8_t)ch_number, 1, ADC_SampleTime_7Cycles5);	/*配置通道和采样周期*/
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);			/*软件触发ADC转换*/
			while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));	/*等待转换完成*/
			return ADC_GetConversionValue(ADC1);
		
		case _ADC2_:
			ADC_RegularChannelConfig(ADC2, (uint8_t)ch_number, 1, ADC_SampleTime_7Cycles5);	/*配置通道和采样周期*/
			ADC_SoftwareStartConvCmd(ADC2, ENABLE);			/*软件触发ADC转换*/
			while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC));	/*等待转换完成*/
			return ADC_GetConversionValue(ADC2);
		
		case _ADC3_:
			ADC_RegularChannelConfig(ADC3, (uint8_t)ch_number, 1, ADC_SampleTime_7Cycles5);	/*配置通道和采样周期*/
			ADC_SoftwareStartConvCmd(ADC3, ENABLE);			/*软件触发ADC转换*/
			while(!ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC));	/*等待转换完成*/
			return ADC_GetConversionValue(ADC3);
		
		default:
			return 0;
	}
}
