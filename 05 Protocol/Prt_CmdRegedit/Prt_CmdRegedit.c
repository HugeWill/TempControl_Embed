/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——指令注册表

  -------------------------------------------------------
   文 件 名   : Prt_CmdRegedit.c
   版 本 号   : V1.0.0.0
   作    者   : ryc
   生成日期   : 2019年07月25日
   功能描述   : 生成指令注册表
   依赖于     : Prt_DoubleList V1.0.0.0
				Func_Common V1.0.0.0
 ********************************************************/
 
#include "Prt_CmdRegedit.h"
 
CMD_REGEDIT_TYPE* _gp_CmdRegeditUnits[CMD_REGEDIT_BUTT] = {0};	/*指令注册表*/

/* 接口函数
 ********************************************************/
 
/* 初始化指令注册表
  ----------------------
  入口：指令注册表编号，模块ID号
  返回值：指令注册表指针
*/
CMD_REGEDIT_TYPE* Cmd_Regedit_Init(CMD_REGEDIT_ENUM id, CMD_MODULE_ID_ENUM module_id)
{
	CMD_REGEDIT_TYPE* p_unit = (CMD_REGEDIT_TYPE*)calloc(1, sizeof(CMD_REGEDIT_TYPE));	/*申请内存*/

	/*分配失败返回NULL*/
	if(p_unit == NULL)
	{
		return NULL;
	}
		
	/*id错误返回NULL*/
	if(id >= CMD_REGEDIT_BUTT)
	{
		free(p_unit); 
		return NULL;
	}
	
	/*如果当前模块存在，取消分配新存储区*/
	if(_gp_CmdRegeditUnits[id] != NULL)
	{
		free(p_unit);
		p_unit = _gp_CmdRegeditUnits[id];
	}
	else
	{
		_gp_CmdRegeditUnits[id] = p_unit;
	}
	
	/*初始化*/
	
	/*-------------------------------常量*/
	p_unit->id = id;
	p_unit->module_id = module_id;
	p_unit->dlist = dlist_create();
	
	/*-------------------------------初始化驱动层*/
	Func_Common_Init(p_unit->dlist);
		
	return p_unit;
}

/* 内部函数
 ********************************************************/

/* 添加单个指令到指令注册表
  ----------------------
  入口：指令注册表首地址，命令字，处理函数
  返回值：成功true，失败false
*/
bool Cmd_AddCmdNode(DList* dlist, uint16_t cmd, FUNC orderfun)
{
    CMD_REGEDIT_NODE_TYPE* cmd_str_q = _Cmd_Malloc(cmd, orderfun);

    if(NULL == cmd_str_q)
    {
        return false;
    }
    dlist_add(dlist, 0, cmd_str_q);
	
    return true;
}

/* 申请一个指令注册表节点
  ----------------------
  入口：命令字，处理函数
  返回值：申请的指令注册节表点
*/
static CMD_REGEDIT_NODE_TYPE* _Cmd_Malloc(uint16_t cmd, FUNC orderfun)
{
    CMD_REGEDIT_NODE_TYPE *order_str_p = (CMD_REGEDIT_NODE_TYPE *)calloc(1, sizeof(CMD_REGEDIT_NODE_TYPE));

    if (NULL == order_str_p)
    {
        return NULL;
    }

    order_str_p->orderfun = orderfun;
    order_str_p->cmd = cmd;

    return order_str_p;
}
