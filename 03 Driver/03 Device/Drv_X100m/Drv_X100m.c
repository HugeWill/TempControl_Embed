/********************************************************
   ��Ȩ���� (C), 2001-2100, �Ĵ��½���������ɷ����޹�˾
  -------------------------------------------------------

				Ƕ��ʽ����ƽ̨����X100mģ��

  -------------------------------------------------------
   �� �� ��   : Drv_X100m.c
   �� �� ��   : V1.0.0.0
   ��    ��   : xsh
   ��������   : 2022��04��14��
   ��������   : ����һ��X100m�¶�ѹ���ɼ�ģ�顣
   ʹ�÷���   ���ϲ�ģ��ͨ������Drv_X100mInit���һ������ģ��
								��ָ�룬ͨ������Drv_GetX100mTemp��ȡ������¶ȣ�
								ͨ������Drv_GetX100mTemp��ȡ������¶ȣ�ͨ������
								Drv_SetTempPara�޸��¶Ȳ�����
   ������     : BSP_Uart V1.0.0.0
								ucosii����ϵͳ
   ע         ����ģ�鲻������Ӳ����·�� 
 ********************************************************/
#include "Drv_X100m.h"

static uint8_t StartCommand[4] = {0xFA, 0x07, 0x00, 0x01}; /*ʹģ����ⷢ��ǻ������*/
static uint8_t StopCommand[4]  = {0xFA, 0x07, 0x00, 0x00}; /*ֹͣģ����ⷢ��ǻ������*/
static uint8_t QueryCommand[2] = {0xFB, 0x03};						 /*��ѯģ�鵱ǰ����汾��Ϣ*/
static uint8_t PresCommandH[2] = {0xFB, 0x57};						 /*���ͻ���ѹ��*/
static uint8_t PresCommandL[2] = {0xFB, 0x56};						 /*���ͻ���ѹ��*/
uint8_t Temp_Data[50][3];
DRV_X100M_TYPE *_gp_X100mUnits[X100M_BUTT] = {0};	/*NTC��ע���*/

/*
������
		��ʼ��X100mģ��
��������
		Drv_X100mInit
������
		id							��ģ��ID��
		baud_rate				��ģ�鲨����
		usart			  		��ģ�鴮�ں�
		tx_port_number	��ģ�����Ŷ˿ں�
		tx_pin_number		��ģ�����ź�
		rx_port_number	��ģ�����Ŷ˿ں�
		rx_pin_number		��ģ�����ź�
		a								��ģ���¶Ȳ���
		b								��ģ���¶Ȳ���
		c								��ģ���¶Ȳ���
		d								��ģ���¶Ȳ���
		e								��ģ���¶Ȳ���
����ֵ��DRV_X100M_TYPEָ��
*/
DRV_X100M_TYPE *Drv_X100mInit(DRV_X100M_ENUM id, uint32_t baud_rate, BSP_USART_ENUM usart,    	\
											BSP_PORT_ENUM tx_port_number, BSP_PIN_ENUM tx_pin_number, 								\
											BSP_PORT_ENUM rx_port_number, BSP_PIN_ENUM rx_pin_number,									\
											float a, float b, float c, float d, float e)
{
	uint8_t i;
	DRV_X100M_TYPE* p_unit = (DRV_X100M_TYPE*)calloc(1, sizeof(DRV_X100M_TYPE));	/*�����ڴ�*/
	
	/*����ʧ�ܷ���NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id���󷵻�NULL*/
	if(id >= X100M_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*�����ǰģ����ڣ�ȡ�������´洢��*/
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
	p_unit->tx_port_number = tx_port_number;				/*���Ŷ˿ں�*/
	p_unit->tx_pin_number  = tx_pin_number;					/*�������*/
	p_unit->rx_port_number = rx_port_number;				/*���Ŷ˿ں�*/
	p_unit->rx_pin_number  = rx_pin_number;					/*�������*/
	p_unit->temp_para.a = a;
	p_unit->temp_para.b = b;
	p_unit->temp_para.c = c;
	p_unit->temp_para.d = d;
	p_unit->temp_para.e = e;
	
	BSP_Init_UsartInterrupt(usart,baud_rate,tx_port_number,tx_pin_number,rx_port_number,rx_pin_number);
	BSP_UartIRQHandler[usart] = Drv_X100m1_IRQHandler;
	
	/*------------��ʼ��Ĭ�ϲ���-----------*/
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
������
		����ģ���¶Ȳ���
��������
		Drv_SetTempPara
������
		p_unit��DRV_X100M_TYPEָ��
		para  ��DRV_X100M_PARA_TYPE�ṹ��
����ֵ����
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
������
		����X100mģ���������¶�ֵ
��������
		Drv_GetX100mTemp
������
		p_unit��DRV_X100M_TYPEָ��
����ֵ��ģ���¶�ֵ
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
	if(p_unit->version == 0) /*û�а汾�ţ��Ȼ�ȡ*/
	{
		Drv_X100mQueryVersion(p_unit);
		OSSemPend(p_unit->x100msem,500,&err);
		if(err != OS_ERR_NONE)
		{
			return 0;
		}
		p_unit->version = ((p_unit->rx_buffer[0]<<8) | (p_unit->rx_buffer[1]&0xFF)); /*����汾��*/
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
//								 + p_unit->temp_para.e;/*�����¶�*/
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
								 + p_unit->temp_para.e;/*�����¶�*/
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
������
		����X100mģ��������ѹ��ֵ
��������
		Drv_GetX100mPres
������
		p_unit��DRV_X100M_TYPEָ��
����ֵ��ģ��ѹ��ֵ
*/
double Drv_GetX100mPres(DRV_X100M_TYPE *p_unit)
{
	uint8_t err;
	uint32_t buf;
	
	p_unit->index = 0;
	memset(p_unit->rx_buffer,0x00,DTADLEN);
	
	if(p_unit->version == 0) /*û�а汾�ţ��Ȼ�ȡ*/
	{
		Drv_X100mQueryVersion(p_unit);
		OSSemPend(p_unit->x100msem,500,&err);
		if(err != OS_ERR_NONE)
		{
			return 0;
		}
		p_unit->version = ((p_unit->rx_buffer[0]<<8) | (p_unit->rx_buffer[1]&0xFF)); /*����汾��*/
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
		p_unit->pres = buf / 4.0; /*����ѹ��*/
	}
	else
	{
		return 0;
	}
	
	return p_unit->pres;
}

/*-----------------------------��̬����-----------------------------------*/
/*
������
		ʹ��X100mģ�鷢��ǻ��ֵ
��������
		Drv_X100m1Enable
������
		p_unit��DRV_X100M_TYPEָ��
����ֵ����
*/
static void Drv_X100m1Enable(DRV_X100M_TYPE *p_unit)
{
	BSP_UsartSendByte(p_unit->usart,StartCommand[0]);
	BSP_UsartSendByte(p_unit->usart,StartCommand[1]);
	BSP_UsartSendByte(p_unit->usart,StartCommand[2]);
	BSP_UsartSendByte(p_unit->usart,StartCommand[3]);
}

/*
������
		ʧ��X100mģ�鷢��ǻ��ֵ
��������
		Drv_X100mDisable
������
		p_unit��DRV_X100M_TYPEָ��
����ֵ����
*/
static void Drv_X100mDisable(DRV_X100M_TYPE *p_unit)
{
	BSP_UsartSendByte(p_unit->usart,StopCommand[0]);
	BSP_UsartSendByte(p_unit->usart,StopCommand[1]);
	BSP_UsartSendByte(p_unit->usart,StopCommand[2]);
	BSP_UsartSendByte(p_unit->usart,StopCommand[3]);
}

/*
������
		��ѯģ��汾��
��������
		Drv_X100mQueryVersion
������
		p_unit��DRV_X100M_TYPEָ��
����ֵ����
*/
static void Drv_X100mQueryVersion(DRV_X100M_TYPE *p_unit)
{
	BSP_UsartSendByte(p_unit->usart,QueryCommand[0]);
	BSP_UsartSendByte(p_unit->usart,QueryCommand[1]);
}

/*
������
		��ѯģ�黷��ѹ��
��������
		Drv_X100mPresH
������
		p_unit��DRV_X100M_TYPEָ��
����ֵ����
*/
static void Drv_X100mPresH(DRV_X100M_TYPE *p_unit)
{
	BSP_UsartSendByte(p_unit->usart,PresCommandH[0]);
	BSP_UsartSendByte(p_unit->usart,PresCommandH[1]);
}

/*
������
		��ѯģ�黷��ѹ��
��������
		Drv_X100mPresL
������
		p_unit��DRV_X100M_TYPEָ��
����ֵ����
*/
static void Drv_X100mPresL(DRV_X100M_TYPE *p_unit)
{
	BSP_UsartSendByte(p_unit->usart,PresCommandL[0]);
	BSP_UsartSendByte(p_unit->usart,PresCommandL[1]);
}

/*
������
		ģ���жϷ�����
��������
		Drv_X100m1_IRQHandler
������
		��
����ֵ����
*/
static void Drv_X100m1_IRQHandler(void)
{
	static uint8_t i = 0;
	uint8_t data;
	data = BSP_UsartGetByte(_gp_X100mUnits[0]->usart);
	if((_gp_X100mUnits[0]->index & 0x8000) != 0) /*���һλ֡ͷ����*/
	{
		if((_gp_X100mUnits[0]->index & 0x4000) != 0) /*��ʼ��������*/
		{
			if(_gp_X100mUnits[0]->version <10) /*�汾��С��10*/
			{
				if(_gp_X100mUnits[0]->status == X100M_TEMP) /*��ȡ�¶�*/
				{
					_gp_X100mUnits[0]->rx_buffer[_gp_X100mUnits[0]->index&0x3FFF] = data;
					_gp_X100mUnits[0]->index++;
					if((_gp_X100mUnits[0]->index &0xff) >= 2)
					{
						_gp_X100mUnits[0]->index &= 0x3FFF; /*�������*/
						OSSemPost(_gp_X100mUnits[0]->x100msem);
					}
				}
				else if(_gp_X100mUnits[0]->status == X100M_PRES) /*��ȡѹ��*/
				{
					_gp_X100mUnits[0]->rx_buffer[_gp_X100mUnits[0]->index&0x3FFF] = data;
					_gp_X100mUnits[0]->index++;
					if((_gp_X100mUnits[0]->index & 0xff) >= 2)
					{
						_gp_X100mUnits[0]->index &= 0x3FFF; /*�������*/
						OSSemPost(_gp_X100mUnits[0]->x100msem);
					}
				}
				else
				{
					_gp_X100mUnits[0]->rx_buffer[_gp_X100mUnits[0]->index&0x3FFF] = data;
					_gp_X100mUnits[0]->index++;
					if((_gp_X100mUnits[0]->index & 0xff) >= 2)
					{
						_gp_X100mUnits[0]->index &= 0x3FFF; /*�������*/
						OSSemPost(_gp_X100mUnits[0]->x100msem);
					}
				}
			}
			else /*�汾�Ų�С��10*/
			{
				if(_gp_X100mUnits[0]->status == X100M_TEMP) /*��ȡ�¶�*/
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
						_gp_X100mUnits[0]->index &= 0x3FFF; /*�������*/
						OSSemPost(_gp_X100mUnits[0]->x100msem);
					}
				}
			}
		}
		else /*֡ͷ�ڶ�λ�ж�*/
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
		if((data == 0xFE) && (_gp_X100mUnits[0]->status == X100M_TEMP)) /*��ʼ�ж�֡ͷ��һλ �¶�*/
		{
			_gp_X100mUnits[0]->index |=0x8000; 
		}
		else if((data == 0xFC) && (_gp_X100mUnits[0]->status == X100M_PRES)) /*��ʼ�ж�֡ͷ��һλ ѹ��*/
		{
			_gp_X100mUnits[0]->index |=0x8000; 
		}
		else if(data == 0xFC) /*�汾��ѯ*/
		{
			_gp_X100mUnits[0]->index |=0x8000; 
		}
	}
}
