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
#include "Drv_X100m.h"

static uint8_t StartCommand[4] = {0xFA, 0x07, 0x00, 0x01}; /*使模块对外发送腔长数据*/
static uint8_t StopCommand[4]  = {0xFA, 0x07, 0x00, 0x00}; /*停止模块对外发送腔长数据*/
static uint8_t QueryCommand[2] = {0xFB, 0x03};						 /*查询模块当前代码版本信息*/
static uint8_t PresCommandH[2] = {0xFB, 0x57};						 /*发送环境压力*/
static uint8_t PresCommandL[2] = {0xFB, 0x56};						 /*发送环境压力*/
uint8_t Temp_Data[50][3];
DRV_X100M_TYPE *_gp_X100mUnits[X100M_BUTT] = {0};	/*NTC的注册表*/

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
											float a, float b, float c, float d, float e)
{
	uint8_t i;
	DRV_X100M_TYPE* p_unit = (DRV_X100M_TYPE*)calloc(1, sizeof(DRV_X100M_TYPE));	/*申请内存*/
	
	/*分配失败返回NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id错误返回NULL*/
	if(id >= X100M_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*如果当前模块存在，取消分配新存储区*/
	if(_gp_X100mUnits[id] != NULL)
	{
		free(p_unit);
		p_unit = _gp_X100mUnits[id];
	}
	else
	{
		_gp_X100mUnits[id] = p_unit;
	}
	
	p_unit->id = id;
	p_unit->baud_rate = baud_rate;
	p_unit->usart = usart;
	p_unit->tx_port_number = tx_port_number;				/*引脚端口号*/
	p_unit->tx_pin_number  = tx_pin_number;					/*引脚序号*/
	p_unit->rx_port_number = rx_port_number;				/*引脚端口号*/
	p_unit->rx_pin_number  = rx_pin_number;					/*引脚序号*/
	p_unit->temp_para.a = a;
	p_unit->temp_para.b = b;
	p_unit->temp_para.c = c;
	p_unit->temp_para.d = d;
	p_unit->temp_para.e = e;
	
	BSP_Init_UsartInterrupt(usart,baud_rate,tx_port_number,tx_pin_number,rx_port_number,rx_pin_number);
	BSP_UartIRQHandler[usart] = Drv_X100m1_IRQHandler;
	
	/*------------初始化默认参数-----------*/
	p_unit->temp = 0;
	p_unit->pres = 0;
	p_unit->index = 0;
	p_unit->version = 0;
	memset(p_unit->rx_buffer,0x00,DTADLEN);
	p_unit->status = X100M_UNDEFINE;
	p_unit->x100msem = OSSemCreate(0); 
	for(i=0;i<50;i++)
	{
		memset(&Temp_Data[i][0],0,3);
	}
	return p_unit;
}

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
void Drv_SetTempPara(DRV_X100M_TYPE *p_unit, DRV_X100M_PARA_TYPE para)
{
	p_unit->temp_para.a = para.a;
	p_unit->temp_para.b = para.b;
	p_unit->temp_para.c = para.c;
	p_unit->temp_para.d = para.d;
	p_unit->temp_para.e = para.e;
}

/*
描述：
		返回X100m模块计算出的温度值
函数名：
		Drv_GetX100mTemp
参数：
		p_unit：DRV_X100M_TYPE指针
返回值：模块温度值
*/
double Drv_GetX100mTemp(DRV_X100M_TYPE *p_unit)
{
	uint8_t err;
	uint8_t i;
	uint8_t j;
	uint8_t count = 0;
	double temp[50];
	double exchange;
	p_unit->index = 0;
	memset(p_unit->rx_buffer,0x00,DTADLEN);
	for(i=0;i<50;i++)
	{
		memset(&Temp_Data[i][0],0,3);
	}
	if(p_unit->version == 0) /*没有版本号，先获取*/
	{
		Drv_X100mQueryVersion(p_unit);
		OSSemPend(p_unit->x100msem,500,&err);
		if(err != OS_ERR_NONE)
		{
			return 0;
		}
		p_unit->version = ((p_unit->rx_buffer[0]<<8) | (p_unit->rx_buffer[1]&0xFF)); /*计算版本号*/
		p_unit->index = 0;
		memset(p_unit->rx_buffer,0x00,DTADLEN);
	}
	p_unit->status = X100M_TEMP;
	Drv_X100m1Enable(p_unit);
	OSSemPend(p_unit->x100msem,3000,&err);
	Drv_X100mDisable(p_unit);
	p_unit->status = X100M_UNDEFINE;
	if(err != OS_ERR_NONE)
	{
		return 0;
	}
	
	
//	if(p_unit->index == 2)
//	{
//		p_unit->temp = ((p_unit->rx_buffer[0]<<8) | (p_unit->rx_buffer[1]&0xFF)); 
//		p_unit->temp = p_unit->temp_para.a * pow(p_unit->temp, 4) + p_unit->temp_para.b * pow(p_unit->temp, 3)\
//								 + p_unit->temp_para.c * pow(p_unit->temp, 2) + p_unit->temp_para.d * p_unit->temp			   \
//								 + p_unit->temp_para.e;/*计算温度*/
//	}
//	else if(p_unit->index == 3)
//	{
	for(i=0;i<50;i++)
	{
		if(!(Temp_Data[i][0] == 0xFF && Temp_Data[i][1] == 0xFF && Temp_Data[i][2] == 0xFF))
		{
			temp[count] = ((Temp_Data[i][0]<<8) | (Temp_Data[i][1]&0xFF)) + (Temp_Data[i][2] / 256.0); 
			temp[count] = p_unit->temp_para.a * pow(temp[count], 4) + p_unit->temp_para.b * pow(temp[count], 3)\
								 + p_unit->temp_para.c * pow(temp[count], 2) + p_unit->temp_para.d *temp[count]			  \
								 + p_unit->temp_para.e;/*计算温度*/
			count++;
		}
	}
	if(count>40)
	{
		for(j=0;j<=count;j++)
		{
			if(temp[j]!=0 && temp[j+1] != 0){
				if(temp[j] < temp[j+1])
				{
					exchange = temp[j];
					temp[j] = temp[j+1];
					temp[j] = exchange;
				}
			}
		}
		j = 0;
		p_unit->temp = 0;
		for(i=14;i<count-15;i++)
		{
			p_unit->temp = p_unit->temp + temp[i];
			j++;
		}
	}
	if(j <= 1)
		p_unit->temp = 0;		//Fault
	else
		p_unit->temp = p_unit->temp/j;
	
	return p_unit->temp;
}

/*
描述：
		返回X100m模块计算出的压力值
函数名：
		Drv_GetX100mPres
参数：
		p_unit：DRV_X100M_TYPE指针
返回值：模块压力值
*/
double Drv_GetX100mPres(DRV_X100M_TYPE *p_unit)
{
	uint8_t err;
	uint32_t buf;
	
	p_unit->index = 0;
	memset(p_unit->rx_buffer,0x00,DTADLEN);
	
	if(p_unit->version == 0) /*没有版本号，先获取*/
	{
		Drv_X100mQueryVersion(p_unit);
		OSSemPend(p_unit->x100msem,500,&err);
		if(err != OS_ERR_NONE)
		{
			return 0;
		}
		p_unit->version = ((p_unit->rx_buffer[0]<<8) | (p_unit->rx_buffer[1]&0xFF)); /*计算版本号*/
		p_unit->index = 0;
		memset(p_unit->rx_buffer,0x00,DTADLEN);
	}
	
	Drv_X100mPresH(p_unit);
	OSSemPend(p_unit->x100msem,500,&err);
	if(err != OS_ERR_NONE)
	{
		return 0;
	}
	if(p_unit->index == 2)
	{
		buf = ((p_unit->rx_buffer[0]<<8) | (p_unit->rx_buffer[1]&0xFF));
	}
	else
	{
		return 0;
	}
	
	Drv_X100mPresL(p_unit);
	OSSemPend(p_unit->x100msem,500,&err);
	if(err != OS_ERR_NONE)
	{
		return 0;
	}
	if(p_unit->index == 2)
	{
		buf = (buf << 16) | ((p_unit->rx_buffer[0]<<8) | (p_unit->rx_buffer[1]&0xFF));
		p_unit->pres = buf / 4.0; /*计算压力*/
	}
	else
	{
		return 0;
	}
	
	return p_unit->pres;
}

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
static void Drv_X100m1Enable(DRV_X100M_TYPE *p_unit)
{
	BSP_UsartSendByte(p_unit->usart,StartCommand[0]);
	BSP_UsartSendByte(p_unit->usart,StartCommand[1]);
	BSP_UsartSendByte(p_unit->usart,StartCommand[2]);
	BSP_UsartSendByte(p_unit->usart,StartCommand[3]);
}

/*
描述：
		失能X100m模块发送腔长值
函数名：
		Drv_X100mDisable
参数：
		p_unit：DRV_X100M_TYPE指针
返回值：无
*/
static void Drv_X100mDisable(DRV_X100M_TYPE *p_unit)
{
	BSP_UsartSendByte(p_unit->usart,StopCommand[0]);
	BSP_UsartSendByte(p_unit->usart,StopCommand[1]);
	BSP_UsartSendByte(p_unit->usart,StopCommand[2]);
	BSP_UsartSendByte(p_unit->usart,StopCommand[3]);
}

/*
描述：
		查询模块版本号
函数名：
		Drv_X100mQueryVersion
参数：
		p_unit：DRV_X100M_TYPE指针
返回值：无
*/
static void Drv_X100mQueryVersion(DRV_X100M_TYPE *p_unit)
{
	BSP_UsartSendByte(p_unit->usart,QueryCommand[0]);
	BSP_UsartSendByte(p_unit->usart,QueryCommand[1]);
}

/*
描述：
		查询模块环境压力
函数名：
		Drv_X100mPresH
参数：
		p_unit：DRV_X100M_TYPE指针
返回值：无
*/
static void Drv_X100mPresH(DRV_X100M_TYPE *p_unit)
{
	BSP_UsartSendByte(p_unit->usart,PresCommandH[0]);
	BSP_UsartSendByte(p_unit->usart,PresCommandH[1]);
}

/*
描述：
		查询模块环境压力
函数名：
		Drv_X100mPresL
参数：
		p_unit：DRV_X100M_TYPE指针
返回值：无
*/
static void Drv_X100mPresL(DRV_X100M_TYPE *p_unit)
{
	BSP_UsartSendByte(p_unit->usart,PresCommandL[0]);
	BSP_UsartSendByte(p_unit->usart,PresCommandL[1]);
}

/*
描述：
		模块中断服务函数
函数名：
		Drv_X100m1_IRQHandler
参数：
		无
返回值：无
*/
static void Drv_X100m1_IRQHandler(void)
{
	static uint8_t i = 0;
	uint8_t data;
	data = BSP_UsartGetByte(_gp_X100mUnits[0]->usart);
	if((_gp_X100mUnits[0]->index & 0x8000) != 0) /*完成一位帧头接收*/
	{
		if((_gp_X100mUnits[0]->index & 0x4000) != 0) /*开始接收数据*/
		{
			if(_gp_X100mUnits[0]->version <10) /*版本号小于10*/
			{
				if(_gp_X100mUnits[0]->status == X100M_TEMP) /*获取温度*/
				{
					_gp_X100mUnits[0]->rx_buffer[_gp_X100mUnits[0]->index&0x3FFF] = data;
					_gp_X100mUnits[0]->index++;
					if((_gp_X100mUnits[0]->index &0xff) >= 2)
					{
						_gp_X100mUnits[0]->index &= 0x3FFF; /*接收完成*/
						OSSemPost(_gp_X100mUnits[0]->x100msem);
					}
				}
				else if(_gp_X100mUnits[0]->status == X100M_PRES) /*获取压力*/
				{
					_gp_X100mUnits[0]->rx_buffer[_gp_X100mUnits[0]->index&0x3FFF] = data;
					_gp_X100mUnits[0]->index++;
					if((_gp_X100mUnits[0]->index & 0xff) >= 2)
					{
						_gp_X100mUnits[0]->index &= 0x3FFF; /*接收完成*/
						OSSemPost(_gp_X100mUnits[0]->x100msem);
					}
				}
				else
				{
					_gp_X100mUnits[0]->rx_buffer[_gp_X100mUnits[0]->index&0x3FFF] = data;
					_gp_X100mUnits[0]->index++;
					if((_gp_X100mUnits[0]->index & 0xff) >= 2)
					{
						_gp_X100mUnits[0]->index &= 0x3FFF; /*接收完成*/
						OSSemPost(_gp_X100mUnits[0]->x100msem);
					}
				}
			}
			else /*版本号不小于10*/
			{
				if(_gp_X100mUnits[0]->status == X100M_TEMP) /*获取温度*/
				{
					_gp_X100mUnits[0]->rx_buffer[_gp_X100mUnits[0]->index&0x3FFF] = data;
					_gp_X100mUnits[0]->index++;
					if((_gp_X100mUnits[0]->index &0xff) >= 3)
					{
						memcpy(&Temp_Data[i][0],_gp_X100mUnits[0]->rx_buffer,3);
						i++;
						if(i == 50)
						{
							OSSemPost(_gp_X100mUnits[0]->x100msem);
							i = 0;
						}
						_gp_X100mUnits[0]->index = 0;	
					}
				}
				else
				{
					_gp_X100mUnits[0]->rx_buffer[_gp_X100mUnits[0]->index&0x3FFF] = data;
					_gp_X100mUnits[0]->index++;
					if((_gp_X100mUnits[0]->index & 0xff) >= 2)
					{
						_gp_X100mUnits[0]->index &= 0x3FFF; /*接收完成*/
						OSSemPost(_gp_X100mUnits[0]->x100msem);
					}
				}
			}
		}
		else /*帧头第二位判断*/
		{
			if((data == 0xFF) && (_gp_X100mUnits[0]->status == X100M_TEMP))
			{
				_gp_X100mUnits[0]->index |=0x4000; 
			}
			else if(((data == 0x57) && (_gp_X100mUnits[0]->status == X100M_PRES)) || \
							((data == 0x56) && (_gp_X100mUnits[0]->status == X100M_PRES)))
			{
				_gp_X100mUnits[0]->index |=0x4000; 
			}
			else if(data == 0x03)
			{
				_gp_X100mUnits[0]->index |=0x4000;
			}
		}
	}
	else
	{
		if((data == 0xFE) && (_gp_X100mUnits[0]->status == X100M_TEMP)) /*开始判断帧头第一位 温度*/
		{
			_gp_X100mUnits[0]->index |=0x8000; 
		}
		else if((data == 0xFC) && (_gp_X100mUnits[0]->status == X100M_PRES)) /*开始判断帧头第一位 压力*/
		{
			_gp_X100mUnits[0]->index |=0x8000; 
		}
		else if(data == 0xFC) /*版本查询*/
		{
			_gp_X100mUnits[0]->index |=0x8000; 
		}
	}
}
