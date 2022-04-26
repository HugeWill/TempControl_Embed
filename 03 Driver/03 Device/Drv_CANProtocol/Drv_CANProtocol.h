/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——CAN通信模块

  -------------------------------------------------------
   文 件 名   : Drv_CANProtocol.h
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年08月01日
   功能描述   : 定义一种CAN通信协议接口。
   使用方法   ：上层模块通过调用CAN_Init获得一个通信模块的
				指针。
   依赖于     : BSP V1.0.0.0
				uC/OS-II V2.92.07
   注         ：该模块不依赖于硬件电路。 
 ********************************************************/

#ifndef _DRV_CANPROTOCOL_H__
#define _DRV_CANPROTOCOL_H__

#include "string.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdarg.h"
#include "ucos_ii.h"
#include "BSP_Can.h"
#include "Func_common.h"

#define CAN_RX_CMD_MAX_NUMBER	5	/*每个ID，CAN接收最大缓存指令条数*/
#define CAN_TX_CMD_MAX_NUMBER	50	/*CAN发送最大缓存指令条数*/
#define CAN_RX_CMD_MAX_SIZE		40	/*CAN接收最大缓存指令帧数，即接收的长帧最大多少帧*/
#define CAN_TX_CMD_MAX_SIZE		10	/*CAN发送最大缓存指令帧数，即发送的长帧最大多少帧*/

#define MUTEX_PRIO				5	/*互斥信号量任务提升优先级*/

#define SEND_MAX_NUMBER			54	/*一个数据包最多发送字节数*/

/*CAN数据结束状态枚举*/
typedef enum{
	MARK_NO_END = 0,		/*未结束*/
	MARK_END = 1,			/*结束*/
} DRV_CAN_END_MARK;

/*CAN通信状态字枚举*/
typedef enum{
	CAN_STATUS_WORD_DONE     = 0,	/*执行成功*/
	CAN_STATUS_WORD_FAILED   = 1,	/*执行失败*/
	CAN_STATUS_WORD_RESPONSE = 2,	/*接收应答*/
	CAN_STATUS_WORD_ERROR    = 3,	/*接收出错*/
	CAN_STATUS_WORD_ACK      = 4,	/*接收完成*/
} DRV_CAN_STATUS_WORD;

/*CAN通信类型字枚举*/
typedef enum{
	CAN_TYPE_WORD_ASK        = 0,	/*请求*/
	CAN_TYPE_WORD_ACK        = 1,	/*应答*/
	CAN_TYPE_WORD_ERR        = 2,	/*出错*/
	CAN_TYPE_WORD_RET        = 3,	/*返回*/
} DRV_CAN_TYPE_WORD;

/*CAN模块接收缓存状态枚举*/
typedef enum{
	RX_BUFFER_NULL = 0,
	RX_BUFFER_WAIT,
	RX_BUFFER_READY,
} DRV_CAN_RX_BUFFER_STATUS_ENUM;

/*CAN模块发送缓存状态枚举*/
typedef enum{
	TX_BUFFER_NULL = 0,
	TX_BUFFER_BUSY,
} DRV_CAN_TX_BUFFER_STATUS_ENUM;

/*CAN模块状态枚举*/
typedef enum{
	CAN_IDLE = 0,	/*空闲*/
	CAN_BUSY,		/*繁忙*/
} DRV_CAN_STATUS_ENUM;

/*CAN接收缓冲区类型*/
typedef struct {
	DRV_CAN_RX_BUFFER_STATUS_ENUM status;	/*接收缓存状态*/
	uint8_t size;							/*接收到的帧数*/
	uint8_t can_id;							/*接收到的CAN ID*/
	uint16_t timer;							/*等待计时*/
	uint8_t buffer[CAN_RX_CMD_MAX_SIZE][8];	/*接收缓存区*/
} DRV_CAN_RX_BUFFER_TYPE;

/*CAN发送缓冲区类型*/
typedef struct {
	DRV_CAN_TX_BUFFER_STATUS_ENUM status;	/*发送缓存状态*/
	uint8_t size;							/*待发送的帧数*/
	uint8_t last_frame_byte;				/*最后一帧的字节数*/
	uint8_t can_id;							/*待发送的CAN ID*/
	uint8_t buffer[CAN_TX_CMD_MAX_SIZE][8];	/*发送缓存区*/
} DRV_CAN_TX_BUFFER_TYPE;

#pragma pack(1)	/*按1字节对齐*/

/*CAN回报类型*/
typedef struct{
	uint8_t serial_number;	/*流水号*/
	uint16_t cmd_status;		/*执行状态*/
	uint16_t res;		/*预留*/
	uint8_t can_id;		/*CAN ID*/
	uint8_t msg_type;	/*消息类型*/
	uint16_t len;		/*长度*/
	uint32_t frame_id;	/*报文ID*/
	uint16_t cmd;		/*CMD*/
	uint16_t status;	/*执行状态，0成功，1失败*/
} DRV_CAN_RETURN_TYPE;

/*CAN回报错误类型*/
typedef struct{
	uint8_t serial_number;	/*流水号*/
	uint16_t cmd_status;		/*执行状态*/
	uint16_t res;		/*预留*/
	uint8_t can_id;		/*CAN ID*/
	uint8_t msg_type;	/*消息类型*/
	uint16_t len;		/*长度*/
	uint32_t frame_id;	/*报文ID*/
	uint16_t cmd;		/*CMD*/
	uint16_t status;	/*执行状态，0成功，1失败*/
	uint8_t err1;		/*报错*/
	uint8_t err2;
	uint8_t err3;
} DRV_CAN_RETURN_ERR_TYPE;

/*CAN回报单个数据类型*/
typedef struct{
	uint8_t serial_number;	/*流水号*/
	uint16_t cmd_status;		/*执行状态*/
	uint16_t res;		/*预留*/
	uint8_t can_id;		/*CAN ID*/
	uint8_t msg_type;	/*消息类型*/
	uint16_t len;		/*长度*/
	uint32_t frame_id;	/*报文ID*/
	uint16_t cmd;		/*CMD*/
	uint16_t status;	/*执行状态，0成功，1失败*/
	uint32_t data;		/*数据*/
} DRV_CAN_RETURN_DATA_TYPE;

#pragma pack()

/*CAN模块结构体*/
typedef struct{
	/*------------------------------变量*/
	DRV_CAN_STATUS_ENUM status;									/*该CAN模块的状态*/	
	DRV_CAN_RX_BUFFER_TYPE rx_buffer[4][CAN_RX_CMD_MAX_NUMBER];	/*接收缓存*/	
	DRV_CAN_TX_BUFFER_TYPE tx_buffer[CAN_TX_CMD_MAX_NUMBER];	/*发送缓存*/
	uint32_t tx_buffer_pop_index;						/*发送缓存弹出下标*/
	volatile uint32_t tx_buffer_push_index;						/*发送缓存压入下标*/
	/*------------------------------变常*/
	
	/*------------------------------常量*/
	BSP_CAN_ENUM can_number;				/*CAN模块编号*/
	uint8_t can_id[4];						/*CAN ID*/
	uint32_t baud_rate;						/*波特率*/
	BSP_PORT_ENUM tx_port_number; 			/*TX引脚端口号*/
	BSP_PIN_ENUM  tx_pin_number;			/*TX引脚编号*/
	BSP_PORT_ENUM rx_port_number; 			/*RX引脚端口号*/
	BSP_PIN_ENUM  rx_pin_number;			/*RX引脚编号*/
	OS_EVENT* rx_sem[4];					/*接收完成信号量*/
	OS_EVENT* tx_sem;						/*发送完成信号量*/
	OS_EVENT* send_to_buffer_mutex;			/*写发送缓冲区互斥信号量*/
} DRV_CAN_TYPE;

/* 接口函数
 ********************************************************/

/* CAN通信模块初始化
  ---------------------------------ok
  最多可同时支持4个can id的接收，不使用的can_id写为0
  入口：CAN口编号，波特率，CAN ID，TX端口，TX引脚，RX端口，RX引脚
  返回值：初始化好的CAN通信模块指针
*/
extern DRV_CAN_TYPE* Drv_CANProtocol_Init(BSP_CAN_ENUM can_number, uint32_t baud_rate, \
								   uint8_t can_id1, uint8_t can_id2, uint8_t can_id3, uint8_t can_id4,\
								   BSP_PORT_ENUM tx_port_number, BSP_PIN_ENUM tx_pin_number, \
								   BSP_PORT_ENUM rx_port_number, BSP_PIN_ENUM rx_pin_number);

/* CAN发送一个数据包到发送缓存
  ---------------------------------ok
  写入缓冲区，等待发送
  入口：CAN ID，数据指针，数据长度
  返回值：true成功，false失败
*/
static bool _Drv_CANProtocol_SendToBuffer(uint8_t can_id, uint8_t* data, uint32_t len, DRV_CAN_END_MARK end_mark);

/* CAN发送一个应答信息到发送缓存
  ---------------------------------ok
  写入缓冲区，等待发送
  入口：CAN ID，流水号
  返回值：true成功，false失败
*/
extern bool Drv_CANProtocol_SendResponseToBuffer(uint8_t can_id, uint8_t serial_number);

/* CAN发送一个报错信息到发送缓存
  ---------------------------------ok
  写入缓冲区，等待发送
  入口：CAN ID，流水号，报错信息
  返回值：true成功，false失败
*/
extern bool Drv_CANProtocol_SendErrorToBuffer(uint8_t can_id, uint8_t serial_number, uint16_t cmd, uint32_t frame_id, uint8_t err1, uint8_t err2, uint8_t err3);

/* CAN发送一个完成信息到发送缓存
  ---------------------------------ok
  写入缓冲区，等待发送
  入口：CAN ID，流水号
  返回值：true成功，false失败
*/
extern bool Drv_CANProtocol_SendDoneToBuffer(uint8_t can_id, uint8_t serial_number, uint16_t cmd, uint32_t frame_id);

/* CAN发送一个接收完成信息到发送缓存
  --------------------------------- OK
  写入缓冲区，等待发送
  入口：CAN ID，流水号，命令字，firm id
  返回值：true成功，false失败
*/
extern bool Drv_CANProtocol_SendAckToBuffer(uint8_t can_id, uint8_t serial_number, uint16_t cmd, uint32_t frame_id);

/* CAN发送一个数据信息到发送缓存
  ---------------------------------ok
  写入缓冲区，等待发送
  入口：CAN ID，流水号，待发送的数据6个字节
  返回值：true成功，false失败
*/
extern bool Drv_CANProtocol_SendDataToBuffer(uint8_t can_id, uint8_t serial_number, uint16_t cmd, uint32_t frame_id, uint32_t data, uint8_t status);

/* CAN发送一串数据信息到发送缓存
  ---------------------------------
  写入缓冲区，等待发送
  入口：CAN ID，流水号，待发送的数据
  返回值：true成功，false失败
*/
extern bool Drv_CANProtocol_SendDatasToBuffer(uint8_t can_id, uint8_t serial_number, uint16_t cmd, uint32_t frame_id, uint8_t* datas, uint16_t len, uint8_t status);

/* CAN发送执行成功带数据到发送缓存
  ---------------------------------
  写入缓冲区，等待发送
  入口：CAN ID，流水号，待发送的数据
  返回值：true成功，false失败
*/
extern bool Drv_CANProtocol_SendDoneAndDatasToBuffer(uint8_t can_id, uint8_t serial_number, uint16_t cmd, uint32_t frame_id, uint8_t* datas, uint16_t len);

/* CAN释放指定接收缓冲区
  ---------------------------------ok
  入口：CAN ID编号，缓冲区地址
  返回值：true成功，false失败
*/
extern bool Drv_CANProtocol_RXBufferFree(uint8_t id_number, DRV_CAN_RX_BUFFER_TYPE* p_rx_data);

/* CAN释放指定发送缓冲区
  ---------------------------------ok
  入口：缓冲区地址
  返回值：true成功，false失败
*/
extern bool Drv_CANProtocol_TXBufferFree(DRV_CAN_TX_BUFFER_TYPE* p_tx_data);

/* CAN获取一个数据包缓存的指针
  ---------------------------------ok
  写入缓冲区，等待发送
  入口：CAN ID编号
  返回值：接收缓存地址
*/
extern DRV_CAN_RX_BUFFER_TYPE* Drv_CANProtocol_GetPacket(uint8_t id_number);

/* CAN发送一个数据帧
  ---------------------------------ok
  从发送缓冲区发送一个数据帧
  入口：
  返回值：true成功，false失败
*/
extern bool Drv_CANProtocol_PutFrame(void);

/* CAN接收缓冲区超时检测
  --------------------------------- OK
  接收缓冲区中长帧超时检测
  入口：
  返回值：true成功，false失败
*/
extern bool Drv_CANProtocol_RX_TimeOutCheck(uint8_t can_id, DRV_CAN_RX_BUFFER_TYPE* p_rx_buffer);

/* 内部函数
 ********************************************************/

/* CAN接收一个数据帧到接收缓存
  ---------------------------------ok
  写入缓冲区，等待发送
  入口：CAN ID，接收缓冲区，接收缓冲区状态，接收完成信号量，数据指针
  返回值：true成功，false失败
*/
static bool _Drv_CANProtocol_GetFrame(uint8_t can_id, \
									  DRV_CAN_RX_BUFFER_TYPE* p_rx_buffer, OS_EVENT* p_rx_sem, \
									  uint8_t* data);

/* 缓冲区格式化
  --------------------------------- 
  去掉非头帧流水号
*/
static void _Drv_CANProtocol_Formatting(DRV_CAN_RX_BUFFER_TYPE* p_rx_buffer);


#endif
