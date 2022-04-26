/********************************************************

   文 件 名   : Drv_UartDataInterface.h
   版 本 号   : V1.0.0.0
   作    者   : hyc
   生成日期   : 2022年4月12日
   功能描述   : 
   使用方法   ：

   
   依赖于     : BSP.h
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/
 
#ifndef _DRV_UARTDATAINTERFACE_H_
#define _DRV_UARTDATAINTERFACE_H_

#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "BSP_Uart.h"

#define USART_RX_MAX_NUMBER	100
#define USART_TX_MAX_NUMBER	100
#define MAX_CMD_BUFFER (5)
extern uint8_t Usart_Rx_Buffer[MAX_CMD_BUFFER][USART_RX_MAX_NUMBER];		//串口接收数据缓存，最多缓存五条指令
extern uint8_t Usart_Tx_Buffer[MAX_CMD_BUFFER][USART_RX_MAX_NUMBER];		//串口发送缓存，最多缓存五百条指令

/*通讯状态字定义*/
typedef enum{
	CMD_WORD = 0x00,				//上位机下发状态字
	RETURN_DONE_WORD,				//下位机回复成功状态字
	RETURN_FAIL_WORD,				//下位机回复失败状态字
	REPORT_ERROR_WORD,				//下位机主动上报错误状态字
	REPORT_EVENT_WORD,				//下位机主动上报事件状态字
} UART_STATUS_WORD;

//下发包结构体
typedef struct 
{
    uint32_t frame_head;			//固定帧头或使用流水号帧id
    uint8_t status;
    uint8_t length;
	uint16_t cmd;
    uint8_t data[];					//数据域4字节对齐
} COMMON_CMD_DATA;
//上报包结构体
typedef struct 
{
    uint32_t frame_head;
    uint8_t status;
    uint8_t length;
	uint16_t cmd;
    uint8_t data[];					//数据域4字节对齐
} COMMON_REP_DATA;


// typedef struct 
// {
// 	uint16_t frame_head;
// 	uint8_t status;
// 	uint8_t length;
// 	uint16_t cmd;
// 	uint8_t rsv[];					
// } RETURN_DONE_TYPE;
// typedef struct 
// {
// 	uint16_t frame_head;
// 	uint8_t status;
// 	uint8_t length;
// 	uint16_t cmd;
// 	uint16_t error_code;
// 	uint16_t module_id;
// 	uint16_t device_id;
// } RETURN_FAIL_TYPE;
// typedef struct 
// {
// 	uint16_t frame_head;
// 	uint8_t status;
// 	uint8_t length;
// 	uint16_t cmd;						//占位
// 	uint16_t error_code;
// 	uint16_t module_id;
// 	uint16_t device_id;
// } REPORT_ERROR_TYPE;
// typedef struct 
// {
// 	uint16_t frame_head;
// 	uint8_t status;
// 	uint8_t length;
// 	uint16_t cmd;						//占位
// 	uint8_t data[];						//占位
// } REPORT_EVENT_TYPE;

/*串口数据接口模块枚举*/
typedef enum{
	UARTDATAINTERFACE_1 = 0,
	UARTDATAINTERFACE_2,
	UARTDATAINTERFACE_BUTT
} DRV_UARTDATAINTERFACE_ENUM;

/*串口数据接口模块状态枚举*/
typedef enum{
	UARTDATAINTERFACE_UNDEFINED = 0,		/*未初始化*/
	UARTDATAINTERFACE_READY,				/*准备好*/
	UARTDATAINTERFACE_BUSY,					/*繁忙*/
} DRV_UARTDATAINTERFACE_STATUS_ENUM;

/*串口发送缓存状态*/
typedef enum{
	USART_TX_IDLE = 0,
	USART_TX_SEND,
} DRV_UARTDATAOUT_STATUS_ENUM;
/*串口数据接口模块结构体*/
typedef struct{
	/*------------------------------变量*/
	DRV_UARTDATAINTERFACE_STATUS_ENUM status;	/*该模块的状态*/
	uint8_t rx_buffer[USART_RX_MAX_NUMBER];		/*接收缓存*/
	uint16_t rx_push;							/*接收下标*/
	DRV_UARTDATAOUT_STATUS_ENUM tx_status;
	/*------------------------------变常*/
	
	/*------------------------------常量*/
	DRV_UARTDATAINTERFACE_ENUM id;				/*模块编号*/
	BSP_USART_ENUM usart_number;				/*串口模块编号*/
	uint32_t baud_rate;							/*波特率*/
	BSP_PORT_ENUM tx_port_number; 				/*TX引脚端口号*/
	BSP_PIN_ENUM  tx_pin_number;				/*TX引脚编号*/
	BSP_PORT_ENUM rx_port_number; 				/*RX引脚端口号*/
	BSP_PIN_ENUM  rx_pin_number;				/*RX引脚编号*/
	OS_EVENT* one_packet_sem;				   	/*数据包接收完成信号量*/
} DRV_UARTDATAINTERFACE_TYPE;


extern DRV_UARTDATAINTERFACE_TYPE* _gp_usart1;
/* 接口函数
 ********************************************************/

/* 初始化串口数据接口模块
  -----------------------------
  入口：串口数据接口模块的ID号，波特率，TX、RX端口
  返回值：初始化好的串口数据接口模块结构体
*/
extern DRV_UARTDATAINTERFACE_TYPE* Drv_UartDataInterfaceInit(DRV_UARTDATAINTERFACE_ENUM id, \
										BSP_USART_ENUM usart_number, uint32_t baud_rate, \
										BSP_PORT_ENUM tx_port_number, BSP_PIN_ENUM tx_pin_number, \
										BSP_PORT_ENUM rx_port_number, BSP_PIN_ENUM rx_pin_number);

/* 串口接收使能
  ------------------------------------
*/
extern void Drv_UsartGetEnable(DRV_UARTDATAINTERFACE_TYPE* p_unit);

/* 串口接收禁用
  ------------------------------------
*/
extern void Drv_UsartGetDisable(DRV_UARTDATAINTERFACE_TYPE* p_unit);

/* 串口打包发送数据
  ------------------------------------
*/
extern void Drv_UsartSendPacket(DRV_UARTDATAINTERFACE_TYPE* p_unit, uint32_t data1, uint32_t data2, uint32_t data3);

/* 串口打包发送命令
  ------------------------------------
*/
extern void Drv_UsartSendCmd(DRV_UARTDATAINTERFACE_TYPE* p_unit, uint8_t cmd, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5);

/* 串口接收中断处理
  ------------------------------------
*/
extern void _Drv_Usart1ReceiveHandler(void);
/* 串口接收中断处理
  ------------------------------------
*/
extern void _Drv_Usart2ReceiveHandler(void);
/* 指令缓冲队列前移
  ------------------------------------
*/
extern void _Drv_UsartCmdFifoForward(void);

/* 发送缓冲队列前移
  ------------------------------------
*/
extern void _Drv_UartTxFifoForward(void);

/* 发送缓存区数据发送
  ------------------------------------
*/
extern void Drv_UartSendDatas(uint8_t* p_buffer);

/* 串口回复成功到发送缓存
  ------------------------------------
*/
extern void _Drv_UsartReturnDoneToBuffer(uint32_t frame_head, uint16_t cmd, uint8_t length, uint8_t* datas);

/* 串口回复失败到发送缓存
  ------------------------------------
*/
extern void _Drv_UsartReturnFailToBuffer(uint32_t frame_head, uint16_t cmd, uint8_t length, uint8_t* datas);

/* 串口上报错误到发送缓存
  ------------------------------------
*/
extern void _Drv_UsartReportErrToBuffer(uint32_t frame_head,uint16_t cmd, uint8_t length, uint8_t* datas);

/* 串口上报错误到发送缓存
  ------------------------------------
*/
extern void _Drv_UsartReportEventToBuffer(uint32_t frame_head, uint16_t cmd, uint8_t length, uint8_t* datas);

/* 串口回复失败到发送缓存
  ------------------------------------
  入口：	err_code 错误编码

  返回值：无
*/
extern void _Drv_UsartReturnOnlyErrToBuffer(uint32_t frame_head, uint16_t cmd, uint16_t err_code);


#endif
