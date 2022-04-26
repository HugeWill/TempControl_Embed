/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——BSP层 DAC封装

  -------------------------------------------------------
   文 件 名   : BSP_Dac.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2020年05月08日
   功能描述   : 定义一种基于STM32的板级支持包。
				该层依赖于硬件电路。
   依赖于     : STM32F10x_StdPeriph_Lib_V3.5.0
 ********************************************************/
 
#include "BSP_Dac.h"

/* 初始化
  ----------------------------------------------------------------------*/

/* 初始化DAC1
  -----------------------------------------
  返回值：成功true，失败false
*/
bool BSP_InitDac1(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitType;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	//使能PORTA通道时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );	//使能DAC通道时钟 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				//端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;			//模拟输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);							//PA.4 输出高
                    
    DAC_InitType.DAC_Trigger=DAC_Trigger_None;				//不使用触发功能 TEN1=0
    DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;//不使用波形发生
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;//屏蔽、幅值设置
    DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;    //DAC1输出缓存关闭 BOFF1=1
    DAC_Init(DAC_Channel_1,&DAC_InitType);					//初始化DAC通道1

    DAC_Cmd(DAC_Channel_1, ENABLE);							//使能DAC1
  
    DAC_SetChannel1Data(DAC_Align_12b_R, 0);				//12位右对齐数据格式设置DAC值
	
	return true;
}

/* 操作
  ----------------------------------------------------------------------*/

/* DAC1输出电压值
  -----------------------------------------
  入口：电压值，0.0~3.0V
  返回值：高电平true，低电平false
*/
void BSP_SetDac1(double vol)
{
    float ftemp = 0;
	
	if(vol > 3.0)
		ftemp = 3.0;
	else if(vol < 0.0)
		ftemp = 0.0;
	else
		ftemp = vol;
		
    ftemp = ftemp * 4096.0 / 3.0;
	
    DAC_SetChannel1Data(DAC_Align_12b_R, (uint16_t)ftemp);	//12位右对齐数据格式设置DAC值
}
