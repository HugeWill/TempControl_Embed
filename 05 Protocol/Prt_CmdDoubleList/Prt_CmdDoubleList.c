/********************************************************
   版权所有 (C), 2001-2100, 四川新健康成生物股份有限公司
  -------------------------------------------------------

				嵌入式开发平台——双向链表

  -------------------------------------------------------
  文 件 名   : Prt_DoubleList.c
  版 本 号   : V1.0.0.0
  作    者   : deng
  生成日期   : 2014年3月4日
  功能描述   : 通用双向链表常用函数
 ********************************************************/

#include "Prt_CmdDoubleList.h"

/*****************************************************************************
 函 数 名  : dlist_create
 功能描述  : dlist_create
 输入参数  : 无
 输出参数  : 无
 返 回 值  : DList
 
 修改历史      :
  1.日    期   : 2014年3月4日
    作    者   : deng

    修改内容   : 新生成函数

*****************************************************************************/
DList *dlist_create()
{
    DListNode* pnode = NULL;
    DList    * dlist = NULL;

    dlist = (DList *)calloc(1, sizeof(DList));
    if (dlist == NULL)
    {
        return NULL;
    }

    pnode = (DListNode *)calloc(1, sizeof(DListNode));
    
    if (pnode == NULL)
    {
        free(dlist);
        return NULL;
    }

    pnode->data = NULL;
    pnode->next = NULL;
    pnode->prev = NULL;

    dlist->head    = pnode;
    dlist->current = pnode;
    
    return dlist;
}

/*****************************************************************************
 函 数 名  : dlist_alloc
 功能描述  : 分配一个链表结点
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2014年4月22日
    作    者   : deng

    修改内容   : 新生成函数

*****************************************************************************/
static DListNode *dlist_alloc()
{
    DListNode * ret_p = NULL;

    ret_p = (DListNode *)calloc(1, sizeof(DListNode));

    if (NULL == ret_p)
    {
        return NULL;
    }

    return ret_p;   
}

/*****************************************************************************
 函 数 名  : data_dlist_check
 功能描述  : 
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2014年4月22日
    作    者   : deng

    修改内容   : 新生成函数

*****************************************************************************/
int data_dlist_check(DList *dlist,  void *data)
{
    if (dlist == NULL)
    {
        return 0;
    }    
    
    if (data == NULL)
    {
        return 0;
    }

    return 1;
}

/*****************************************************************************
 函 数 名  : dlist_add
 功能描述  : 添加元素
 输入参数  : dlist:指定链表  index: 添加的位置 data：加入的数据的指针
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2014年4月22日
    作    者   : deng

    修改内容   : 新生成函数

*****************************************************************************/
Ret dlist_add(DList *dlist, int index, void *data)
{
    int flag                = 1;
    int len                 = 0;
    DListNode *node         = NULL;
    DListNode *dlistnode    = NULL;

    flag = data_dlist_check(dlist, data);
    if (0 == flag)
    {
        return RET_FAULT;
    }

    len = dlist_len(dlist);
    return_val_if_fail(index <= len, RET_FAULT);
    
    if (index > len)
    {
        return RET_FAULT;
    }

    node = dlist_alloc();
    if (node == NULL)
    {
        return RET_OOM;
    }

    node->data = data;

    if (((len == 0) && (index == 0)) || (index < 0))
    {
        //tail
        dlist->current->next = node;
        node->prev = dlist->current;
        dlist->current = node;

        return RET_OK;
    }
    else if ((len != 0) && (index == 0))
    {
        //head
        dlist->head->next->prev = node;
        node->prev = dlist->head;
        node->next = dlist->head->next;
        dlist->head->next = node;

        return RET_OK;
    }
    else if ((len != 0) && (index != 0))
    {
        dlistnode = dlist_get(dlist, index);

        if (dlistnode == NULL)
        {
            return RET_FAULT;
        }
        dlistnode->prev->next = node;
        node->prev = dlistnode->prev;
        node->next = dlistnode;
        dlistnode->prev = node;

        return RET_OK;
    }

    return RET_FAULT;
}

/*****************************************************************************
 函 数 名  : dlist_delete
 功能描述  : 删除指定位置的值
 输入参数  : dlist:指定链表 index：位置 
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2014年4月22日
    作    者   : deng

    修改内容   : 新生成函数

*****************************************************************************/
Ret dlist_delete(DList *dlist, int index)
{
    int len = 0;
    DListNode *dlistnode = NULL;

    return_val_if_fail((dlist != NULL), RET_FAULT);

    if (dlist == NULL)
    {
        return RET_FAULT;
    }

    len = dlist_len(dlist);
    return_val_if_fail(index <= len, RET_FAULT);

    if (index > len)
    {
        return RET_FAULT;
    }

    if ((index > 0) && (index <len))
    {
        dlistnode = dlist_get(dlist, index);
        if (dlistnode == NULL)
        {
            return RET_FAULT;
        }

        dlistnode->next->prev = dlistnode->prev;
        dlistnode->prev->next = dlistnode->next;
        dlistnode->prev = NULL;
        dlistnode->next = NULL;
        dlistnode->data = NULL;
        free(dlistnode);

        return RET_OK;
    }
    else if ((index <= 0) || (index == len))
    {
        dlistnode = dlist->current->prev;
        dlist->current->prev->next = NULL;
        dlist->current->prev = NULL;
        dlist->current->data = NULL;
        free(dlist->current);
        dlist->current = dlistnode;

        return RET_OK;
    }

    return  RET_FAULT;
}

/*****************************************************************************
 函 数 名  : dlist_get
 功能描述  : 得到节点
 输入参数  : dlist:要操作的链表指针地址 index:位置
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2014年4月22日
    作    者   : deng

    修改内容   : 新生成函数

*****************************************************************************/
DListNode *dlist_get(DList *dlist, int index)
{
    int n = 1;
    DListNode *dlistnode = NULL;

    return_val_if_fail((dlist != NULL), NULL);

    dlistnode = dlist->head;
    while (n <= index)
    {
        dlistnode = dlistnode->next;
        n = n + 1;
    }

    return dlistnode;
}

/*****************************************************************************
 函 数 名  : dlist_foreach
 功能描述  : 遍历链表，执行visitfunc函数
 输入参数  : dlist:指定链表 visitfunc：回调函数 ctx：上下文
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2014年4月22日
    作    者   : deng

    修改内容   : 新生成函数

  2.日    期   : 2019年8月29日
    作    者   : ryc

    修改内容   : 返回执行状态

*****************************************************************************/
uint8_t dlist_foreach(DList *dlist, VisitFunc visit_func,void *ctx)
{
    DListNode *ipointer = NULL;
    uint8_t       retp     = 0;

    return_val_if_fail(((dlist != NULL) && (visit_func != NULL)), NULL);

    ipointer = dlist->head->next;              //指向表头的下一个节点
    while (ipointer != NULL)
    {
			retp = visit_func(ctx, ipointer->data);
			if(retp == 1)	/*返回true，表示已经执行*/
			{
				return 1;
			}
			if(retp == 0) /*不支持的指令*/
				return 0;
			ipointer = ipointer->next;
    }
		/*找不到指令*/
    return 0;
}

/*****************************************************************************
 函 数 名  : dlist_len
 功能描述  : 获得长度
 输入参数  : dlist:指定链表
 输出参数  : 
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2014年4月22日
    作    者   : deng

    修改内容   : 新生成函数

*****************************************************************************/
int dlist_len(DList *dlist)
{
    int len = 0;
    DListNode *dlistnode = NULL;

    return_val_if_fail((dlist != NULL), RET_FAULT);

    dlistnode = dlist->head;
    while (dlistnode->next != NULL)
    {
        dlistnode = dlistnode->next;
        len = len + 1;
    }

    return len;
}

/*****************************************************************************
 函 数 名  : dlist_destroy
 功能描述  : 释放指定链表内存
 输入参数  : dlist:指定链表
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2014年4月22日
    作    者   : deng

    修改内容   : 新生成函数

*****************************************************************************/
Ret dlist_destroy(DList *dlist, DesFunc des_func, int index)
{
    DListNode* cursor = NULL;

    if (des_func != NULL)
    {
        des_func(dlist, index);
    }
    else
    {
        while ((dlist->current != NULL) && (dlist->current != dlist->head))
        {
            cursor = dlist->current->prev;
            free(dlist->current->data);
            free(dlist->current);
            dlist->current = cursor ;
        }
        
        if (dlist->head != NULL)
        {
            free(dlist->head->data);
            free(dlist->head);
            dlist->current = NULL;
        }
        
        if (dlist != NULL)
        {
            free(dlist);
        }
    }

    return RET_OK;
}
