#include "Func_TC.h"

/*------------------------PID控制整理----------------------------*/
/*PID三个参数*/ 
#define KP_INIT0 0.0f
#define KI_INIT0 0.0f
#define KD_INIT0 0.0f

uint8_t SwitchTemp = 1; /*默认开启*/

static OS_TMR *SofeTimer1 = NULL; /*操作系统软件定时器*/

//定义了加热组件结构体
static DRV_HEAT_TYPE* _gp_HEAT[HEAT_BUTT] = {NULL};
//定义了PID参数自整定结构体
static DRV_PIDTUNINGPARA_TYPE _g_PIDTUNNING[PID_BUTT] = {0};

/*实际温度值*/
float CurTemp[PID_BUTT] = {0};

//定义了PID设定温度 扩大100倍
static float GoalTemp[PID_BUTT] = {0};

/*软件定时器1回调函数*/
void SofeTimer1CallBack(void *ptmr,void *p_arg);

void Func_PidTuningInit(void)
{
	uint8_t i = 0;
	_g_PIDTUNNING[0].pid	= Drv_PidInit(PID_1,GoalTemp[0]/100.0,KP_INIT0,KI_INIT0,KD_INIT0);
	_g_PIDTUNNING[1].pid	= Drv_PidInit(PID_2,GoalTemp[1]/100.0,KP_INIT0,KI_INIT0,KD_INIT0);
	_g_PIDTUNNING[2].pid  = Drv_PidInit(PID_3,GoalTemp[2]/100.0,KP_INIT0,KI_INIT0,KD_INIT0);
	_g_PIDTUNNING[3].pid	= Drv_PidInit(PID_4,GoalTemp[3]/100.0,KP_INIT0,KI_INIT0,KD_INIT0);
	_g_PIDTUNNING[4].pid	= Drv_PidInit(PID_5,GoalTemp[4]/100.0,KP_INIT0,KI_INIT0,KD_INIT0);
	_g_PIDTUNNING[5].pid	= Drv_PidInit(PID_6,GoalTemp[5]/100.0,KP_INIT0,KI_INIT0,KD_INIT0);
	_g_PIDTUNNING[6].pid	= Drv_PidInit(PID_7,GoalTemp[6]/100.0,KP_INIT0,KI_INIT0,KD_INIT0);
	_g_PIDTUNNING[7].pid	= Drv_PidInit(PID_8,GoalTemp[7]/100.0,KP_INIT0,KI_INIT0,KD_INIT0);
	
	for(i = 0; i<PID_BUTT; i++)
	{
		Drv_PidTuningParaInit(_g_PIDTUNNING + i, _g_PIDTUNNING[i].pid, _gp_HEAT[i], Drv_HeatOpen, Drv_HeatClose); /*自整定初始化*/
	}
	
}


/*自整定线程*/
void Task1(void *p_arg)
{
	uint8_t i = 0;
	uint8_t err;
	while(1)
	{
		if(SwitchTemp)
		{
			for(i = 0; i < PID_BUTT; i++)
			{
				if(Drv_PidTuning(_g_PIDTUNNING + i, CurTemp[i]) != END)
				{
					SofeTimer1 = OSTmrCreate(10,1,OS_TMR_OPT_PERIODIC,SofeTimer1CallBack, (void *)0, (INT8U *)"timer1",&err);
				}
			}
			OSTimeDlyHMSM(0, 0, 1, 0);
		}
		else
		{
			OSTimeDlyHMSM(0, 0, 1, 0);
		}
	}
}

/*PID计算线程*/
void Task2(void *p_arg)
{
	uint8_t i = 0;
	while(1)
	{
		if(SwitchTemp)
		{
			for(i = 0; i < PID_BUTT; i++)
			{
				if(_g_PIDTUNNING[i].pid->status == PID_WORKING && _g_PIDTUNNING[i].constatus == PID_CAL) /*PID计算*/
				{
					_g_PIDTUNNING[i].duty = Drv_PidCalculate(_g_PIDTUNNING[i].pid,CurTemp[i]);
					_g_PIDTUNNING[i].constatus = PID_CON;
				}
			}
			OSTimeDlyHMSM(0, 0, 0, 200);
		}
		else
		{
			OSTimeDlyHMSM(0, 0, 1, 0);
		}
				
	}
}


/*软件定时器1回调函数*/
void SofeTimer1CallBack(void *ptmr,void *p_arg)
{
	uint8_t i = 0;
	for(i = 0; i < PID_BUTT; i++)
	{
		if(_g_PIDTUNNING[i].constatus == PID_CON)
		{
			if(_g_PIDTUNNING[i].duty > _g_PIDTUNNING[i].pertim)
				_g_PIDTUNNING[i].positivefun(_g_PIDTUNNING[i].modular);
			else
				_g_PIDTUNNING[i].negativefun(_g_PIDTUNNING[i].modular);
			_g_PIDTUNNING[i].pertim -= 10;
			if(_g_PIDTUNNING[i].pertim <= 0)
			{
				_g_PIDTUNNING[i].pertim = FULLDUTY;
				_g_PIDTUNNING[i].constatus = PID_CAL;
			}
		}
	}
}
