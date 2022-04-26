/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——PID模块

  -------------------------------------------------------
   文 件 名   : Drv_Pid.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2020年2月14日
   功能描述   : 定义一种PID模块。
   使用方法   ：
   依赖于     : 
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/
 
#include "Drv_Pid.h"

DRV_PID_TYPE* _gp_PidUnits[PID_BUTT] = {0};	/*PID模块的注册表*/

/* 接口函数
 ********************************************************/

/* 初始化PID模块
  -----------------------------
  入口：PID模块的ID号，目标值，P、I、D参数
  返回值：初始化好的PID模块结构体
*/
DRV_PID_TYPE* Drv_PidInit(DRV_PID_ENUM id, float setpoint, float kp, float ki, float kd)
{	
	DRV_PID_TYPE* p_unit = (DRV_PID_TYPE*)calloc(1, sizeof(DRV_PID_TYPE));	/*申请内存*/

	/*分配失败返回NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id错误返回NULL*/
	if(id >= PID_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*如果当前模块存在，取消分配新存储区*/
	if(_gp_PidUnits[id] != NULL)
	{
		free(p_unit);
		p_unit = _gp_PidUnits[id];
	}
	else
	{
		_gp_PidUnits[id] = p_unit;
	}
	
	/*初始化*/
	
	/*-------------------------------常量*/
	p_unit->id			= id;				/*模块id*/
	p_unit->setpoint	= setpoint;
	p_unit->kp 			= kp;
	p_unit->ki 			= ki;
	p_unit->kd 			= kd;
	
	/*-------------------------------变量*/
	p_unit->status		= PID_WORKING;
	p_unit->lasterr		= 0;
	p_unit->preverr		= 0;
	p_unit->sumerr		= 0;
	
	return p_unit;
}

/* 初始化PID自整定模块
  -----------------------------
 */
void Drv_PidTuningParaInit(DRV_PIDTUNINGPARA_TYPE* pid_tuning_para, DRV_PID_TYPE* pid, void *modular, void *positivefun, void *negativefun)
{
	pid_tuning_para->pid = pid;
	pid_tuning_para->pid->status = PID_TUNING; /*自整定*/
	pid_tuning_para->modular = modular;
	pid_tuning_para->positivefun = (PidFunc *)positivefun;
	pid_tuning_para->negativefun = (PidFunc *)negativefun;
    pid_tuning_para->period = DEFAULTPERIOD;
    pid_tuning_para->status = PRE_PROCESS;
	pid_tuning_para->constatus = PID_CAL;
    pid_tuning_para->tuning_max = 0;   	/*不能修改，用于第一次比较判断*/
    pid_tuning_para->tuning_min = 100;   /*不能修改，用于第一次比较判断*/
	pid_tuning_para->jitter = 0;
	pid_tuning_para->duty = 0;
	pid_tuning_para->pertim = FULLDUTY;
}

/* PID模块输出计算
  -----------------------------
  入口：PID模块指针，当前值
  返回值：输出值
*/
float Drv_PidCalculate(DRV_PID_TYPE* p_unit, float current)
{
    float kp = 0.0;
    float ki = 0.0;
    float kd = 0.0;
    float sum = 0.0;
    float sumerr = 0.0;
    int index = 0;
    
    p_unit->preverr = p_unit->lasterr;				/*记录前一轮误差*/
    p_unit->lasterr = p_unit->setpoint - current;	/*计算当前误差*/
    p_unit->sumerr += p_unit->lasterr;				/*计算误差累积*/
        	
    if(fabs((double)(p_unit->lasterr)) > ARRAY_ERR) 			/*积分限幅清零*/
    {
        p_unit->sumerr = 0;
    }
    
    if(fabs((double)(p_unit->ki)) > FLT_EPSILON)				/*积分舍弃偏差*/
    {
        sumerr = (2 * ARRAY_DUTY / p_unit->ki);
    }
    else
    {
        sumerr = 0.0;
    }

    if(p_unit->sumerr > sumerr)
    {
        p_unit->sumerr = sumerr;
    }
    else if(p_unit->sumerr < -sumerr)
    {
        p_unit->sumerr = -sumerr;
    }
    
    kp = p_unit->kp * p_unit->lasterr;
    ki = p_unit->ki * p_unit->sumerr;
    kd = p_unit->kd * (p_unit->lasterr - p_unit->preverr);

    sum = kp + ki + kd;
    if (sum <= 0)
    {
        return 0;
    }
    else if (sum >= FULLDUTY)
    {
        return FULLDUTY;
    }
    else
    {
        return sum;
    }
}

/* PID模块复位
  -----------------------------
  入口：PID模块指针
*/
void Drv_PidReset(DRV_PID_TYPE* p_unit)
{
    p_unit->preverr = 0;
    p_unit->lasterr = 0;
    p_unit->sumerr = 0;
}

/* PID参数自整定
  -----------------------------
  返回值：自整定状态
*/
TUNING_STAT Drv_PidTuning(DRV_PIDTUNINGPARA_TYPE* pid_tuning_para, float current)
{
	float setpoint = pid_tuning_para->pid->setpoint;
	
    if ((pid_tuning_para->status) != END)
    {		
        switch(pid_tuning_para->status)
        {
            case PRE_PROCESS:   /*预加热*/
            {
                if(setpoint > current)	/*当前温度小于目标温度，先消抖，再加热*/
                {
					if((pid_tuning_para->jitter) < 0)
					{
						(pid_tuning_para->jitter) = 0;
					}
					(pid_tuning_para->jitter)++;
					if((pid_tuning_para->jitter) >= 3)
					{
						(pid_tuning_para->jitter) = 0;
//						Drv_HeatOpen(pid_tuning_para->heat);
						pid_tuning_para->positivefun();
					}
                }
				else	/*当前温度大于等于目标温度，先消抖，再停止加热*/
				{
					if((pid_tuning_para->jitter) > 0)
					{
						(pid_tuning_para->jitter) = 0;
					}
					(pid_tuning_para->jitter)--;
					if((pid_tuning_para->jitter) <= -3)
					{
						(pid_tuning_para->jitter) = 0;
						pid_tuning_para->status = HIGH1;
//						Drv_HeatClose(pid_tuning_para->heat);
						pid_tuning_para->negativefun();
					}
				}
//                OSTimeDly(pid_tuning_para->period*1000);
                break;
            }
			
            case HIGH1: /*第一个震荡周期舍弃*/
            {
                if(setpoint > current)	/*当前温度小于目标温度，先消抖，再加热*/
                {
					if((pid_tuning_para->jitter) < 0)
					{
						(pid_tuning_para->jitter) = 0;
					}
					(pid_tuning_para->jitter)++;
					if((pid_tuning_para->jitter) >= 3)
					{
						(pid_tuning_para->jitter) = 0;
						pid_tuning_para->status = LOW1;
//						Drv_HeatOpen(pid_tuning_para->heat);
						pid_tuning_para->positivefun();
					}
                }
				else	/*当前温度大于等于目标温度，先消抖，再停止加热*/
				{
					if((pid_tuning_para->jitter) > 0)
					{
						(pid_tuning_para->jitter) = 0;
					}
					(pid_tuning_para->jitter)--;
					if((pid_tuning_para->jitter) <= -3)
					{
						(pid_tuning_para->jitter) = 0;
//						Drv_HeatClose(pid_tuning_para->heat);
						pid_tuning_para->negativefun();
					}
				}
//                OSTimeDly(pid_tuning_para->period*1000);
                break;
            }
			
            case LOW1:
            {
                if(setpoint > current)	/*当前温度小于目标温度，先消抖，再加热*/
                {
					if((pid_tuning_para->jitter) < 0)
					{
						(pid_tuning_para->jitter) = 0;
					}
					(pid_tuning_para->jitter)++;
					if((pid_tuning_para->jitter) >= 3)
					{
						(pid_tuning_para->jitter) = 0;
//						Drv_HeatOpen(pid_tuning_para->heat);
						pid_tuning_para->positivefun();
					}
                }
				else	/*当前温度大于等于目标温度，先消抖，再停止加热*/
				{
					if((pid_tuning_para->jitter) > 0)
					{
						(pid_tuning_para->jitter) = 0;
					}
					(pid_tuning_para->jitter)--;
					if((pid_tuning_para->jitter) <= -3)
					{
						(pid_tuning_para->jitter) = 0;
						pid_tuning_para->status = HIGH2;
//						Drv_HeatClose(pid_tuning_para->heat);
						pid_tuning_para->negativefun();
					}
				}
//                OSTimeDly(pid_tuning_para->period*1000);
                break;
            }
			
            case HIGH2:
            {
                if(setpoint > current)	/*当前温度小于目标温度，先消抖，再加热*/
                {
					if((pid_tuning_para->jitter) < 0)
					{
						(pid_tuning_para->jitter) = 0;
					}
					(pid_tuning_para->jitter)++;
					if((pid_tuning_para->jitter) >= 3)
					{
						(pid_tuning_para->jitter) = 0;
						pid_tuning_para->status = LOW2;
//						Drv_HeatOpen(pid_tuning_para->heat);
						pid_tuning_para->positivefun();
					}
                }
				else	/*当前温度大于等于目标温度，先消抖，再停止加热*/
				{
					if((pid_tuning_para->jitter) > 0)
					{
						(pid_tuning_para->jitter) = 0;
					}
					(pid_tuning_para->jitter)--;
					if((pid_tuning_para->jitter) <= -3)
					{
						(pid_tuning_para->jitter) = 0;
//						Drv_HeatClose(pid_tuning_para->heat);
						pid_tuning_para->negativefun();
					}
				}
//                OSTimeDly(pid_tuning_para->period*1000);
                (pid_tuning_para->tuning_period)++;  		/*记录周期个数*/
                if((pid_tuning_para->tuning_max) < current)
                {
                    (pid_tuning_para->tuning_max) = current; /*取该阶段最高点*/
                }	
                break;
            }
			
            case LOW2:
            {
                if(setpoint > current)	/*当前温度小于目标温度，先消抖，再加热*/
                {
					if((pid_tuning_para->jitter) < 0)
					{
						(pid_tuning_para->jitter) = 0;
					}
					(pid_tuning_para->jitter)++;
					if((pid_tuning_para->jitter) >= 3)
					{
						(pid_tuning_para->jitter) = 0;
//						Drv_HeatOpen(pid_tuning_para->heat);
						pid_tuning_para->positivefun();
					}
                }
				else	/*当前温度大于等于目标温度，先消抖，再停止加热*/
				{
					if((pid_tuning_para->jitter) > 0)
					{
						(pid_tuning_para->jitter) = 0;
					}
					(pid_tuning_para->jitter)--;
					if((pid_tuning_para->jitter) <= -3)
					{
						(pid_tuning_para->jitter) = 0;
						pid_tuning_para->status = END;
						pid_tuning_para->pid->status = PID_WORKING;
						pid_tuning_para->tuning_period = (pid_tuning_para->period)*(pid_tuning_para->tuning_period);   /*单个周期时间x周期个数*/
						_Drv_PidPara(pid_tuning_para, pid_tuning_para->tuning_period, ((pid_tuning_para->tuning_max)-(pid_tuning_para->tuning_min))/2);  /*根据震荡周期特征量整定pid参数*/
//						Drv_HeatClose(pid_tuning_para->heat);
						pid_tuning_para->negativefun();
					}
				}
//                OSTimeDly(pid_tuning_para->period*1000);
                (pid_tuning_para->tuning_period)++;  /*记录周期个数*/
                if ((pid_tuning_para->tuning_min) > current)
                {
                    (pid_tuning_para->tuning_min) = current; /*取该阶段最低点*/
                }
                break;
            }
			
            default:
            {
                break;
            }
        }
    }
	
    return pid_tuning_para->status;
}


/* 消除整定过程状态迁移的抖动
  -----------------------------
*/
static bool _Drv_Avoidjitter(DRV_PIDTUNINGPARA_TYPE *pid_tuning_para)
{
	(pid_tuning_para->jitter)++;
	if ((pid_tuning_para->jitter) == 3)
	{
		(pid_tuning_para->jitter) = 0;
		return true;
	}
	return false;
}


/* 通过整定数据，计算PID参数
  -----------------------------
*/
static void _Drv_PidPara(DRV_PIDTUNINGPARA_TYPE* pid_tuning_para, unsigned int tu, float k)
{
    float Ti = 0.0;
    float Td = 0.0;
    float ku = 0.0;
    
//    if(fabs(k) > FLT_EPSILON)
//    {
//        ku = 4 * 100 / (PI * k);
//    }
		ku = 4 * 100 / (PI * k);
    pid_tuning_para->pid->kp = 0.6 * ku;

    
    Ti = tu * 0.5;
    
    if(fabs(Ti) > FLT_EPSILON)
    {
        pid_tuning_para->pid->ki = pid_tuning_para->pid->kp / Ti;
    }
    else
    {
        pid_tuning_para->pid->ki = 0;
    }
    
    Td = tu * 0.125;
    pid_tuning_para->pid->kd = pid_tuning_para->pid->kp * Td;
}
