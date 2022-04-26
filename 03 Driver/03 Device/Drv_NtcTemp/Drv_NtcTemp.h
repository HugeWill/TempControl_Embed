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

/*NTCģ��ö��*/
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

/*NTCģ��״̬ö��*/
typedef enum{
	NTC_UNDEFINED = 0,				/*δ��ʼ��*/
	NTC_OPEN     = 1,					/*��*/
	NTC_CLOSE    = 2,					/*�ر�*/
} DRV_NTC_STATUS_ENUM;

/*NTCģ��ṹ��*/
typedef struct
{
	/*----------����------------*/
	DRV_NTC_STATUS_ENUM status;										/*NTCģ��״̬  0��ʾ�ر� 1��ʾ����*/
	uint32_t adc_value;   						/*adcԭʼֵ*/
	double temp; 											/*�¶�*/
	double R;													/*NTC����ֵ ��λK*/
	
	/*---------����------------*/
	double Rd;											/*��NTC��ӵķ�ѹ���� ��λK*/
	double Rp;                      /*NTC����(25��C)��ֵ ��λK*/
	double Bx;											/*NTC��Ҫ����*/
	double T2;											/*NTC�����µĿ������¶�*/
	double Ka;
	double V;												/*NTC��·���ܵ�ѹֵ*/
	double Am;											/*NTC��·�к�������ֵ ��λmA*/
	DRV_NTC_ENUM id;								/*NTCģ��id*/
	BSP_ADC_ENUM adc_num;						/*NTCģ��adc���*/
	BSP_CH_ENUM adc_ch;							/*NTCģ��adcͨ����*/
	BSP_PORT_ENUM port_number;			/*�����Ƶ����Ŷ˿ں�*/
	BSP_PIN_ENUM pin_number;				/*�����Ƶ����ű��*/
}DRV_NTC_TYPE;

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
����ֵ��DRV_NTC_TYPEָ��
*/

DRV_NTC_TYPE *Drv_NtcTempInit(DRV_NTC_ENUM id, BSP_ADC_ENUM adc_number, BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number, \
														double Rd, double Rp, double Bx,double V, double Am);

/*
������
		����NTCģ���������¶�ֵ
��������
		Drv_GetNtcTemp
������
		p_unit��DRV_NTC_TYPEָ��
����ֵ��ģ����¶�ֵ
*/
double Drv_GetNtcTemp(DRV_NTC_TYPE *p_unit);

/*
������
		����NTCģ��������ADCֵ
��������
		Drv_GetNtcAdc
������
		p_unit��DRV_NTC_TYPEָ��
����ֵ��ģ���ADC
*/
uint32_t Drv_GetNtcAdc(DRV_NTC_TYPE *p_unit);

/*
������
		����NTCģ�������ĵ���ֵ
��������
		Drv_GetNtcR
������
		p_unit��DRV_NTC_TYPEָ��
����ֵ��ģ��NTC����ֵ
*/
double Drv_GetNtcR(DRV_NTC_TYPE *p_unit);

/*
������
		����NTCģ��ɼ�����
��������
		Drv_OpenNtc
������
		p_unit��DRV_NTC_TYPEָ��
����ֵ����
*/
void Drv_OpenNtc(DRV_NTC_TYPE *p_unit);

/*
������
		�ر�NTCģ��ɼ�����
��������
		Drv_CloseNtc
������
		p_unit��DRV_NTC_TYPEָ��
����ֵ����
*/
void Drv_CloseNtc(DRV_NTC_TYPE *p_unit);

/*
������
		�ж�NTCģ���Ƿ�򿪲ɼ�����
��������
		Drv_NtcIsOpen
������
		p_unit��DRV_NTC_TYPEָ��
����ֵ��true or false
*/
bool Drv_NtcIsOpen(DRV_NTC_TYPE *p_unit);

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
static bool Drv_NtcAdcInit(BSP_ADC_ENUM adc_number,BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number);

/*
������
		�ж�ADCʱ���Ƿ��� Ӧ����BSP
��������
		Drv_AdcIsEnable
������
		adc_number	��ģ��ADC��
����ֵ��true or false
*/
static bool Drv_AdcIsEnable(BSP_ADC_ENUM adc_number);

/*
������
		�ɼ����Ҽ���ģ���������
��������
		Drv_NtcChaAndCalLoop
������
		��
����ֵ����
*/
extern void Drv_NtcChaAndCalLoop(void);

#endif

