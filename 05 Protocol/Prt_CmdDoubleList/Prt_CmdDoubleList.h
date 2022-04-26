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
 
#ifndef _PRT_DOUBLELIST_H_
#define _PRT_DOUBLELIST_H_

#include "stdlib.h"
#include "stdbool.h"
#include "stdint.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef struct _DListNode
{
    void *data;
    struct _DListNode *prev;
    struct _DListNode *next;
}DListNode;

typedef struct _DList
{
    DListNode *head;
    DListNode *current;
}DList;

typedef enum _Ret
{
    RET_OK = 1,
    RET_FAULT,
    RET_OOM,
}Ret;

typedef uint8_t (*VisitFunc)(void *ctx, void* data);

typedef void (*DesFunc)(DList *dlist, int index);

DList *dlist_create(void);
int dlist_len(DList *dlist);
DListNode *dlist_get(DList *dlist, int index);
Ret dlist_delete(DList *dlist, int index);
Ret dlist_add(DList *dlist, int index,  void *data);
uint8_t dlist_foreach(DList *dlist, VisitFunc visit_func, void *ctx);
Ret dlist_destroy(DList *dlist,DesFunc des_func, int index);

#define return_val_if_fail(p, val) if(!(p)) {return val;}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* _PRT_DOUBLELIST_H_ */
