/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——指令注册表

  -------------------------------------------------------
   文 件 名   : Prt_CmdRegedit.h
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年07月25日
   功能描述   : 生成指令注册表
   依赖于     : Prt_DoubleList V1.0.0.0
				Func_Common V1.0.0.0
 ********************************************************/

#ifndef _PRT_CMDREGEDIT_H_
#define _PRT_CMDREGEDIT_H_

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "Prt_CmdDoubleList.h"
#include "Common_Cmd.h"

/*处理函数结构定义*/
typedef bool(* FUNC)(void* p_msg);

/*模块ID枚举*/
typedef enum{
	MODULE_1 = 0,
	MODULE_2,
	MODULE_3,
	MODULE_4,
} CMD_MODULE_ID_ENUM;
/*指令注册表节点结构体*/
typedef struct {
	uint8_t cmd;	/*命令字*/
	FUNC orderfun;	/*处理函数*/
} CMD_REGEDIT_NODE_TYPE;

/*指令注册表枚举*/
typedef enum{
	CMD_REGEDIT_1 = 0,
	CMD_REGEDIT_2,
	CMD_REGEDIT_BUTT,
} CMD_REGEDIT_ENUM;

/*指令注册表结构体*/
typedef struct{	
	/*------------------------------常量*/
	CMD_REGEDIT_ENUM id;			/*注册表id*/
	CMD_MODULE_ID_ENUM module_id;	/*模块id*/
	DList* dlist;					/*指令注册表表头*/
} CMD_REGEDIT_TYPE;

/* 接口函数
 ********************************************************/
 
/* 初始化指令注册表
  ----------------------
  入口：指令注册表编号，模块ID号
*/
extern CMD_REGEDIT_TYPE* Cmd_Regedit_Init(CMD_REGEDIT_ENUM id, CMD_MODULE_ID_ENUM module_id);

/* 添加单个命令到命令注册表
  ----------------------
  入口：命令注册表地址，命令，处理函数
  返回值：成功true，失败false
*/
extern bool Cmd_AddCmdNode(DList* dlist, uint16_t cmd, FUNC orderfun);

/* 内部函数
 ********************************************************/


/* 申请一个命令节点存储空间
  ----------------------
*/
static CMD_REGEDIT_NODE_TYPE* _Cmd_Malloc(uint16_t cmd, FUNC orderfun);

#endif
