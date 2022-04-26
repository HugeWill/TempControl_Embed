/**************************************************

			
			嵌入式开发平台：PID温控模块应用层代码
			
			Func_TempControl.c
			
			功能描述：
			
				
****************************************************/

#include "Func_TempControl.h"

//PID初始参数
#define KP_INIT0 100.0
#define KI_INIT0 0.0
#define KD_INIT0 0.0
//常数PI
//#define PI 3.1415926
#define ON (1)
#define OFF (0)
//NTC电阻计算温度相关的常数
const float R25 = 10000.0;  /*NTC电阻标称阻值10k*/
const float T2 = (273.15+25.0); /*开氏室温*/
const float B = 3435.0;    /*热敏系数*/
const float Ka = 273.15; /*温度常数*/


//目标温度默认值
float GOALTEMP = 12.0;
double Cur_temp[7];



//定义了PID参数自整定结构体指针
static DRV_PIDTUNINGPARA_TYPE _g_PIDTUNNING[COLD_MAX + HEAT_MAX] = {NULL};


//参数整定状态信号量
OS_EVENT* _gp_PIDTUNNINGFINISH;
OS_EVENT* _gp_ColdStart;
OS_EVENT*	_gp_FanOFF;
volatile uint8_t _g_Motor = 0;				//0:close ,1:open
static uint32_t _g_pumpstarttime = 0;
volatile uint8_t _g_TempReport = 0;		//0:close ,1:open
uint16_t back_delay = 0;
static OS_TMR *SofeTimer[4] = {NULL}; /*?????????*/
/*?????1????*/
typedef void Func(void *ptmr,void *p_arg);
Func* SoftTimerCallBack[4];
void SofeTimer1CallBack(void *ptmr,void *p_arg);
void SofeTimer2CallBack(void *ptmr,void *p_arg);
void SofeTimer3CallBack(void *ptmr,void *p_arg);
void SofeTimer4CallBack(void *ptmr,void *p_arg);

TEMP_PARA_TYPE _g_ControlParameter = {0x11, 1, 1, 2, 6,22.0*100};
//X100_PARAMETER _g_X100_Para[5] = {{0x11,0,165951226655946,19},\
//																	{0x11,-1,145656862190146,15},\
//																	{0x11,29659,1966092086,10},\
//																	{0x11,-341257080,27809,6},\
//																	{0x11,1472431868192,86,2}};
MOTOR_SPEED_CUREV _g_Motor_Curve = {0x11,50,300,2,40,5000};
FIBERX100M_PARA_TYPE _g_X100 = {0x11,0.0000165951426655946,-1.14565686219046,29659.1966092086,-341257080.027809,1472431868192.86};

volatile LIQUID_STATUS_ENUM _g_Liquid_Status = LIQUID_NULL;			//0:NULL,1:Fill,2:Back
bool Func_PrivateInit1(DList* dlist)
{
	Func_TempInit();
	
	 /*公共指令注册*/
	Cmd_AddCmdNode(dlist, CMD_HANDSHAKE,			Func_Cmd_Com_Hello);			\
	Cmd_AddCmdNode(dlist, CMD_DCPUMPCONTROL,  Func_Cmd_Com_Pump_Control);
	Cmd_AddCmdNode(dlist, CMD_STEPPUMPCONTROL,  Func_Cmd_Com_StepPump_Control);
	Cmd_AddCmdNode(dlist, CMD_FANCONTROL,  Func_Cmd_Com_Fan_Control);
	Cmd_AddCmdNode(dlist, CMD_VALVECONTROL, Func_Cmd_Com_Valve_Control);
	Cmd_AddCmdNode(dlist, CMD_COLDING,  Func_Cmd_Com_Freezer_Control);
	Cmd_AddCmdNode(dlist, CMD_HEATING,  Func_Cmd_Com_Heater_Control);
	Cmd_AddCmdNode(dlist, CMD_READLIQUID,  Func_Cmd_Com_Liquid_Read);
	Cmd_AddCmdNode(dlist, CMD_READTEMP,  Func_Cmd_Com_Temp_Read);
	Cmd_AddCmdNode(dlist, CMD_LEDCONTROL,  Func_Cmd_Com_Led_Control);
	Cmd_AddCmdNode(dlist, CMD_INQUERYVERSION,  Func_Cmd_Com_Inquiry_Version);
	/*私有命令注册*/
	Cmd_AddCmdNode(dlist, CMD_FIBERPARA_RW,  Func_X100_Para_RW);
	Cmd_AddCmdNode(dlist, CMD_TEMP_PARA_RW,  Func_Temp_Para_RW);
	Cmd_AddCmdNode(dlist, CMD_LIQUID_FILL,  Func_Liquid_Fill);
	Cmd_AddCmdNode(dlist, CMD_LIQUID_NULL,  Func_Liquid_Back);
	Cmd_AddCmdNode(dlist, CMD_STEMPUMP_CONTROL, Func_Step_Pump_Switch);
	Cmd_AddCmdNode(dlist, CMD_TEMP_REPORTSWITCH, Func_Temp_Report_Switch);
	Cmd_AddCmdNode(dlist, CMD_FREEZER_CURRENT_INQUIRY, Func_Freezer_Current_Inquiry);
	return true;
}

/*
		TempInit
*/
static bool Func_TempInit()
{
	uint8_t temp = 0;
	uint8_t i;
	REPORT_EVENT_DATA_TYPE* p_return = (REPORT_EVENT_DATA_TYPE*)malloc(sizeof(REPORT_EVENT_DATA_TYPE));
	RETURN_ERR_DATA_TYPE* p_err = (RETURN_ERR_DATA_TYPE*)malloc(sizeof(RETURN_ERR_DATA_TYPE));
	if(p_return == NULL)
		return false;
	if(p_err == NULL)
		return false;
	_gp_E = At24c32Init(AT24C32_1, _PB_, _P6_, _PB_, _P5_);	/*EEP*/
	_gp_ColdStart = OSSemCreate(0);
	_gp_FanOFF = OSSemCreate(0);
	temp = At24c32ReadByte(_gp_E,1*32);
	if(temp == 0x11) At24c32ReadPage(_gp_E,1,(uint8_t*)(&_g_ControlParameter));
	temp = At24c32ReadByte(_gp_E,10*32);
	if(temp == 0x11) At24c32ReadPage(_gp_E,10,(uint8_t*)(&_g_Motor_Curve));
	temp = At24c32ReadByte(_gp_E,20*32);
	if(temp == 0x11) At24c32ReadPage(_gp_E,20,(uint8_t*)(&_g_X100));
	
	/*StepPumpInit*/

	_gp_StepPump = MCC_Init(MCC_1,2,1.8,360,360,MCC_F,MCC_RESET_POSITIVE,MCC_SHELTER_YES,\
													MCC_SHELTER_NO,MCC_SHELTER_NO,\
													_PORT_BUTT_,_PIN_BUTT_,\
													_PORT_BUTT_,_PIN_BUTT_,\
													_PORT_BUTT_,_PIN_BUTT_,
													_PORT_BUTT_,_PIN_BUTT_,
													_PE_,_P8_,
													_PE_,_P7_,
													_PE_,_P9_);
	BSP_Init_Pin(_PE_,_P10_,_OUT_PP_);
	BSP_WritePin(_PE_,_P10_,1);
	BSP_WritePin(_PE_,_P8_,1);
	if(_gp_StepPump == NULL)
		return false;
	//Default Speed
	MCC_SetStempMotorLine5(_gp_StepPump,_g_Motor_Curve.start_fre,_g_Motor_Curve.end_fre,\
												_g_Motor_Curve.each_lader_step,_g_Motor_Curve.lader_num,_g_Motor_Curve.s_para,0);
	MCC_SetStempMotorLine5(_gp_StepPump,_g_Motor_Curve.start_fre,_g_Motor_Curve.end_fre,\
												_g_Motor_Curve.each_lader_step,_g_Motor_Curve.lader_num,_g_Motor_Curve.s_para,1);
	//Valve Initial
	_gp_V = Valve_Init(VALVE_1,VALVE_NORMAL_CLOSE,VALVE_POSITIVE_LOGIC,_PC_,_P7_);
	if(_gp_V == NULL)
		return false;
	Valve_Close(_gp_V);
	//DCPump Initial
	_gp_P[0] = PumpInit(PUMP_1, PUMP_UNADJUSTABLE_SPEED,\
											PUMP_POSITIVE_LOGIC,10000,100,\
											_T7_,_PC_,_P8_,_PORT_BUTT_,_PIN_BUTT_);   //DCPUMP1
	_gp_P[1] = PumpInit(PUMP_2, PUMP_UNADJUSTABLE_SPEED, \
											PUMP_POSITIVE_LOGIC,10000,100,\
											_T7_,_PC_,_P9_,_PORT_BUTT_,_PIN_BUTT_);	 //DCPUMP2
	_gp_P[2] = PumpInit(PUMP_3, PUMP_UNADJUSTABLE_SPEED, \
											PUMP_POSITIVE_LOGIC,10000,100,\
											_T7_,_PD_,_P1_,_PORT_BUTT_,_PIN_BUTT_);	 //DCPUMP3

	for(i=0;i<DCPUMP_MAX;i++)
	{
		if(_gp_P[i] == NULL)
			return false;
		PumpStop(_gp_P[i]);
	}
	//FAN INIT
	_gp_Fan[0] = Drv_FanInit(FAN_1,FAN_POSITIVE_LOGIC,_PD_,_P7_);			
	_gp_Fan[1] = Drv_FanInit(FAN_2,FAN_POSITIVE_LOGIC,_PD_,_P6_);
	_gp_Fan[2] = Drv_FanInit(FAN_3,FAN_POSITIVE_LOGIC,_PD_,_P5_);
	_gp_Fan[3] = Drv_FanInit(FAN_4,FAN_POSITIVE_LOGIC,_PD_,_P4_);
	_gp_Fan[4] = Drv_FanInit(FAN_5,FAN_POSITIVE_LOGIC,_PD_,_P3_);
	for(i=0;i<FAN_MAX;i++)
	{
		if(_gp_Fan[i] == NULL)
			return false;
		else
			Drv_FanOFF(_gp_Fan[i]);
	}
	Drv_FanON(_gp_Fan[4]);
	//cold
	_gp_C[0] = Drv_RefrigeratingInit(REFRIGERATING_1,_PE_,_P15_,_ADC1_,_PB_,_P0_,10);
	_gp_C[1] = Drv_RefrigeratingInit(REFRIGERATING_2,_PE_,_P14_,_ADC1_,_PA_,_P6_,10);
	_gp_C[2] = Drv_RefrigeratingInit(REFRIGERATING_3,_PE_,_P13_,_ADC1_,_PA_,_P4_,10);
	_gp_C[3] = Drv_RefrigeratingInit(REFRIGERATING_4,_PE_,_P12_,_ADC1_,_PA_,_P2_,10);
	for(i=0;i<COLD_MAX;i++)
	{
		if(_gp_C[i] == NULL)
			return false;
		else
			Drv_RefrigeratingClose(_gp_C[i]);
	}
	//heat
	_gp_H[0] = Drv_HeatInit(HEAT_1,_PC_,_P10_);
	_gp_H[1] = Drv_HeatInit(HEAT_2,_PC_,_P11_);
	for(i=0;i<HEAT_MAX;i++)
	{
		if(_gp_H[i] == NULL)
			return false;
		Drv_HeatClose(_gp_H[i]);
	}
	//NTC
	_gp_N[0] = Drv_NtcTempInit(NTC_1,_ADC1_,_PC_,_P5_,10,10,B,3.3,0.1);
	_gp_N[1] = Drv_NtcTempInit(NTC_2,_ADC1_,_PC_,_P4_,10,10,B,3.3,0.1);
	_gp_N[2] = Drv_NtcTempInit(NTC_3,_ADC1_,_PC_,_P3_,10,10,B,3.3,0.1);
	_gp_N[3] = Drv_NtcTempInit(NTC_4,_ADC1_,_PC_,_P2_,10,10,B,3.3,0.1);						//coldmetalboard2
	_gp_N[4] = Drv_NtcTempInit(NTC_5,_ADC1_,_PC_,_P1_,10,10,B,3.3,0.1);						//coldmetalboard1
	_gp_N[5] = Drv_NtcTempInit(NTC_6,_ADC1_,_PC_,_P0_,10,10,B,3.3,0.1);						//loopTubeSensor
	for(i=0;i<TEMP_SENSOR_MAX;i++)
	{
		if(_gp_N[i] == NULL)
			return false;
	}
	//LiquidSensor
	_gp_S[0] = Drv_SwitchingSensorInit(SWITCHINGSENSOR_1,FALLING,10,_PB_,_P7_);		//L
	_gp_S[1] = Drv_SwitchingSensorInit(SWITCHINGSENSOR_2,FALLING,10,_PB_,_P8_);		//H
	if(BSP_ReadPin(_gp_S[0]->port_number,_gp_S[0]->pin_number) == DOWN || BSP_ReadPin(_gp_S[1]->port_number,_gp_S[1]->pin_number) == DOWN)
	{
		p_err->err_code = ERR_LIQUID_INIT_ERR;
		p_err->module_id = MODULE_LIQUID;
		p_err->device_id = LIQUIDE_MAX;
		_Drv_UsartReportErrToBuffer(0,0,sizeof(RETURN_ERR_DATA_TYPE),(uint8_t*)p_err);
		free(p_err);
		free(p_return);
		OSTimeDlyHMSM(0,0,0,500);
//		return false;
		
	}
	for(i=0;i<LIQUIDE_MAX;i++)
	{
		if(_gp_S[i] == NULL)
			return false;
	}
	//X100
	_gp_X = Drv_X100mInit(X100M_1,57600,_USART3_,_PB_,_P10_,_PB_,_P11_,_g_X100.a,_g_X100.b,_g_X100.c,_g_X100.d,_g_X100.e);
	if(_gp_X == NULL)
		return false;
	//BUTTON
//	_gp_B[0] = Drv_SwitchingSensorInit(SWITCHINGSENSOR_3,RISING,10,_PD_,_P14_);
//	_gp_B[1] = Drv_SwitchingSensorInit(SWITCHINGSENSOR_4,RISING,10,_PD_,_P13_);
	_gp_B[SS1_BUTTON] = Drv_SwitchingSensorInit(SWITCHINGSENSOR_5,RISING,10,_PD_,_P15_);
//	_gp_B[3] = Drv_SwitchingSensorInit(SWITCHINGSENSOR_6,RISING,10,_PD_,_P11_);
//	_gp_B[4] = Drv_SwitchingSensorInit(SWITCHINGSENSOR_7,RISING,10,_PD_,_P10_);
	_gp_B[SS2_BUTTON] = Drv_SwitchingSensorInit(SWITCHINGSENSOR_8,RISING,10,_PD_,_P12_);
	for(i=0;i<SS_MAX;i++)
	{
		if(_gp_B[i] == NULL)
			return false;
	}
	//LED
	_gp_LED[LED_RED] = Led_Init(LED_2,LED_POSITIVE_LOGIC,_PD_,_P8_);
	_gp_LED[LED_GREEN] = Led_Init(LED_3,LED_POSITIVE_LOGIC,_PD_,_P9_);
	_gp_LED[LED_YELLOW] = Led_Init(LED_4,LED_POSITIVE_LOGIC,_PD_,_P0_);
	for(i=0;i<LED_MAX;i++)
	{
		if(_gp_LED[i] == NULL)
			return false;
	}
	Led_On(_gp_LED[LED_GREEN]);
	//PID
	_g_PIDTUNNING[COLD1].pid	= Drv_PidInit(PID_1,_g_ControlParameter.goal_temp_multiply100/100.0,KP_INIT0,KI_INIT0,KD_INIT0);
	_g_PIDTUNNING[COLD2].pid	= Drv_PidInit(PID_2,_g_ControlParameter.goal_temp_multiply100/100.0,KP_INIT0,KI_INIT0,KD_INIT0);
	_g_PIDTUNNING[COLD3].pid	= Drv_PidInit(PID_3,_g_ControlParameter.goal_temp_multiply100/100.0,KP_INIT0,KI_INIT0,KD_INIT0);
	_g_PIDTUNNING[COLD4].pid	= Drv_PidInit(PID_4,_g_ControlParameter.goal_temp_multiply100/100.0,KP_INIT0,KI_INIT0,KD_INIT0);
//	_g_PIDTUNNING[COLD_MAX+HEAT1].pid	= Drv_PidInit(PID_5,_g_ControlParameter.goal_temp_multiply100/100.0,KP_INIT0,KI_INIT0,KD_INIT0);
//	_g_PIDTUNNING[COLD_MAX+HEAT2].pid	= Drv_PidInit(PID_6,_g_ControlParameter.goal_temp_multiply100/100.0,KP_INIT0,KI_INIT0,KD_INIT0);
	for(i = 0;i<COLD_MAX;i++)
	{
		Drv_PidTuningParaInit(&_g_PIDTUNNING[i], _g_PIDTUNNING[i].pid, _gp_C[i], _Func_FreezerOFF, _Func_FreezerON); /*??????*/
	}
//	for(i=0;i<HEAT_MAX;i++)
//	{
//		Drv_PidTuningParaInit(&_g_PIDTUNNING[i+COLD_MAX], _g_PIDTUNNING[i+COLD_MAX].pid, _gp_H[i], _Func_HeaterON, _Func_HeaterOFF); /*??????*/
//	}
	SoftTimerCallBack[0] = SofeTimer1CallBack;
	SoftTimerCallBack[1] = SofeTimer2CallBack;
	SoftTimerCallBack[2] = SofeTimer3CallBack;
	SoftTimerCallBack[3] = SofeTimer4CallBack;
	//return success
	p_return->event_id = EVENT_INIT_SUCCESS;
	p_return->rsv[0] = 0;
	p_return->rsv[1] = 0;
	_Drv_UsartReportEventToBuffer(0,0,sizeof(REPORT_EVENT_DATA_TYPE),(uint8_t*)p_return);
	free(p_return);
	free(p_err);
	return true;
}

/*ReportEventTask*/
void Task1()
{
	uint8_t i = 0;
	uint8_t err = OS_ERR_NONE;
	REPORT_EVENT_DATA_TYPE* p_msg = (REPORT_EVENT_DATA_TYPE*)malloc(sizeof(REPORT_EVENT_DATA_TYPE));
	RETURN_ERR_DATA_TYPE* p_err = (RETURN_ERR_DATA_TYPE*)malloc(sizeof(RETURN_ERR_DATA_TYPE));
	static uint8_t event1=0;
	static uint8_t event2= 0;
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,50);
		if(_gp_S[LIQUIDE1]->buffer == DOWN && _gp_S[LIQUIDE2]->buffer == DOWN)
		{
			if(_g_Liquid_Status == LIQUID_FILL)
			{
				_g_Liquid_Status = LIQUID_NULL;
				PumpStop(_gp_P[DCPUMP1]);
				p_msg->event_id = EVENT_LIQUID_SENSOR_L;
				p_msg->rsv[0] = 0;
				p_msg->rsv[1] = 0;
				_Drv_UsartReportEventToBuffer(0,0,sizeof(REPORT_EVENT_DATA_TYPE),(uint8_t*)p_msg);
				PumpStart(_gp_P[DCPUMP3]);
				OSSemPost(_gp_ColdStart);
			}
		}
		else if(_gp_S[LIQUIDE1]->buffer == UP && _gp_S[LIQUIDE2]->buffer == UP)
		{
			if(_g_Liquid_Status == LIQUID_BACK)
			{
				_g_Liquid_Status = LIQUID_NULL;
				PumpStop(_gp_P[DCPUMP2]);
				Valve_Close(_gp_V);
				p_msg->event_id = EVENT_LIQUID_SENSOR_H;
				p_msg->rsv[0] = 0;
				p_msg->rsv[1] = 0;
				_Drv_UsartReportEventToBuffer(0,0,sizeof(REPORT_EVENT_DATA_TYPE),(uint8_t*)p_msg);
			}
		}
		else if(_gp_S[LIQUIDE1]->buffer == UP && _gp_S[LIQUIDE2]->buffer == DOWN)
		{
			if(_g_Liquid_Status == LIQUID_ERR)
			{
				_g_Liquid_Status = LIQUID_NULL;
				PumpStop(_gp_P[DCPUMP1]);
				PumpStop(_gp_P[DCPUMP2]);
				Valve_Close(_gp_V);
			}
			if(_g_Liquid_Status == LIQUID_BACK)
			{
				back_delay++;
				if(back_delay >=6000 )
				{
					back_delay = 0;
					_g_Liquid_Status = LIQUID_NULL;
					PumpStop(_gp_P[DCPUMP2]);
					Valve_Close(_gp_V);
					p_msg->event_id = EVENT_LIQUID_SENSOR_H;
					p_msg->rsv[0] = 0;
					p_msg->rsv[1] = 0;
					_Drv_UsartReportEventToBuffer(0,0,sizeof(REPORT_EVENT_DATA_TYPE),(uint8_t*)p_msg);
				}
			}
		}
		else
		{	
			if(_g_Liquid_Status != LIQUID_ERR)
			{
				_g_Liquid_Status = LIQUID_ERR;
				PumpStop(_gp_P[DCPUMP1]);
				PumpStop(_gp_P[DCPUMP2]);
				Valve_Close(_gp_V);
				p_err->err_code = ERR_LIQUID_SENSOR;
				p_err->module_id = MODULE_LIQUID;
				p_err->device_id = LIQUIDE_MAX;
				_Drv_UsartReportErrToBuffer(0,0,sizeof(RETURN_ERR_DATA_TYPE),(uint8_t*)p_err);
			}
		}
		for(i=0;i<SS_MAX;i++)
		{
			OSSemPend(_gp_B[i]->sem,10,&err);
			if(err!=OS_ERR_NONE)
				err = OS_ERR_NONE;
			else
			{
				switch(i)
				{
					case SS1_BUTTON:
					{
						p_msg->event_id = EVENT_SS1_BUTTON;
						p_msg->rsv[0] = 0;
						p_msg->rsv[1] = 0;
						_Drv_UsartReportEventToBuffer(0,0,sizeof(REPORT_EVENT_DATA_TYPE),(uint8_t*)p_msg);
						break;
					}
					case SS2_BUTTON:
					{
						p_msg->event_id = EVENT_SS2_BUTTON;
						_Drv_UsartReportEventToBuffer(0,0,sizeof(REPORT_EVENT_DATA_TYPE),(uint8_t*)p_msg);
						break;
					}
					default:
						break;
				}
				break;
			}
		}
		for(i=0;i<COLD_MAX;i++)
		{
			Drv_RefChaAndCalLoop();
			if(Drv_RefrigeratingIsOpen(_gp_C[i]))
			{
				if(_gp_C[i]->Ic <= 1.0)
				{
					p_err->err_code = ERR_CLODX_ERR;
					p_err->module_id = MODULE_COLDING;
					p_err->device_id = i;
					_Drv_UsartReportErrToBuffer(0,0,sizeof(RETURN_ERR_DATA_TYPE),(uint8_t*)p_err);
					_g_ControlParameter.pid_switch = 0;
					_g_ControlParameter.speed = COLD_MAX;
					_Func_FreezerOFF();
				}
			}
		}
	}
}
/*ReportErrorTask*/
void Task2()
{
	uint8_t i=0;
	RETURN_ERR_DATA_TYPE* p_err = (RETURN_ERR_DATA_TYPE*)malloc(sizeof(RETURN_ERR_DATA_TYPE));
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,50);
		Drv_NtcChaAndCalLoop();
//		Cur_temp[0] = Drv_GetX100mTemp(_gp_X);
		for(i=0;i<TEMP_SENSOR_MAX;i++)
		{	
			Cur_temp[i+1] = Drv_GetNtcTemp(_gp_N[i]);
			if(_g_ControlParameter.pid_switch == 1)
			{
//				if(Cur_temp[i] <= 0||Cur_temp[i] > 60)
//				{
//					p_err->err_code = ERR_SENSOR4 - i;
//					if(0 == i)
//					{
//						p_err->module_id = MODULE_TEMPX100;
//						p_err->module_id = FIBER1;
//					}
//					else
//					{
//						p_err->module_id = MODULE_TEMPSENSOR;
//						p_err->device_id = (TEMP_SENSOR_ENUM)(i - 1);
//					}
//					_Drv_UsartReportErrToBuffer(0,0,sizeof(RETURN_ERR_DATA_TYPE),(uint8_t*)p_err);
////					_g_ControlParameter.pid_switch = 0;
//					_Func_FreezerOFF();
//					_Func_HeaterOFF();
//				}
			}
		}
		if(Cur_temp[6] < 10.0 || Cur_temp[6] >50.0)
		{
			p_err->err_code = 	ERR_TEMP6;
			p_err->module_id = MODULE_TEMPSENSOR;
			p_err->device_id = TEMP_SENSOR6;
			_Drv_UsartReportErrToBuffer(0,0,sizeof(RETURN_ERR_DATA_TYPE),(uint8_t *)p_err);
//			_g_ControlParameter.pid_switch = 0;
			_Func_FreezerOFF();
			_Func_HeaterOFF();
		}
		if(Cur_temp[5] < 5.0 || Cur_temp[5] > 70.0)
		{
			p_err->err_code = 	ERR_TEMP5;
			p_err->module_id = MODULE_TEMPSENSOR;
			p_err->device_id = TEMP_SENSOR5;
			_Drv_UsartReportErrToBuffer(0,0,sizeof(RETURN_ERR_DATA_TYPE),(uint8_t *)p_err);
//			_g_ControlParameter.pid_switch = 0;
			_Func_FreezerOFF();
			_Func_HeaterOFF();
		}
		if(Cur_temp[4] < 5.0 || Cur_temp[4] > 70.0)
		{
			p_err->err_code = 	ERR_TEMP4;
			p_err->module_id = MODULE_TEMPSENSOR;
			p_err->device_id = TEMP_SENSOR4;
			_Drv_UsartReportErrToBuffer(0,0,sizeof(RETURN_ERR_DATA_TYPE),(uint8_t *)p_err);
//			_g_ControlParameter.pid_switch = 0;
			_Func_FreezerOFF();
			_Func_HeaterOFF();
		}

	}
}
void TaskResident1()
{
	while(1)
	{
		if(_g_Motor == 1)
		{
			MCC_Move_Distance(_gp_StepPump,!_gp_StepPump->reset_dir,3600,10000,MCC_MOVE_DISTANCE,0,MCC_LINE_SLOW);
		}
		OSTimeDlyHMSM(0,0,0,10);
		switch(_g_Liquid_Status)
		{
			case LIQUID_NULL:
				break;
			case LIQUID_FILL:
			{
				PumpStop(_gp_P[DCPUMP2]);
				Valve_Close(_gp_V);
				PumpStart(_gp_P[DCPUMP1]);
				break;
			}
			case LIQUID_BACK:
			{
				PumpStop(_gp_P[DCPUMP1]);
				PumpStart(_gp_P[DCPUMP2]);
				Valve_Open(_gp_V);
				break;
			}
			default:
			{
				PumpStop(_gp_P[DCPUMP1]);
				PumpStop(_gp_P[DCPUMP2]);
				Valve_Close(_gp_V);
				break;
			}
		}
	}
}
void TaskResident2()
{
	while(1)
	{
		Drv_AllSwitchingSensorDebounce_Loop();
	}
}
void Task3()
{
	uint8_t i;
	uint8_t err;
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 50);
		if(PumpIsStart(_gp_P[DCPUMP3]))   //freeze loop pump is open
		{
			if(_g_Motor == 1)
			{
				_g_pumpstarttime++;
			}
			else
			{
				_g_pumpstarttime = 0;
				_g_ControlParameter.current_ch = 6;				/*default channel is colder liquid*/
			}
			if(_g_pumpstarttime>12000)
				_g_ControlParameter.current_ch = 0;				/*goal temp switch to X100*/
			if(_g_ControlParameter.pid_switch == 1)
			{
				if(_g_ControlParameter.mode == 0)
				{
					for(i = 0; i < _g_ControlParameter.speed; i++)
					{ 
						if(Drv_PidTuning(&_g_PIDTUNNING[i], Cur_temp[_g_ControlParameter.current_ch]) == END)
						{
							SofeTimer[i] = OSTmrCreate(10,1,OS_TMR_OPT_PERIODIC,SoftTimerCallBack[i], (void *)0, (INT8U *)"timer1",&err);
							OSTmrStart(SofeTimer[i],&err);
						}
					}
				}
				if(_g_ControlParameter.mode == 1)
				{
					if(_g_ControlParameter.goal_temp_multiply100/100 > Cur_temp[_g_ControlParameter.current_ch])
					{
						_Func_HeaterON();
					}
					else
					{
						_Func_HeaterOFF();
					}
				}
//				if(_g_ControlParameter.mode == 1)
//				{
//					for(i = 0; i < _g_ControlParameter.speed; i++)
//					{ 
//						if(Drv_PidTuning(&_g_PIDTUNNING[i+COLD_MAX], Cur_temp[_g_ControlParameter.current_ch]) == END)
//						{
//							SofeTimer[i] = OSTmrCreate(10,1,OS_TMR_OPT_PERIODIC,SoftTimerCallBack[i], (void *)0, (INT8U *)"timer1",&err);
//							OSTmrStart(SofeTimer[i],&err);
//						}
//					}
//				}
//				OSTimeDlyHMSM(0, 0, 1, 0);
			}
			else
			{
				OSTimeDlyHMSM(0, 0, 1, 0);
			}
	}
}
}
void Task4()
{
	uint8_t i = 0;
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,100);
		if(_g_ControlParameter.pid_switch)
		{
			if(_g_ControlParameter.mode == 0)					//freezer
			{
				for(i = 0; i < _g_ControlParameter.speed; i++)
				{
					if(_g_PIDTUNNING[i].pid->status == PID_WORKING && _g_PIDTUNNING[i].constatus == PID_CAL) /*PID??*/
					{
						_g_PIDTUNNING[i].duty = Drv_PidCalculate(_g_PIDTUNNING[i].pid,Cur_temp[_g_ControlParameter.current_ch]);
						_g_PIDTUNNING[i].constatus = PID_CON;
					}
				}
			}
//			if(_g_ControlParameter.mode == 1)
//			{
//				for(i = 0; i < _g_ControlParameter.speed; i++)
//				{
//					if(_g_PIDTUNNING[i+COLD_MAX].pid->status == PID_WORKING && _g_PIDTUNNING[i+COLD_MAX].constatus == PID_CAL) /*PID??*/
//					{
//						_g_PIDTUNNING[i+COLD_MAX].duty = Drv_PidCalculate(_g_PIDTUNNING[i+COLD_MAX].pid,Cur_temp[_g_ControlParameter.current_ch]);
//						_g_PIDTUNNING[i+COLD_MAX].constatus = PID_CON;
//					}
//				}
//			
//			}
//			OSTimeDlyHMSM(0, 0, 0, 200);
		}
		else
		{
			OSTimeDlyHMSM(0, 0, 1, 0);
		}
	}
}

void Task5()
{
	uint8_t err = OS_ERR_NONE;
	uint8_t i;
	while(1)
	{
//		OSTimeDlyHMSM(0,0,0,100);
		OSSemPend(_gp_FanOFF,0,&err);
		if(_g_ControlParameter.pid_switch == 1)
		{
			OSTimeDlyHMSM(0,1,0,0);
			for(i=0;i<4;i++)
			{
				if(!Drv_RefrigeratingIsOpen(_gp_C[i]))
					Drv_FanOFF(_gp_Fan[i]);
			}
		}
		else
		{
			OSTimeDlyHMSM(0,5,0,0);
			for(i=0;i<4;i++)
			{
				if(!Drv_RefrigeratingIsOpen(_gp_C[i]))
					Drv_FanOFF(_gp_Fan[i]);
			}
		}
	}
}
void Task6()
{
	uint8_t i = 0;
	TEMP_REPORT_DATA_TYPE* p_return = (TEMP_REPORT_DATA_TYPE*)malloc(sizeof(TEMP_REPORT_DATA_TYPE));
	p_return->event_id = EVENT_TEMP_REPORT;
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,100);
		if(_g_TempReport)
		{
			for(i=0;i<7;i++)
			{
				p_return->temp[i] = (uint32_t)Cur_temp[i]*100;
			}
			p_return->rsv[0] = 0;
			p_return->rsv[1] = 0;
			_Drv_UsartReportEventToBuffer(0,0,sizeof(TEMP_REPORT_DATA_TYPE),(uint8_t*)p_return);
		}
	}
}

/* ??????
  ------------------------------
  ???:true,??;false,??
*/
bool Func_Cmd_Com_StepPump_Control(void* p_buffer)
{
	uint8_t i = 0;
	COMMON_CMD_DATA* p_msg = (COMMON_CMD_DATA*)p_buffer;
	STEPPUMPCONTROL_TYPE* p_data = (STEPPUMPCONTROL_TYPE*)p_msg->data;
	COMMON_RETURN_DATA_TYPE* p_return = (COMMON_RETURN_DATA_TYPE*)malloc(sizeof(COMMON_RETURN_DATA_TYPE));
	RETURN_ERR_DATA_TYPE* p_err = (RETURN_ERR_DATA_TYPE*)malloc(sizeof(RETURN_ERR_DATA_TYPE));
	
	uint32_t frame_id = p_msg->frame_head;
	uint16_t cmd = p_data->cmd;
	p_return->cmd = cmd;
	p_err->err_code = ERR_CMD_FAIL;
	p_err->module_id = MODULE_STEPPUMP;
	p_err->device_id = STEP_PUMP1;
	_g_Motor_Curve.start_fre = p_data->speed_start;
	_g_Motor_Curve.end_fre = p_data->speed_end;
	_g_Motor_Curve.each_lader_step = p_data->echlader_up;
	_g_Motor_Curve.lader_num = p_data->lader;
	_g_Motor_Curve.s_para = p_data->s_para;
	At24c32WritePage(_gp_E,10,(uint8_t*)(&_g_Motor_Curve));
	MCC_SetStempMotorLine5(_gp_StepPump,_g_Motor_Curve.start_fre,_g_Motor_Curve.end_fre,\
												_g_Motor_Curve.each_lader_step,_g_Motor_Curve.lader_num,\
												_g_Motor_Curve.s_para,0);
	MCC_SetStempMotorLine5(_gp_StepPump,_g_Motor_Curve.start_fre,_g_Motor_Curve.end_fre,\
												_g_Motor_Curve.each_lader_step,_g_Motor_Curve.lader_num,\
												_g_Motor_Curve.s_para,1);
	if(p_data->volumn>0&&p_data->volumn<0xFFFFFFFF)
		i = MCC_Move_Coordinates(_gp_StepPump,p_data->volumn/100.0,99000,MCC_MOVE_DISTANCE,0,MCC_LINE_SLOW);
	if(p_data->volumn == 0xFFFFFFFF)
		_g_Motor = 1;
	if(p_data->volumn == 0)
		_g_Motor = 0;
	if(i != OS_ERR_NONE)
	{
		_Drv_UsartReturnFailToBuffer(frame_id,cmd,sizeof(RETURN_ERR_DATA_TYPE),(uint8_t*)p_err);
		free(p_err);
		free(p_return);
		return false;
	}
	_Drv_UsartReturnDoneToBuffer(frame_id,cmd,sizeof(COMMON_RETURN_DATA_TYPE),(uint8_t*)p_return);
	free(p_err);
	free(p_return);
	return true;

}

/* ???????????
  ------------------------------
  ???:true,??;false,??
*/
bool Func_X100_Para_RW(void* p_buffer)
{
	COMMON_CMD_DATA* p_msg = (COMMON_CMD_DATA*)p_buffer;
	FIBERX100_PARA_RW* p_data = (FIBERX100_PARA_RW*)p_msg->data;
	COMMON_RETURN_DATA_TYPE* p_return = (COMMON_RETURN_DATA_TYPE*)malloc(sizeof(COMMON_RETURN_DATA_TYPE));
	
	uint32_t frame_id = p_msg->frame_head;
	uint16_t cmd = p_data->cmd;
	p_return->cmd = cmd;
	
	if(p_data->rw)
	{
		p_data->a = _g_X100.a;
		p_data->b = _g_X100.b;
		p_data->c = _g_X100.c;
		p_data->d = _g_X100.d;
		p_data->e = _g_X100.e;
//		p_data->a_int = _g_X100.;
//		p_data->b_int = _g_X100_Para[1].x_int;
//		p_data->c_int = _g_X100_Para[2].x_int;
//		p_data->d_int = _g_X100_Para[3].x_int;
//		p_data->e_int = _g_X100_Para[4].x_int;
//		
//		p_data->a_index = _g_X100_Para[0].x_index;
//		p_data->b_index = _g_X100_Para[1].x_index;
//		p_data->c_index = _g_X100_Para[2].x_index;
//		p_data->d_index = _g_X100_Para[3].x_index;
//		p_data->e_index = _g_X100_Para[4].x_index;
//		
//		p_data->a_frac = _g_X100_Para[0].x_frac;
//		p_data->b_frac = _g_X100_Para[1].x_frac;
//		p_data->c_frac = _g_X100_Para[2].x_frac;
//		p_data->d_frac = _g_X100_Para[3].x_frac;
//		p_data->e_frac = _g_X100_Para[4].x_frac;
		
		_Drv_UsartReturnDoneToBuffer(frame_id,cmd,sizeof(FIBERX100_PARA_RW),(uint8_t*)p_data);
		free(p_return);
		return true;
	}
	else
	{
		_g_X100.a = p_data->a;
		_g_X100.b = p_data->b;
		_g_X100.c = p_data->c;
		_g_X100.d = p_data->d;
		_g_X100.e = p_data->e;
		At24c32WritePage(_gp_E,20,(uint8_t*)(&_g_X100));
//		_g_X100_Para[0].x_int = p_data->a_int;
//		_g_X100_Para[1].x_int = p_data->b_int;
//		_g_X100_Para[2].x_int = p_data->c_int;
//		_g_X100_Para[3].x_int = p_data->d_int;
//		_g_X100_Para[4].x_int = p_data->e_int;
//		
//		_g_X100_Para[0].x_index = p_data->a_index;
//		_g_X100_Para[1].x_index = p_data->b_index;
//		_g_X100_Para[2].x_index = p_data->c_index;
//		_g_X100_Para[3].x_index = p_data->d_index;
//		_g_X100_Para[4].x_index = p_data->e_index;
//		
//		_g_X100_Para[0].x_frac = p_data->a_frac;
//		_g_X100_Para[1].x_frac = p_data->b_frac;
//		_g_X100_Para[2].x_frac = p_data->c_frac;
//		_g_X100_Para[3].x_frac = p_data->d_frac;
//		_g_X100_Para[4].x_frac = p_data->e_frac;
//		
//		for(i=0;i<5;i++)
//		{
//			At24c32WritePage(_gp_E,20+i,(uint8_t*)(&_g_X100_Para[i]));
//		}
	}

	_Drv_UsartReturnDoneToBuffer(frame_id,cmd,sizeof(COMMON_RETURN_DATA_TYPE),(uint8_t*)p_return);
	free(p_return);
	OSTimeDlyHMSM(0,0,0,50);			//wait for response to uppermachine
	BSP_Reboot();
	return true;

}

/* ????????
  ------------------------------
  ???:true,??;false,??
*/
bool Func_Temp_Para_RW(void* p_buffer)
{
	uint8_t i = 0;
	COMMON_CMD_DATA* p_msg = (COMMON_CMD_DATA*)p_buffer;
	CMD_TEMP_DATA* p_data = (CMD_TEMP_DATA*)p_msg->data;
	COMMON_RETURN_DATA_TYPE* p_return = (COMMON_RETURN_DATA_TYPE*)malloc(sizeof(COMMON_RETURN_DATA_TYPE));
	RETURN_ERR_DATA_TYPE* p_err = (RETURN_ERR_DATA_TYPE*)malloc(sizeof(RETURN_ERR_DATA_TYPE));
	
	uint32_t frame_id = p_msg->frame_head;
	uint16_t cmd = p_data->cmd;
	p_return->cmd = cmd;
	p_err->err_code = ERR_CMD_FAIL;
	p_err->module_id = MODULE_TEMP_CONTROL;
	p_err->device_id = 0;
	
	if(p_data->rw)
	{
		p_data->mode = _g_ControlParameter.mode;
		p_data->pid_switch = _g_ControlParameter.pid_switch;
		p_data->goal_ch = _g_ControlParameter.current_ch;
		p_data->goal_temp_multiply100 = _g_ControlParameter.goal_temp_multiply100;
		p_data->speed = _g_ControlParameter.speed;
		
		_Drv_UsartReturnDoneToBuffer(frame_id,cmd,sizeof(CMD_TEMP_DATA),(uint8_t*)p_data);
		free(p_err);
		free(p_return);
		return true;
		
	}
	else
	{
		_g_ControlParameter.mode = p_data->mode;
		_g_ControlParameter.pid_switch = p_data->pid_switch;
		_g_ControlParameter.current_ch = p_data->goal_ch;
		_g_ControlParameter.goal_temp_multiply100 = p_data->goal_temp_multiply100;
		_g_ControlParameter.speed = p_data->speed;		
		i = At24c32WritePage(_gp_E,1,(uint8_t*)(&_g_ControlParameter));

		if(i != true)
		{
			_Drv_UsartReturnFailToBuffer(frame_id,cmd,sizeof(RETURN_ERR_DATA_TYPE),(uint8_t*)p_err);
			free(p_err);
			free(p_return);
			return false;
		}
		_Drv_UsartReturnDoneToBuffer(frame_id,cmd,sizeof(COMMON_RETURN_DATA_TYPE),(uint8_t*)p_return);
		free(p_err);
		free(p_return);
		return true;
	}
}

/* ?????
  ------------------------------
  ???:true,??;false,??
*/
bool Func_Liquid_Fill(void* p_buffer)
{
	COMMON_CMD_DATA* p_msg = (COMMON_CMD_DATA*)p_buffer;
	CMD_LIQUIDE_FILL_DATA* p_data = (CMD_LIQUIDE_FILL_DATA*)p_msg->data;
	COMMON_RETURN_DATA_TYPE* p_return = (COMMON_RETURN_DATA_TYPE*)malloc(sizeof(COMMON_RETURN_DATA_TYPE));

	uint32_t frame_head = p_msg->frame_head;
	uint16_t cmd = p_data->cmd;
	//Liquid Fill
	if(_g_Liquid_Status!= LIQUID_FILL)
	{
		_g_Liquid_Status = LIQUID_FILL;
		PumpStop(_gp_P[DCPUMP2]);
		Valve_Close(_gp_V);
		PumpStart(_gp_P[DCPUMP1]);
	}
	p_return->cmd = p_data->cmd;
  _Drv_UsartReturnDoneToBuffer(frame_head,cmd,sizeof(COMMON_RETURN_DATA_TYPE),(uint8_t *)p_return);
	free(p_return);
	return true;
}

/* ?????
  ------------------------------
  ???:true,??;false,??
*/
bool Func_Liquid_Back(void* p_buffer)
{
	COMMON_CMD_DATA* p_msg = (COMMON_CMD_DATA*)p_buffer;
	CMD_LIQUIDE_NULL_DATA* p_data = (CMD_LIQUIDE_NULL_DATA*)p_msg->data;
	COMMON_RETURN_DATA_TYPE* p_return = (COMMON_RETURN_DATA_TYPE*)malloc(sizeof(COMMON_RETURN_DATA_TYPE));

	uint32_t frame_head = p_msg->frame_head;
	uint16_t cmd = p_data->cmd;
	//Liquid back
	if(_g_Liquid_Status != LIQUID_BACK)
	{
		_g_Liquid_Status = LIQUID_BACK;
		PumpStop(_gp_P[DCPUMP3]);
		PumpStop(_gp_P[DCPUMP1]);
		PumpStart(_gp_P[DCPUMP2]);
		Valve_Open(_gp_V);
	}
	p_return->cmd = p_data->cmd;
  _Drv_UsartReturnDoneToBuffer(frame_head,cmd,sizeof(COMMON_RETURN_DATA_TYPE),(uint8_t *)p_return);
	free(p_return);
	return true;
}


/* ????????
  ------------------------------
  ???:true,??;false,??
*/
bool Func_Cmd_Com_Temp_Read(void* p_buffer)
{
	COMMON_CMD_DATA* p_msg = (COMMON_CMD_DATA*)p_buffer;
	TEMP_TYPE* p_data = (TEMP_TYPE*)p_msg->data;
	TEMP_RETURN_DATA_TYPE* p_return = (TEMP_RETURN_DATA_TYPE*)malloc(sizeof(TEMP_RETURN_DATA_TYPE));
	RETURN_ERR_DATA_TYPE* p_err = (RETURN_ERR_DATA_TYPE*)malloc(sizeof(RETURN_ERR_DATA_TYPE));
	
	uint32_t frame_id = p_msg->frame_head;
	uint16_t cmd = p_data->cmd;
	p_return->cmd = cmd;
	p_err->err_code = ERR_CMD_FAIL;
	if(p_data->temp_ch == 0)
		p_err->module_id = MODULE_TEMPX100;
	else
		p_err->module_id = MODULE_TEMPSENSOR;
	p_err->device_id = p_data->temp_ch&0xff;
	
	if(p_data->temp_ch<=TEMP_SENSOR_MAX)
	{
		p_return->temp_ch = p_data->temp_ch;
		p_return->temp_value = Cur_temp[p_data->temp_ch]*100;
	}
	else
	{
		_Drv_UsartReturnFailToBuffer(frame_id,cmd,sizeof(RETURN_ERR_DATA_TYPE),(uint8_t*)p_err);
		free(p_err);
		free(p_return);
		return false;
	}
	_Drv_UsartReturnDoneToBuffer(frame_id,cmd,sizeof(TEMP_RETURN_DATA_TYPE),(uint8_t*)p_return);
	free(p_err);
	free(p_return);
	return true;
}
/* ???????
  ------------------------------
  ???:true,??;false,??
*/
bool Func_Step_Pump_Switch(void* p_buffer)
{
	COMMON_CMD_DATA* p_msg = (COMMON_CMD_DATA*)p_buffer;
	CMD_STEPPUMP_SWITCH_DATA* p_data = (CMD_STEPPUMP_SWITCH_DATA*)p_msg->data;
	COMMON_REP_DATA* p_return = (COMMON_REP_DATA*)malloc(sizeof(COMMON_REP_DATA));

	uint32_t frame_id = p_msg->frame_head;
	uint16_t cmd = p_data->cmd;
	p_return->cmd = cmd;
	if(p_data->on_off)
		_g_Motor = 1;
	else
		_g_Motor = 0;
	_Drv_UsartReturnDoneToBuffer(frame_id,cmd,sizeof(COMMON_REP_DATA),(uint8_t*)p_return);
	free(p_return);
	return true;
	
}

/* ??????
  ------------------------------
  ???:true,??;false,??
*/
bool Func_Temp_Report_Switch(void* p_buffer)
{
	COMMON_CMD_DATA* p_msg = (COMMON_CMD_DATA*)p_buffer;
	CMD_TEMP_REPORT_SWITCH_DATA* p_data = (CMD_TEMP_REPORT_SWITCH_DATA*)p_msg->data;
	COMMON_REP_DATA* p_return = (COMMON_REP_DATA*)malloc(sizeof(COMMON_REP_DATA));

	uint32_t frame_id = p_msg->frame_head;
	uint16_t cmd = p_data->cmd;
	p_return->cmd = cmd;
	if(p_data->on_off)
		_g_TempReport = 1;
	else
		_g_TempReport = 0;
	_Drv_UsartReturnDoneToBuffer(frame_id,cmd,sizeof(COMMON_REP_DATA),(uint8_t*)p_return);
	free(p_return);
	return true;
}

/* ??????
  ------------------------------
  ???:true,??;false,??
*/
bool Func_Freezer_Current_Inquiry(void* p_buffer)
{
	COMMON_CMD_DATA* p_msg = (COMMON_CMD_DATA*)p_buffer;
	CMD_CURRENT_INQUIRY_DATA* p_data = (CMD_CURRENT_INQUIRY_DATA*)p_msg->data;
	FREEZER_CURRENT_RETURN_TYPE* p_return = (FREEZER_CURRENT_RETURN_TYPE*)malloc(sizeof(FREEZER_CURRENT_RETURN_TYPE));
	uint8_t i = 0;
	uint32_t frame_id = p_msg->frame_head;
	uint16_t cmd = p_data->cmd;
	p_return->cmd = cmd;
	Drv_RefChaAndCalLoop();
	for(i=0;i<COLD_MAX;i++)
	{
		p_return->current[i] = (uint16_t)_gp_C[i]->Ic*100;
	}
	p_return->rsv[0] = 0;
	p_return->rsv[1] = 0;
	_Drv_UsartReturnDoneToBuffer(frame_id,cmd,sizeof(FREEZER_CURRENT_RETURN_TYPE),(uint8_t*)p_return);
	free(p_return);
	return true;

}
/*FreezerControl*/
void _Func_FreezerON(void)
{
	uint8_t i;
	uint8_t num = _g_ControlParameter.speed;
	if(num>COLD_MAX)
		num = COLD_MAX;
	for(i=0;i<num;i++)
	{
		Drv_RefrigeratingOpen(_gp_C[i]);
		Drv_FanON(_gp_Fan[i]);
	}
}
void _Func_FreezerOFF(void)
{
	uint8_t i;
	uint8_t num = _g_ControlParameter.speed;
	if(num>COLD_MAX)
		num = COLD_MAX;
	for(i=0;i<num;i++)
	{
		Drv_RefrigeratingClose(_gp_C[i]);
//		Drv_FanOFF(_gp_Fan[i]);
		OSSemPost(_gp_FanOFF);
	}
}
/*HeaterControl*/
void _Func_HeaterON(void)
{
	uint8_t i;
	uint8_t count = _g_ControlParameter.speed;
	if(count>HEAT_MAX)
		count = HEAT_MAX;
	for(i=0;i<count;i++)
	{
		Drv_HeatOpen(_gp_H[i]);
	}
}
void _Func_HeaterOFF(void)
{
	uint8_t i;
	uint8_t count = _g_ControlParameter.speed;
	if(count>HEAT_MAX)
		count = HEAT_MAX;
	for(i=0;i<count;i++)
	{
		Drv_HeatClose(_gp_H[i]);
	}
}
/*?????1????*/
/*?????1????*/
void SofeTimer1CallBack(void *ptmr,void *p_arg)
{
	if(_g_ControlParameter.mode == 0)		//Freezer
	{
		if(_g_PIDTUNNING[COLD1].constatus == PID_CON)
		{
			if(_g_PIDTUNNING[COLD1].duty > _g_PIDTUNNING[COLD1].pertim)
				_g_PIDTUNNING[COLD1].positivefun();
			else
				_g_PIDTUNNING[COLD1].negativefun();
			_g_PIDTUNNING[COLD1].pertim -= 10;
			if(_g_PIDTUNNING[COLD1].pertim <= 0)
			{
				_g_PIDTUNNING[COLD1].pertim = FULLDUTY;
				_g_PIDTUNNING[COLD1].constatus = PID_CAL;
			}
		}
	}
//	if(_g_ControlParameter.mode == 1) 		//heater
//	{
//		if(_g_PIDTUNNING[HEAT1+COLD_MAX].constatus == PID_CON)
//		{
//			if(_g_PIDTUNNING[HEAT1+COLD_MAX].duty > _g_PIDTUNNING[HEAT1+COLD_MAX].pertim)
//				_g_PIDTUNNING[HEAT1+COLD_MAX].positivefun();
//			else
//				_g_PIDTUNNING[HEAT1+COLD_MAX].negativefun();
//			_g_PIDTUNNING[HEAT1+COLD_MAX].pertim -= 10;
//			if(_g_PIDTUNNING[HEAT1+COLD_MAX].pertim <= 0)
//			{
//				_g_PIDTUNNING[HEAT1+COLD_MAX].pertim = FULLDUTY;
//				_g_PIDTUNNING[HEAT1+COLD_MAX].constatus = PID_CAL;
//			}
//		}
//	}
}
void SofeTimer2CallBack(void *ptmr,void *p_arg)
{
	if(_g_ControlParameter.mode == 0)		//Freezer
	{
		if(_g_PIDTUNNING[COLD2].constatus == PID_CON)
		{
			if(_g_PIDTUNNING[COLD2].duty > _g_PIDTUNNING[COLD2].pertim)
				_g_PIDTUNNING[COLD2].positivefun();
			else
				_g_PIDTUNNING[COLD2].negativefun();
			_g_PIDTUNNING[COLD2].pertim -= 10;
			if(_g_PIDTUNNING[COLD2].pertim <= 0)
			{
				_g_PIDTUNNING[COLD2].pertim = FULLDUTY;
				_g_PIDTUNNING[COLD2].constatus = PID_CAL;
			}
		}
	}
//	if(_g_ControlParameter.mode == 1) 		//heater
//	{
//		if(_g_PIDTUNNING[HEAT2+COLD_MAX].constatus == PID_CON)
//		{
//			if(_g_PIDTUNNING[HEAT2+COLD_MAX].duty > _g_PIDTUNNING[HEAT2+COLD_MAX].pertim)
//				_g_PIDTUNNING[HEAT2+COLD_MAX].positivefun();
//			else
//				_g_PIDTUNNING[HEAT2+COLD_MAX].negativefun();
//			_g_PIDTUNNING[HEAT2+COLD_MAX].pertim -= 10;
//			if(_g_PIDTUNNING[HEAT2+COLD_MAX].pertim <= 0)
//			{
//				_g_PIDTUNNING[HEAT2+COLD_MAX].pertim = FULLDUTY;
//				_g_PIDTUNNING[HEAT2+COLD_MAX].constatus = PID_CAL;
//			}
//		}
//	}
}
void SofeTimer3CallBack(void *ptmr,void *p_arg)
{
	if(_g_ControlParameter.mode == 0)		//Freezer
	{
		if(_g_PIDTUNNING[COLD3].constatus == PID_CON)
		{
			if(_g_PIDTUNNING[COLD3].duty > _g_PIDTUNNING[COLD3].pertim)
				_g_PIDTUNNING[COLD3].positivefun();
			else
				_g_PIDTUNNING[COLD3].negativefun();
			_g_PIDTUNNING[COLD3].pertim -= 10;
			if(_g_PIDTUNNING[COLD3].pertim <= 0)
			{
				_g_PIDTUNNING[COLD3].pertim = FULLDUTY;
				_g_PIDTUNNING[COLD3].constatus = PID_CAL;
			}
		}
	}
}
void SofeTimer4CallBack(void *ptmr,void *p_arg)
{
	if(_g_ControlParameter.mode == 0)		//Freezer
	{
		if(_g_PIDTUNNING[COLD4].constatus == PID_CON)
		{
			if(_g_PIDTUNNING[COLD4].duty > _g_PIDTUNNING[COLD4].pertim)
				_g_PIDTUNNING[COLD4].positivefun();
			else
				_g_PIDTUNNING[COLD4].negativefun();
			_g_PIDTUNNING[COLD4].pertim -= 10;
			if(_g_PIDTUNNING[COLD4].pertim <= 0)
			{
				_g_PIDTUNNING[COLD4].pertim = FULLDUTY;
				_g_PIDTUNNING[COLD4].constatus = PID_CAL;
			}
		}
	}
}
