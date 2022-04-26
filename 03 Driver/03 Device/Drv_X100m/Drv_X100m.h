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
#ifndef __DRV_X100M_H
#define __DRV_X100M_H

#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "BSP_Uart.h"
#include "ucos_ii.h"

#define DTADLEN 10 /*�������ݳ���*/
extern uint8_t Temp_Data[10][3];
/*X100mģ��ö��*/
typedef enum
{
	X100M_1 = 0,
	X100M_BUTT,
}DRV_X100M_ENUM;

/*X100mģ���������ö��*/
typedef enum
{
	X100M_UNDEFINE = 0,
	X100M_TEMP,
	X100M_PRES,
}DRV_X100M_STATUS_ENUM;

/*X100mģ���¶�ϵ��*/
typedef struct
{
	float a;
	float b;
	float c;
	float d;
	float e;
}DRV_X100M_PARA_TYPE;

typedef struct
{
	/*----------����------------*/
	double temp;  												/*�¶�ֵ*/
	double pres;													/*ѹ��ֵ*/
	uint16_t index;											/*����ѹջֵ*/
	uint8_t rx_buffer[DTADLEN];					/*���ݻ�����*/
	OS_EVENT *x100msem;									/*��������ź���*/
	DRV_X100M_STATUS_ENUM status;       /*����״̬����*/
	
	/*---------����------------*/
	uint32_t baud_rate;									/*������*/
	uint16_t version;										/*�汾��*/
	DRV_X100M_ENUM id;									/*ID��*/
	BSP_USART_ENUM usart;								/*���ڶ˺�*/
	DRV_X100M_PARA_TYPE temp_para;			/*�¶�ϵ��ֵ*/
	BSP_PORT_ENUM tx_port_number;			/*�����Ƶ����Ŷ˿ں�*/
	BSP_PIN_ENUM tx_pin_number;				/*�����Ƶ����ű��*/
	BSP_PORT_ENUM rx_port_number;			/*�����Ƶ����Ŷ˿ں�*/
	BSP_PIN_ENUM rx_pin_number;				/*�����Ƶ����ű��*/
}DRV_X100M_TYPE;

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
											float a, float b, float c, float d, float e);

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
void Drv_SetTempPara(DRV_X100M_TYPE *p_unit, DRV_X100M_PARA_TYPE para);

/*
������
		����X100mģ���������¶�ֵ
��������
		Drv_GetX100mTemp
������
		p_unit��DRV_X100M_TYPEָ��
����ֵ��ģ���¶�ֵ
*/
double Drv_GetX100mTemp(DRV_X100M_TYPE *p_unit);

/*
������
		����X100mģ��������ѹ��ֵ
��������
		Drv_GetX100mPres
������
		p_unit��DRV_X100M_TYPEָ��
����ֵ��ģ��ѹ��ֵ
*/
double Drv_GetX100mPres(DRV_X100M_TYPE *p_unit);

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
static void Drv_X100m1Enable(DRV_X100M_TYPE *p_unit);

/*
������
		ʧ��X100mģ�鷢��ǻ��ֵ
��������
		Drv_X100mDisable
������
		p_unit��DRV_X100M_TYPEָ��
����ֵ����
*/
static void Drv_X100mDisable(DRV_X100M_TYPE *p_unit);

/*
������
		��ѯģ��汾��
��������
		Drv_X100mQueryVersion
������
		p_unit��DRV_X100M_TYPEָ��
����ֵ����
*/
static void Drv_X100mQueryVersion(DRV_X100M_TYPE *p_unit);

/*
������
		��ѯģ�黷��ѹ��
��������
		Drv_X100mPresH
������
		p_unit��DRV_X100M_TYPEָ��
����ֵ����
*/
static void Drv_X100mPresH(DRV_X100M_TYPE *p_unit);

/*
������
		��ѯģ�黷��ѹ��
��������
		Drv_X100mPresL
������
		p_unit��DRV_X100M_TYPEָ��
����ֵ����
*/
static void Drv_X100mPresL(DRV_X100M_TYPE *p_unit);

/*
������
		ģ���жϷ�����
��������
		Drv_X100m1_IRQHandler
������
		��
����ֵ����
*/
static void Drv_X100m1_IRQHandler(void);

#endif
