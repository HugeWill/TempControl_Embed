/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——Main函数

  -------------------------------------------------------
   文 件 名   : main.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年08月01日
   功能描述   : 定义main函数的一般形式。
   依赖于     : uC/OS-II V2.92.07
				BSP V1.0.0.0
				Task V1.0.0.0
   注         ：该文件不依赖于硬件电路。 
 ********************************************************/

#include "stdlib.h"
#include "ucos_ii.h"
#include "BSP_Sys.h"
#include "Task.h"

/* Main函数
  -----------------------------------
*/
int main(void)
{
    OSInit();	/*初始化OS*/
	
	BSP_InitSysTick(72, OS_TICKS_PER_SEC);	/*设置系统滴答时钟*/
	
	OSTaskCreate(TaskStart, NULL, (OS_STK *)&g_TaskStart_Stk[TASK_START_STK_SIZE - 1], TASK_START_PRIO);	/*创建初始线程*/
				
    OSStart();	/*启动OS*/
	
	return 0;
}
