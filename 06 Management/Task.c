/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——任务定义

  -------------------------------------------------------
   文 件 名   : Tsk.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年08月01日
   功能描述   : 定义并行任务。
   依赖于     : uC/OS-II V2.92.07
   注         ：该文件不依赖于硬件电路。 
 ********************************************************/
 
#include "Task.h"

/*----------------------------------------------------定义任务堆栈*/
OS_STK g_TaskStart_Stk[TASK_START_STK_SIZE];							/*初始任务 堆栈*/

static OS_STK g_TaskCmdTransceiver_Stk[TASK_CMD_TRANSCEIVER_STK_SIZE];	/*指令收发器任务 堆栈*/
static OS_STK g_TaskCmdScheduler1_Stk[TASK_CMD_SCHEDULER1_STK_SIZE];	/*指令调度器1任务 堆栈*/
static OS_STK g_TaskCmdScheduler2_Stk[TASK_CMD_SCHEDULER2_STK_SIZE];	/*指令调度器2任务 堆栈*/

static OS_STK g_TaskLed_Stk[TASK_LED_STK_SIZE];							/*系统指示灯任务 堆栈*/
static OS_STK g_TaskResident1_Stk[TASK_RESIDENT_1_STK_SIZE];			/*私有常驻任务1 堆栈*/
static OS_STK g_TaskResident2_Stk[TASK_RESIDENT_2_STK_SIZE];			/*私有常驻任务2 堆栈*/
static OS_STK g_Task1_Stk[TASK1_STK_SIZE];
static OS_STK g_Task2_Stk[TASK2_STK_SIZE];
static OS_STK g_Task3_Stk[TASK3_STK_SIZE];
static OS_STK g_Task4_Stk[TASK4_STK_SIZE];
static OS_STK g_Task5_Stk[TASK5_STK_SIZE];
static OS_STK g_Task6_Stk[TASK6_STK_SIZE];
/*----------------------------------------------------定义全局变量*/


/*----------------------------------------------------定义任务*/

/* 初始任务
  -----------------------------------ok
*/
void TaskStart(void* p_arg)
{
#if OS_CRITICAL_METHOD == 3			/* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    OS_ENTER_CRITICAL();

	/*创建协议层任务*/
    OSTaskCreate(TaskCmdTransceiver, NULL, \
				(OS_STK*)&g_TaskCmdTransceiver_Stk[TASK_CMD_TRANSCEIVER_STK_SIZE - 1], \
				TASK_CMD_TRANSCEIVER_PRIO);	/*指令收发器*/
				
	OSTaskCreate(TaskCmdScheduler1,  NULL, \
				(OS_STK*)&g_TaskCmdScheduler1_Stk[TASK_CMD_SCHEDULER1_STK_SIZE - 1], \
				TASK_CMD_SCHEDULER1_PRIO);	/*指令调度器1*/
		OSTaskCreate(TaskCmdScheduler2,  NULL, \
				(OS_STK*)&g_TaskCmdScheduler2_Stk[TASK_CMD_SCHEDULER2_STK_SIZE - 1], \
				TASK_CMD_SCHEDULER2_PRIO);	/*指令调度器2*/
				
	/*创建功能层任务*/
    OSTaskCreate(TaskLed,            NULL, \
				(OS_STK*)&g_TaskLed_Stk[TASK_LED_STK_SIZE - 1], \
				TASK_LED_PRIO);				/*系统指示灯*/
    OSTaskCreate(TaskResident1,      NULL, \
				(OS_STK*)&g_TaskResident1_Stk[TASK_RESIDENT_1_STK_SIZE - 1], \
				TASK_RESIDENT_1_PRIO);		/*私有常驻任务1*/
    OSTaskCreate(TaskResident2,      NULL, \
				(OS_STK*)&g_TaskResident2_Stk[TASK_RESIDENT_2_STK_SIZE - 1], \
				TASK_RESIDENT_2_PRIO);		/*私有常驻任务2*/
//		OSTaskCreate(Task1,      NULL, \
//				(OS_STK*)&g_Task1_Stk[TASK1_STK_SIZE - 1], \
//				TASK1_PRIO);		/*私有常驻任务2*/
//		OSTaskCreate(Task2,      NULL, \
//				(OS_STK*)&g_Task2_Stk[TASK2_STK_SIZE - 1], \
//				TASK2_PRIO);		/*私有常驻任务2*/
//		OSTaskCreate(Task3,      NULL, \
//				(OS_STK*)&g_Task3_Stk[TASK3_STK_SIZE - 1], \
//				TASK3_PRIO);		/*私有常驻任务2*/
//		OSTaskCreate(Task4,      NULL, \
//				(OS_STK*)&g_Task4_Stk[TASK4_STK_SIZE - 1], \
//				TASK4_PRIO);		/*私有常驻任务2*/
//		OSTaskCreate(Task5,      NULL, \
//				(OS_STK*)&g_Task5_Stk[TASK5_STK_SIZE - 1], \
//				TASK5_PRIO);		/*私有常驻任务2*/
//		OSTaskCreate(Task6,      NULL, \
//				(OS_STK*)&g_Task6_Stk[TASK6_STK_SIZE - 1], \
//				TASK6_PRIO);		/*私有常驻任务2*/
	
    OSTaskSuspend(TASK_START_PRIO);	/* suspend but not delete */
    OS_EXIT_CRITICAL();
}

/* 指令收发器任务
  -----------------------------------ok
*/
static void TaskCmdTransceiver(void* p_arg)
{	
	OS_CPU_SR  cpu_sr;
	_gp_usart1 = Drv_UartDataInterfaceInit(UARTDATAINTERFACE_1,_USART1_,115200,_PA_,_P9_,_PA_,_P10_);
	while (1)
	{
		OSTimeDlyHMSM(0,0,0,5);
		OS_ENTER_CRITICAL();
		if(USART_TX_IDLE != _gp_usart1->tx_status)
		{
			Drv_UartSendDatas(&Usart_Tx_Buffer[0][0]);
			_Drv_UartTxFifoForward();
		}
		OS_EXIT_CRITICAL();
	}
	
}

/* 指令调度器1任务
  -----------------------------------
*/
static void TaskCmdScheduler1(void* p_arg)
{
	uint8_t err = OS_ERR_NONE;
	COMMON_CMD_DATA* p_msg = NULL;
	

	CMD_REGEDIT_TYPE* p_cmd_regedit = Cmd_Regedit_Init(CMD_REGEDIT_1,MODULE_1);
	OSTaskCreate(Task1,      NULL, \
			(OS_STK*)&g_Task1_Stk[TASK1_STK_SIZE - 1], \
			TASK1_PRIO);		/*私有常驻任务2*/
	OSTaskCreate(Task2,      NULL, \
			(OS_STK*)&g_Task2_Stk[TASK2_STK_SIZE - 1], \
			TASK2_PRIO);		/*私有常驻任务2*/
	OSTaskCreate(Task3,      NULL, \
			(OS_STK*)&g_Task3_Stk[TASK3_STK_SIZE - 1], \
			TASK3_PRIO);		/*私有常驻任务2*/
	OSTaskCreate(Task4,      NULL, \
			(OS_STK*)&g_Task4_Stk[TASK4_STK_SIZE - 1], \
			TASK4_PRIO);		/*私有常驻任务2*/
	OSTaskCreate(Task5,      NULL, \
			(OS_STK*)&g_Task5_Stk[TASK5_STK_SIZE - 1], \
			TASK5_PRIO);		/*私有常驻任务2*/
	OSTaskCreate(Task6,      NULL, \
			(OS_STK*)&g_Task6_Stk[TASK6_STK_SIZE - 1], \
			TASK6_PRIO);		/*私有常驻任务2*/
	while (1)
	{
		if(p_cmd_regedit == NULL)
		{
			OSTimeDlyHMSM(0,0,0,500);
		}
		else
		{
			OSSemPend(_gp_usart1->one_packet_sem,0,&err);
			p_msg = (COMMON_CMD_DATA*)(&Usart_Rx_Buffer[0][0]);			//从缓存区取出第一条指令
			if(!Prt_CmdScheduler(p_msg,p_cmd_regedit))						//执行指令
				_Drv_UsartReturnOnlyErrToBuffer(0,p_msg->cmd,ERR_CMD_NULL); /*未识别指令或通讯出错*/
			memset(&Usart_Rx_Buffer[0][0],0,USART_RX_MAX_NUMBER);		//清空第一条指令
			_Drv_UsartCmdFifoForward();									//指令缓冲前移
		}
	}	
}


/* 系统指示灯任务
  -----------------------------------ok
*/
static void TaskLed(void* p_arg)
{
	DRV_LED_TYPE* p_led = Led_Init(LED_1, LED_POSITIVE_LOGIC, CONFIG_LED_PORT, CONFIG_LED_PIN);
	
	if(p_led == NULL)
	{
		while(1)
		{
			OSTimeDlyHMSM(0, 0, 0, 500);	//出错需要处理
		}
	}
	else
	{
		while(1)
		{
			Led_On(p_led);
			OSTimeDlyHMSM(0, 0, 0, 500);
			Led_Off(p_led);
			OSTimeDlyHMSM(0, 0, 0, 500);
		}
	}
}
