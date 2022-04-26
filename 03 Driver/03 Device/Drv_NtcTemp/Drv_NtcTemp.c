/********************************************************
   ��Ȩ���� (C), 2001-2100, �Ĵ��½���������ɷ����޹�˾
  -------------------------------------------------------

				Ƕ��ʽ����ƽ̨��������Ĥģ��

  -------------------------------------------------------
   �� �� ��   : Drv_NtcTemp.c
   �� �� ��   : V1.0.0.0
   ��    ��   : xsh
   ��������   : 2022��04��12��
   ��������   : ����һ��NTC�¶Ȳɼ�ģ�顣
   ʹ�÷���   ���ϲ�ģ��ͨ������Drv_NtcTempInit���һ������ģ��
				��ָ�룬ͨ������Drv_NtcChaAndCalLoop������ز�����ͨ��
				����Drv_GetNtcTemp��ȡ������¶ȡ�
								ͨ���޸�VERSION�����ֵ���Ĳ�ͬ�汾��
   ������     : BSP_Gpio V1.0.0.0
								BSP_Adc V1.0.0.0
   ע         ����ģ�鲻������Ӳ����·�� 
 ********************************************************/

#include "Drv_NtcTemp.h"

DRV_NTC_TYPE *_gp_NTCUnits[NTC_BUTT] = {0};	/*NTC��ע���*/

/*
������
		��ʼ��NTCģ��
��������
		Drv_NtcTempInit
������
		id					��ģ��ID��
		adc_number	��ģ��ADC��
		port_number ��ģ��˿ں�
		pin_number	��ģ�����ź�
		Rd					��ģ���·�·�ѹ����ֵ
		Rp					��ģ��NTC�����µ���ֵ
		Bx					��ģ��NTC��Ҫ����ֵ
		V						��ģ���·���ܵ�ѹֵ
		Am					: ģ���·�º�������ֵ
����ֵ��DRV_NTC_TYPEָ��
*/

DRV_NTC_TYPE *Drv_NtcTempInit(DRV_NTC_ENUM id, BSP_ADC_ENUM adc_number, BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number, \
														double Rd, double Rp, double Bx,double V, double Am)
{
	DRV_NTC_TYPE* p_unit = (DRV_NTC_TYPE*)calloc(1, sizeof(DRV_NTC_TYPE));	/*�����ڴ�*/
	
	/*����ʧ�ܷ���NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id���󷵻�NULL*/
	if(id >= NTC_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*�����ǰģ����ڣ�ȡ�������´洢��*/
	if(_gp_NTCUnits[id] != NULL)
	{
		free(p_unit);
		p_unit = _gp_NTCUnits[id];
	}
	else
	{
		_gp_NTCUnits[id] = p_unit;
	}
	
	/*�ж��˿ںŵ�ADCͨ��*/
	switch(port_number)
	{
		case _PA_:
		{	
			if(pin_number >= _P0_ && pin_number <= _P7_)
			{
				p_unit->adc_ch = (BSP_CH_ENUM)pin_number;  /*����ADCͨ����*/
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
				p_unit->adc_ch = (BSP_CH_ENUM)(pin_number + 8); /*����ADCͨ����*/
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
				p_unit->adc_ch = (BSP_CH_ENUM)(pin_number + 10); /*����ADCͨ����*/
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
	p_unit->port_number = port_number;				/*���Ŷ˿ں�*/
	p_unit->pin_number  = pin_number;					/*�������*/
	
	p_unit->Rd = Rd;													/*��NTC��ӵķֵ��� ��λK*/
	p_unit->Rp = Rp;													/*NTC����(25��C)��ֵ ��λK*/
	p_unit->Bx = Bx;													/*NTC��Ҫ����*/
	p_unit->V	 = V;
	p_unit->Am = Am;
	p_unit->T2 = 273.15 + 25.0; 							/*NTC�����µĿ������¶�*/
	p_unit->Ka = 273.15;
	
	if(Drv_NtcAdcInit(adc_number, port_number, pin_number) != true)
	{
		free(p_unit);
		return NULL;
	}
	/*------------��ʼ��Ĭ�ϲ���-----------*/
	p_unit->status    = NTC_OPEN;        /*Ĭ�Ͽ���NTC�ɼ�����*/
	p_unit->temp 	    = 0.0f;
	p_unit->adc_value = 0;	
	p_unit->R 				= 0;	
	return p_unit;
}

/*
������
		����NTCģ���������¶�ֵ
��������
		Drv_GetNtcTemp
������
		p_unit��DRV_NTC_TYPEָ��
����ֵ��ģ����¶�ֵ
*/
double Drv_GetNtcTemp(DRV_NTC_TYPE *p_unit)
{
	return p_unit->temp;
}

/*
������
		����NTCģ��������ADCֵ
��������
		Drv_GetNtcAdc
������
		p_unit��DRV_NTC_TYPEָ��
����ֵ��ģ���ADC
*/
uint32_t Drv_GetNtcAdc(DRV_NTC_TYPE *p_unit)
{
	return p_unit->adc_value;
}

/*
������
		����NTCģ�������ĵ���ֵ
��������
		Drv_GetNtcR
������
		p_unit��DRV_NTC_TYPEָ��
����ֵ��ģ��NTC����ֵ
*/
double Drv_GetNtcR(DRV_NTC_TYPE *p_unit)
{
	return p_unit->R;
}

/*
������
		����NTCģ��ɼ�����
��������
		Drv_OpenNtc
������
		p_unit��DRV_NTC_TYPEָ��
����ֵ����
*/
void Drv_OpenNtc(DRV_NTC_TYPE *p_unit)
{
	p_unit->status = NTC_OPEN;
}

/*
������
		�ر�NTCģ��ɼ�����
��������
		Drv_CloseNtc
������
		p_unit��DRV_NTC_TYPEָ��
����ֵ����
*/
void Drv_CloseNtc(DRV_NTC_TYPE *p_unit)
{
	p_unit->status = NTC_CLOSE;
}

/*
������
		�ж�NTCģ���Ƿ�򿪲ɼ�����
��������
		Drv_NtcIsOpen
������
		p_unit��DRV_NTC_TYPEָ��
����ֵ��true or false
*/
bool Drv_NtcIsOpen(DRV_NTC_TYPE *p_unit)
{
	if(p_unit->status == NTC_OPEN)
		return true;
	else
		return false;
}


/*
������
		�ɼ����Ҽ���ģ���������
��������
		Drv_NtcChaAndCalLoop
������
		��
����ֵ����
*/
void Drv_NtcChaAndCalLoop(void)
{
	uint8_t i;
	for(i = 0; i < NTC_BUTT; i++)
	{
		if(_gp_NTCUnits[i] != NULL)
//		if(_gp_NTCUnits[i]->status == NTC_OPEN) /*�ж��Ƿ�����������*/
		{
			#if defined XC300
			
			_gp_NTCUnits[i]->adc_value = BSP_ReadAdc(_gp_NTCUnits[i]->adc_num,_gp_NTCUnits[i]->adc_ch);
			_gp_NTCUnits[i]->temp = 3.3 * _gp_NTCUnits[i]->adc_value / 4096.0; /*����NTC��ѹֵ*/
			_gp_NTCUnits[i]->R = (_gp_NTCUnits[i]->adc_value * _gp_NTCUnits[i]->Rd) / (_gp_NTCUnits[i]->V - _gp_NTCUnits[i]->adc_value);
			_gp_NTCUnits[i]->temp =_gp_NTCUnits[i]->R / _gp_NTCUnits[i]->Rp;
			_gp_NTCUnits[i]->temp = log(_gp_NTCUnits[i]->temp);
			_gp_NTCUnits[i]->temp /= _gp_NTCUnits[i]->Bx;
			_gp_NTCUnits[i]->temp += (1/_gp_NTCUnits[i]->T2);
			_gp_NTCUnits[i]->temp = 1/_gp_NTCUnits[i]->temp;
			_gp_NTCUnits[i]->temp -= _gp_NTCUnits[i]->Ka;
			
			#elif defined XC8002C
			_gp_NTCUnits[i]->adc_value = BSP_ReadAdc(_gp_NTCUnits[i]->adc_num,_gp_NTCUnits[i]->adc_ch);
			_gp_NTCUnits[i]->temp = 2 * 3.3 * _gp_NTCUnits[i]->adc_value / 4096.0; /*����NTC��ѹֵ*/
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

/*-------------------------------------��̬����-------------------------------------------------*/
/*
������
		NTCģ��Ӳ����ʼ��
��������
		Drv_NtcAdcInit
������
		adc_number	��ģ��ADC��
		port_number ��ģ��˿ں�
		pin_number	��ģ�����ź�
����ֵ��true or false
*/
static bool Drv_NtcAdcInit(BSP_ADC_ENUM adc_number,BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number)
{
	ADC_InitTypeDef ADC_InitStructure;
	
	
	BSP_Init_Pin(port_number, pin_number, _IN_AIN_); /*��ʼ��Ӳ��*/
	
	if(Drv_AdcIsEnable(adc_number) == true)
	{
		return true;
	}
	
	/*��ʼ��ADC*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;			/*����ͨ��ɨ��*/
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		/*��������ת��*/
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	/*��ʹ���ⲿ����ת��*/
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	/*�ɼ������Ҷ���*/
	ADC_InitStructure.ADC_NbrOfChannel = 1;					/*ת��ͨ����1*/
	
	switch(adc_number)
	{
		case _ADC1_:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
			ADC_Init(ADC1, &ADC_InitStructure);
			
			RCC_ADCCLKConfig(RCC_PCLK2_Div8);			/*����ADCʱ��*/
			
			ADC_Cmd(ADC1, ENABLE);						/*ʹ��ADC*/
			
			ADC_ResetCalibration(ADC1);					/*��λУ׼�Ĵ���*/
			while(ADC_GetResetCalibrationStatus(ADC1));	/*�ȴ�У׼�Ĵ�����λ���*/
			
			ADC_StartCalibration(ADC1);					/*ADCУ׼*/
			while(ADC_GetCalibrationStatus(ADC1));		/*�ȴ�У׼���*/
			
			break;
			
		case _ADC2_:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
			ADC_Init(ADC2, &ADC_InitStructure);
			
			RCC_ADCCLKConfig(RCC_PCLK2_Div8);			/*����ADCʱ��*/
			
			ADC_Cmd(ADC2, ENABLE);						/*ʹ��ADC*/
			
			ADC_ResetCalibration(ADC2);					/*��λУ׼�Ĵ���*/
			while(ADC_GetResetCalibrationStatus(ADC2));	/*�ȴ�У׼�Ĵ�����λ���*/
			
			ADC_StartCalibration(ADC2);					/*ADCУ׼*/
			while(ADC_GetCalibrationStatus(ADC2));		/*�ȴ�У׼���*/
			
			break;
			
		case _ADC3_:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
			ADC_Init(ADC3, &ADC_InitStructure);
			
			RCC_ADCCLKConfig(RCC_PCLK2_Div8);			/*����ADCʱ��*/
			
			ADC_Cmd(ADC3, ENABLE);						/*ʹ��ADC*/
			
			ADC_ResetCalibration(ADC3);					/*��λУ׼�Ĵ���*/
			while(ADC_GetResetCalibrationStatus(ADC3));	/*�ȴ�У׼�Ĵ�����λ���*/
			
			ADC_StartCalibration(ADC3);					/*ADCУ׼*/
			while(ADC_GetCalibrationStatus(ADC3));		/*�ȴ�У׼���*/
			
			break;
			
		default:
			return false;		
	}
	return true;
}

/*
������
		�ж�ADCʱ���Ƿ��� Ӧ����BSP
��������
		Drv_AdcIsEnable
������
		adc_number	��ģ��ADC��
����ֵ��true or false
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

