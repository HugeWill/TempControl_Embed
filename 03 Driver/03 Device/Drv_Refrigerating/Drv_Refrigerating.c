/********************************************************
   ��Ȩ���� (C), 2001-2100, �Ĵ��½���������ɷ����޹�˾
  -------------------------------------------------------

				Ƕ��ʽ����ƽ̨��������Ĥģ��

  -------------------------------------------------------
   �� �� ��   : Drv_Refrigerating.c
   �� �� ��   : V1.0.0.0
   ��    ��   : xc
   ��������   : 2021��05��17��
   ��������   : ����һ���������ģ�顣
   ʹ�÷���   ���ϲ�ģ��ͨ������Drv_RefrigeratingInit���һ������ģ��
				��ָ�룬ͨ������Drv_Refrigerating_Open��������Ƭ��ͨ��
				����Drv_Refrigerating_Close�ر�����Ƭ��
   ������     : BSP_Gpio V1.0.0.0
   ע         ����ģ�鲻������Ӳ����·��
	 
	 �� �� ��   : Drv_Refrigerating.c
   �� �� ��   : V1.0.1.0
   ��    ��   : xsh
   ��������   : 2022��04��13��
   ��������   : ����һ���������ģ�顣
   ������     : BSP_Gpio V1.0.0.0
								BSP_Adc V1.0.0.0
   ע         :���ӵ����ɼ�
 ********************************************************/

#include "Drv_Refrigerating.h"

DRV_REFRIGERATING_TYPE* _gp_RefrigeratingUnits[REFRIGERATING_BUTT] = {0};	/*����Ƭ��ע���*/

/* �ӿں���
 ********************************************************/

/* ��ʼ������Ƭģ��
  -----------------------------
  ��ڣ�����Ƭ��ID�ţ�IO�˿ڣ�ADC�ţ�ADC IO�˿ڣ� �������� mR
  ����ֵ����ʼ���õ�����Ƭģ��ṹ��
*/
DRV_REFRIGERATING_TYPE* Drv_RefrigeratingInit(DRV_REFRIGERATING_ENUM id, 						\
								  BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number,												\
									BSP_ADC_ENUM adc_num,																	\
									BSP_PORT_ENUM adc_port_number, BSP_PIN_ENUM adc_pin_number, uint16_t Rc)
{	
	DRV_REFRIGERATING_TYPE* p_unit = (DRV_REFRIGERATING_TYPE*)calloc(1, sizeof(DRV_REFRIGERATING_TYPE));	/*�����ڴ�*/
	
	/*����ʧ�ܷ���NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id���󷵻�NULL*/
	if(id >= REFRIGERATING_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*�����ǰģ����ڣ�ȡ�������´洢��*/
	if(_gp_RefrigeratingUnits[id] != NULL)
	{
		free(p_unit);
		p_unit = _gp_RefrigeratingUnits[id];
	}
	else
	{
		_gp_RefrigeratingUnits[id] = p_unit;
	}
	
	/*�ж��˿ںŵ�ADCͨ��*/
	switch(adc_port_number)
	{
		case _PA_:
		{	
			if(adc_pin_number >= _P0_ && adc_pin_number <= _P7_)
			{
				p_unit->adc_ch = (BSP_CH_ENUM)adc_pin_number;  /*����ADCͨ����*/
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
				p_unit->adc_ch = (BSP_CH_ENUM)(adc_pin_number + 8); /*����ADCͨ����*/
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
				p_unit->adc_ch = (BSP_CH_ENUM)(adc_pin_number + 10); /*����ADCͨ����*/
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
	
	/*��ʼ��*/
	
	/*-------------------------------����*/
	p_unit->id          = id;						/*ģ��id*/
	p_unit->adc_num = adc_num;					/*Refģ��adc���*/
	p_unit->Rc = Rc;										/*Refģ���������*/
	p_unit->port_number = port_number;				/*���Ŷ˿ں�*/
	p_unit->pin_number  = pin_number;				/*�������*/
	p_unit->adc_port_number = adc_port_number;				/*���Ŷ˿ں�*/
	p_unit->adc_pin_number  = adc_pin_number;				/*�������*/
	
	if(Drv_RefAdcInit(adc_num, adc_port_number, adc_pin_number) != true)
	{
		free(p_unit);
		return NULL;
	}
	BSP_Init_Pin(port_number, pin_number, _OUT_PP_);	/*��ʼ��Ӳ��*/
	
	/*-------------------------------����*/
	p_unit->status = REFRIGERATING_CLOSE;	/*ȫ��*/
	p_unit->Vc = 0;
	p_unit->Ic = 0;
	
	p_unit->pin_buffer = 0;							/*��ʼ��Ϊ�ߵ�ƽ���ر�����Ƭ*/
	BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	
	return p_unit;
}

/* ������Ƭ
  -----------------------------
  ��ڣ�����Ƭ�ṹ��
*/
void  Drv_RefrigeratingOpen(DRV_REFRIGERATING_TYPE* p_unit)
{
	p_unit->pin_buffer = 1;
	BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	p_unit->status = REFRIGERATING_OPEN;
}

/* �ر�����Ƭ
  -----------------------------
 ��ڣ�����Ƭ�ṹ��
*/
void Drv_RefrigeratingClose(DRV_REFRIGERATING_TYPE* p_unit)
{
	p_unit->pin_buffer = 0;
	BSP_WritePin(p_unit->port_number, p_unit->pin_number, p_unit->pin_buffer);
	p_unit->status =REFRIGERATING_CLOSE;
}

/* �ж�����Ƭ�Ƿ���
  -----------------------------
  ��ڣ�����Ƭ�ṹ��
  ����ֵ���������Ƭ�����򷵻�true
*/
bool Drv_RefrigeratingIsOpen(DRV_REFRIGERATING_TYPE* p_unit)
{
	if(p_unit->status == REFRIGERATING_OPEN)
		return true;
	else
		return false;
}

/*
������
		�ɼ����Ҽ���ģ���������
��������
		Drv_RefChaAndCalLoop
������
		��
����ֵ����
*/
void Drv_RefChaAndCalLoop(void)
{
	uint8_t i;
	for(i = 0; i < REFRIGERATING_BUTT; i++)
	{
		if(_gp_RefrigeratingUnits[i]->status == REFRIGERATING_OPEN) /*�ж��Ƿ�����������*/
		{
			_gp_RefrigeratingUnits[i]->Vc = BSP_ReadAdc(_gp_RefrigeratingUnits[i]->adc_num,_gp_RefrigeratingUnits[i]->adc_ch);
			_gp_RefrigeratingUnits[i]->Vc = (3.3 * _gp_RefrigeratingUnits[i]->Vc / 4096.0)/50;
			_gp_RefrigeratingUnits[i]->Ic = (_gp_RefrigeratingUnits[i]->Vc / _gp_RefrigeratingUnits[i]->Rc) * 1000 ;
		}
	}
}


/*-------------------------------------��̬����-------------------------------------------------*/
/*
������
		Refģ��Ӳ����ʼ��
��������
		Drv_NtcAdcInit
������
		adc_number	��ģ��ADC��
		port_number ��ģ��˿ں�
		pin_number	��ģ�����ź�
����ֵ��true or false
*/
static bool Drv_RefAdcInit(BSP_ADC_ENUM adc_number,BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number)
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

