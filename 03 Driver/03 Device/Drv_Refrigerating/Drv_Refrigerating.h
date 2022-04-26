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
				��ָ�룬ͨ������Drv_RefrigeratingOpen��������Ƭ��ͨ��
				����Drv_RefrigeratingClose�ر�����Ƭ��
   ������     : BSP_Gpio V1.0.0.0
   ע         ����ģ�鲻������Ӳ����·�� 
 ********************************************************/

#ifndef _DRV_REFRIGERATING_H_
#define _DRV_REFRIGERATING_H_

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "BSP_Gpio.h"
#include "BSP_Adc.h"


/*����Ƭģ��ö��*/
typedef enum{
  REFRIGERATING_1  = 0,
	REFRIGERATING_2,
	REFRIGERATING_3,
	REFRIGERATING_4,
	REFRIGERATING_5,
	REFRIGERATING_6,
	REFRIGERATING_BUTT
} DRV_REFRIGERATING_ENUM;

/*����Ƭģ��״̬ö��*/
typedef enum{
	REFRIGERATING_UNDEFINED = 0,				/*δ��ʼ��*/
	REFRIGERATING_OPEN     = 1,					/*��*/
	REFRIGERATING_CLOSE    = 2,					/*�ر�*/
} DRV_REFRIGERATING_STATUS_ENUM;

/*����Ƭģ��ṹ��*/
typedef struct{
	/*------------------------------����*/
	DRV_REFRIGERATING_STATUS_ENUM status;			/*������Ƭģ���״̬*/
	bool pin_buffer;						/*�����������*/
	float Vc;										/*�ɼ���ѹֵ*/
	float Ic;										/*�ɼ�����ֵ*/
	/*------------------------------�䳣*/
	/*------------------------------����*/
	uint16_t Rc;													/*��������ֵ ��λ��ŷ*/
	DRV_REFRIGERATING_ENUM id;						/*����Ƭģ��id*/
	BSP_ADC_ENUM adc_num;						/*Refģ��adc���*/
	BSP_CH_ENUM adc_ch;							/*Refģ��adcͨ����*/
	BSP_PORT_ENUM port_number; 				/*�����Ƶ����Ŷ˿ں�*/
	BSP_PIN_ENUM pin_number;				/*�����Ƶ����ű��*/
	BSP_PORT_ENUM adc_port_number; 				/*adc�����Ƶ����Ŷ˿ں�*/
	BSP_PIN_ENUM adc_pin_number;				/*adc�����Ƶ����ű��*/
} DRV_REFRIGERATING_TYPE;

/* �ӿں���
 ********************************************************/

/* ��ʼ������Ƭģ��
  -----------------------------
  ��ڣ�����Ƭ��ID�ţ�IO�˿�
  ����ֵ����ʼ���õ�����Ƭģ��ṹ��
*/
extern DRV_REFRIGERATING_TYPE* Drv_RefrigeratingInit(DRV_REFRIGERATING_ENUM id, 						\
								  BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number,												\
									BSP_ADC_ENUM adc_num, 																										\
									BSP_PORT_ENUM adc_port_number, BSP_PIN_ENUM adc_pin_number, uint16_t Rc);

/* ������Ƭ
  -----------------------------
  ��ڣ�����Ƭ�ṹ��
*/
extern void Drv_RefrigeratingOpen(DRV_REFRIGERATING_TYPE* p_unit);

/* �ر�����Ƭ
  -----------------------------
 ��ڣ�����Ƭ�ṹ��
*/
extern void Drv_RefrigeratingClose(DRV_REFRIGERATING_TYPE* p_unit);

/* �жϼ���Ĥ�Ƿ���
  -----------------------------
  ��ڣ�����Ĥ�ṹ��
  ����ֵ���������Ĥ�����򷵻�true
*/
extern bool Drv_RefrigeratingIsOpen(DRV_REFRIGERATING_TYPE* p_unit);

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
static bool Drv_RefAdcInit(BSP_ADC_ENUM adc_number,BSP_PORT_ENUM port_number, BSP_PIN_ENUM pin_number);

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

extern void Drv_RefChaAndCalLoop(void);
#endif


