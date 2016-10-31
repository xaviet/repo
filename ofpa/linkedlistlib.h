//  filename  linkedlistlib.h
//  desc      linkedlist library h file
//  contact   pioevh@163.com
//  date      20160809

//  g_    global value
//  td_   type define
//  m_    member value
//  p_    point value
//  def_  macro define

#ifndef __linkedlistlibH__
#define __linkedlistlibH__

#include "stdlib.h"
#include "string.h"

int mc,mf;

#define def_tdlinkedlistNodeDefault {0,0,0,NULL,0,0,0,NULL}
struct td_linkedlistNode
{
  int m_handle;
  int m_optcode;
  int m_status;
  void* m_pbuff;
  int m_length;
  int m_mtype;
  int m_stype;
  struct td_linkedlistNode* m_pnext;
};

struct td_linkedlist
{
  struct td_linkedlistNode* m_ptail;
  struct td_linkedlistNode* m_phead;
};

struct td_linkedlist* linkedlistCreate(void)
{
  struct td_linkedlist* t_plinkedlist=NULL;
  t_plinkedlist=(struct td_linkedlist*)malloc(sizeof(struct td_linkedlist));printf("\nm:%d  f:%d\n",mc++,mf);
  t_plinkedlist->m_phead=t_plinkedlist->m_ptail=NULL;
  return(t_plinkedlist);
}

int linkedlistPut(struct td_linkedlist* v_plinkedlist,int v_handle,int v_optcode,int v_status,void* v_pbuff,int v_len,int v_mtype,int v_stype)
{
  int t_rt=0;
  struct td_linkedlistNode* t_plinkedlistNode=NULL;
  if(v_plinkedlist!=NULL)
  {
    t_plinkedlistNode=(struct td_linkedlistNode*)malloc(sizeof(struct td_linkedlistNode));printf("\nm:%d  f:%d\n",mc++,mf);
    t_plinkedlistNode->m_handle=v_handle;
    t_plinkedlistNode->m_optcode=v_optcode;
    t_plinkedlistNode->m_status=v_status;
    t_plinkedlistNode->m_pbuff=v_pbuff;
    t_plinkedlistNode->m_length=v_len;
    t_plinkedlistNode->m_mtype=v_mtype;
    t_plinkedlistNode->m_stype=v_stype;
    t_plinkedlistNode->m_pnext=NULL;
    if(v_plinkedlist->m_phead==NULL)
    {
      v_plinkedlist->m_phead=v_plinkedlist->m_ptail=t_plinkedlistNode;
    }
    else
    {
      v_plinkedlist->m_ptail->m_pnext=t_plinkedlistNode;
      v_plinkedlist->m_ptail=t_plinkedlistNode;
    }
    t_rt=1;
  }
  else
  {
    t_rt=-1;
  }
  return(t_rt);
}

struct td_linkedlistNode* linkedlistGet(struct td_linkedlist* v_plinkedlist)
{
  struct td_linkedlistNode* t_plinkedlistNode=NULL;
  if(v_plinkedlist->m_phead!=NULL)
  {
    t_plinkedlistNode=v_plinkedlist->m_phead;
    v_plinkedlist->m_phead=t_plinkedlistNode->m_pnext;
    t_plinkedlistNode->m_pnext=NULL;
  }
  else
  {
    t_plinkedlistNode=NULL;
  }
  return(t_plinkedlistNode);
}

int linkedlistNodeFree(struct td_linkedlistNode* v_plinkedlistNode)
{
  if(v_plinkedlistNode->m_pbuff!=NULL)
  {
    free(v_plinkedlistNode->m_pbuff);printf("\nm:%d  f:%d\n",mc,mf++);
    v_plinkedlistNode->m_pbuff=NULL;
  }
  free(v_plinkedlistNode);printf("\nm:%d  f:%d\n",mc,mf++);
  return(0);
}

int linkedlistDestory(struct td_linkedlist* v_plinkedlist)
{
  while(v_plinkedlist->m_phead!=NULL)
  {
    linkedlistNodeFree(linkedlistGet(v_plinkedlist));
  }
  free(v_plinkedlist);printf("\nm:%d  f:%d\n",mc,mf++);
  return(0);
}

int linkedlistLength(struct td_linkedlist* v_plinkedlist)
{
  int t_len=1;
  struct td_linkedlistNode* t_p=v_plinkedlist->m_phead;
  if(t_p==NULL)
  {
    return(0);
  }
  while(t_p!=v_plinkedlist->m_ptail)
  {
    t_len++;
    t_p=t_p->m_pnext;    
  }  
  return(t_len);
}

#endif
