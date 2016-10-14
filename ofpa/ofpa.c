//  filename  ofpa.c
//  desc      openflow protocol adapter c file
//  contact   pioevh@163.com
//  date      20160823

//  g_    global value
//  td_   type define
//  m_    member value
//  p_    point value
//  def_  macro define

#ifndef _def_msgBuff_ofpaC__
#define __ofpaC__

#include "ofplib.c"
#include "ofpa.h"
#include "ptnapilib.c"
#include "global.h"

int ofpaInitBigEndianMode()
{
  int t_rt=0;
  t_rt=senseBigEndian();
  logStr("endian mode (big=1): ",0);
  logInt(t_rt,1);
  return(t_rt);
}

int ofpaInitIpAddress(int v_argc,char *v_argv[])
{
  int t_rt=0;
  int t_ipMinLen=7;
  int t_len=0;
  (v_argc>=3)?(t_len=strlen(v_argv[2])):(t_len=0);
  ((t_len<=def_ipaddressBuff)&&(t_len>t_ipMinLen))? \
    strcpy(g_ptnapiAddress,v_argv[2]):strcpy(g_ptnapiAddress,def_ptnapiAddress);
  (v_argc>=2)?(t_len=strlen(v_argv[1])):(t_len=0);
  ((t_len<=def_ipaddressBuff)&&(t_len>t_ipMinLen))? \
    strcpy(g_ofpAddress,v_argv[1]):strcpy(g_ofpAddress,def_ofpAddress);
  logStr("ptnapi Address: ",1);logStr(g_ptnapiAddress,1);
  logStr("ofp Address: ",1);logStr(g_ofpAddress,1);
  return(t_rt);
}

struct td_linkedlist* ofpaInitList()
{
  struct td_linkedlist* t_pList=NULL;
  t_pList=(struct td_linkedlist*)malloc(sizeof(struct td_linkedlist));
  t_pList->m_phead=t_pList->m_ptail=NULL;
  return(t_pList); 
}

int ofpaInitMsgList()
{
  logStr("ofpa init msg list!",1);
  g_pMsgLinkList=ofpaInitList();
  return((int)g_pMsgLinkList);
}

int ofpaInitSocketList()
{
  logStr("ofpa init socket list!",1);
  g_pSocketLinkList=ofpaInitList();
  return((int)g_pSocketLinkList);
}

int ofpaInit(int v_argc,char *v_argv[])
{
  int t_rt=0;
  ofpaInitBigEndianMode();
  t_rt=ofpaInitIpAddress(v_argc,v_argv);
  def_rtNonZeroAbnormal("ofpa init ip address Fail! code: ");
  t_rt=ofpaInitMsgList();
  def_rtZeroAbnormal("ofpa init msg list Fail! code: ");
  t_rt=ofpaInitSocketList();
  def_rtZeroAbnormal("ofpa init socket list Fail! code: ");
  return(t_rt);
}

int ofpaMsgPut(int v_msgId,int v_optcode,int v_status,void* v_p,int v_len)
{
  linkedlistPut(g_pMsgLinkList,v_msgId,v_optcode,v_status,v_p,v_len,0,0);
  logStr("ofpa Msg Put ID: ",0);logInt(v_msgId,1);
  logStr("ofpa Msg Put optcode: ",0);logInt(v_optcode,1);
  logStr("ofpa Msg Link List deep: ",0);logInt(linkedlistLength(g_pMsgLinkList),1);
  return(0);
}

struct td_linkedlistNode* ofpaLinkListGet(struct td_linkedlist* v_pLinkList)
{
  return(linkedlistGet(v_pLinkList));
}

int ofpaLinkListNodeFree(struct td_linkedlistNode* v_pLinkListNode)
{
  logStr("ofpa LinkList Node Free ID: ",0);logInt(v_pLinkListNode->m_handle,1);
  logStr("ofpa LinkList Node Free optcode: ",0);logInt(v_pLinkListNode->m_optcode,1);
  linkedlistNodeFree(v_pLinkListNode);
  return(0);
}

int ofpaLoopStart()
{
#ifndef def_mockDev
  ofpaMsgPut(def_ofpaDevSocketInit,0,0,NULL,0);
#else
  ofpaMsgPut(def_ofpaDevMock,0,0,NULL,0);
  ofpaMsgPut(def_ofpaCtrlrSocketInit,0,0,NULL,0);
#endif
  ofpaMsgPut(def_ofpaLoopRun,0,0,NULL,0);
  return(0);
}

int ofpaLoopStop()
{
  ofpaFinalList(g_pMsgLinkList);
  return(0);
}

int ofpaDevSocketInit()
{
  g_ptnapiSocketFd=ptnapiSocketInit(g_ptnapiAddress,def_ptnapiTcpPort);
  logStr("ofpa Dev Socket Init: socketfd--",0);logInt(g_ptnapiSocketFd,1);
  logStr(g_ptnapiAddress,1);
  logStr("ofpa Dev Socket Init: port------",0);logInt(def_ptnapiTcpPort,1);
  if(g_ptnapiSocketFd!=0)
  {
    logStr("ofpa Dev Socket Init ok!",1);
    ofpaMsgPut(def_ofpaDevSocket,def_ofpaDevSocketOptCodeLogin,0,NULL,0);
    return(0);
  }
  else
  {
    logStr("ofpa Dev Socket Init fail! redo...",1);
    ofpaMsgPut(def_ofpaDevSocketInit,0,0,NULL,0);
    return(1);
  }
  return(0);
}

int ofpaCtrlrSocketInit()
{
  g_ofpSocketFd=ofpSocketInit(g_ofpAddress,def_ofpTcpPort);
  logStr("ofpa Ctrlr Socket Init: socketfd--",0);logInt(g_ofpSocketFd,1);
  logStr(g_ofpAddress,1);
  logStr("ofpa Ctrlr Socket Init: port------",0);logInt(def_ofpTcpPort,1);
  if(g_ofpSocketFd!=0)
  {
    logStr("ofpa Ctrlr Socket Init ok!",1);
    ofpaMsgPut(def_ofpaCtrlrSocket,def_ofpaCtrlrSocketOptCodeRx,0,NULL,0);
    return(0);
  }
  else
  {
    logStr("ofpa Ctrlr Socket Init fail! redo...",1);
    ofpaMsgPut(def_ofpaCtrlrSocketInit,0,0,NULL,0);
    return(1);
  }
  return(0);
}

int ofpaSocketReady(int v_socket)
{
    fd_set t_fd;
    struct timeval t_timeout={0,0};
    FD_ZERO(&t_fd);
    FD_SET(v_socket,&t_fd);
    return(select(v_socket+1,&t_fd,NULL,NULL,&t_timeout));
}
int ofpaSocketRxBuff(int v_socket,char* v_buff,int v_len)
{
   int t_rt=0;
   if(ofpaSocketReady(v_socket))
   {
    t_rt=recv(v_socket,v_buff,v_len,0);
   }
   return(t_rt);
}

int ofpaSocketRx(struct td_linkedlistNode* v_pSocketLinkList)
{
  int t_len=0;
  td_callbackLinklistNode t_callbackLinklistNode=(td_callbackLinklistNode)(v_pSocketLinkList->m_status);
  char* t_pBuff=g_socketBuff;
  memset(t_pBuff,0,def_stringBuff);
  t_len=ofpaSocketRxBuff(v_pSocketLinkList->m_handle,t_pBuff,def_stringBuff);
  if(t_callbackLinklistNode!=NULL)
  {
    if(v_pSocketLinkList->m_mtype==1)
    {
      while(t_callbackLinklistNode(v_pSocketLinkList,t_pBuff,t_len)==0)
      {
        t_len+=ofpaSocketRxBuff(v_pSocketLinkList->m_handle,t_pBuff+t_len,def_stringBuff-t_len);
        usleep(20000);
      }
    }
    else if(t_len>0)
    {
      t_callbackLinklistNode(v_pSocketLinkList,t_pBuff,t_len);
    }
  }
  logStr("ofpa Socket Rx(Bytes): ",0);logInt(t_len,1);
  return(t_len);
}

int ofpaSocketTx(struct td_linkedlistNode* v_pSocketLinkList)
{
  logStr("ofpa Socket Tx(Bytes): ",0);logInt(v_pSocketLinkList->m_length,1);
  if(v_pSocketLinkList->m_length>0)
  {
    send(v_pSocketLinkList->m_handle,v_pSocketLinkList->m_pbuff,v_pSocketLinkList->m_length,0);
    usleep(20000);
  }
  return(0);
}

int ofpaSocketProcess(struct td_linkedlist* v_pSocketLinkList)
{
  struct td_linkedlistNode* t_pSocket;
  while((t_pSocket=ofpaLinkListGet(v_pSocketLinkList))!=NULL)
  {
    switch(t_pSocket->m_optcode)
    {
      def_switch(def_SocketTx,ofpaSocketTx(t_pSocket));
      def_switch(def_SocketRx,ofpaSocketRx(t_pSocket));
      default:
        break;
    }
    ofpaLinkListNodeFree(t_pSocket); 
  }
  ofpaMsgPut(def_ofpaLoopRun,0,0,NULL,0); 
  return(0);
}

int ofpaLoopRun()
{
  ofpaSocketProcess(g_pSocketLinkList);
  return(0);
}

int ofpaDevSocketOptCodeLogin(struct td_linkedlist* v_pSocketLinkList,int v_ptnapiSocketFd)
{
  logStr("ofpa Dev Socket OptCode Login",1);
  if(ptnapiSocketLogin(v_pSocketLinkList,v_ptnapiSocketFd)!=0)
  {
    logStr("ofpa Dev Socket OptCode Login fail! ",1);
    ofpaMsgPut(def_ofpaDevSocket,def_ofpaDevSocketOptCodeLogin,0,NULL,0);
  }
  else
  {
    ofpaMsgPut(def_ofpaDevSocket,def_ofpaDevSocketOptCodeDefaultCfg,0,NULL,0);
  }
  return(0);
}

int ofpaDevSocketOptCodeDefaultCfg(struct td_linkedlist* v_pSocketLinkList,int v_ptnapiSocketFd)
{
  logStr("ofpa Dev Socket OptCode Default Cfg",1);
  if(ptnapiSocketOptCodeDefaultCfg(v_pSocketLinkList,v_ptnapiSocketFd)!=0)
  {
    logStr("ofpa Dev Socket OptCode Default Cfg fail!",1);
    ofpaMsgPut(def_ofpaLoopStop,0,0,NULL,0);
  }
  //Non Mock dev
  ofpaMsgPut(def_ofpaCtrlrSocketInit,0,0,NULL,0);
  return(0);
}

int ofpaDevMock()
{
  //Mock dev
  ptnapiMock();
  return(0);
}

int ofpaDevSocketOptCodeLldpInquiry(struct td_linkedlistNode* v_pMsg)
{
  logStr("ofpa Dev Socket OptCode Lldp Inquiry(portname) :",1);logStr(v_pMsg->m_pbuff,1);
  ptnapiLldpSignleInquery(v_pMsg->m_pbuff);
  return(0);
}

int ofpaDevSocket(struct td_linkedlistNode* v_pMsg)
{
  logStr("ofpa Dev Socket optcode: ",0);logInt(v_pMsg->m_optcode,1);
  switch(v_pMsg->m_optcode)
  {
    def_switch(def_ofpaDevSocketOptCodeLogin,ofpaDevSocketOptCodeLogin(g_pSocketLinkList,g_ptnapiSocketFd));
    def_switch(def_ofpaDevSocketOptCodeDefaultCfg,ofpaDevSocketOptCodeDefaultCfg(g_pSocketLinkList,g_ptnapiSocketFd));
    def_switch(def_ofpaDevSocketOptCodeLldpInquiry,ofpaDevSocketOptCodeLldpInquiry(v_pMsg));
    default:
      break;    
  }
  return(0);
}

int ofpaCtrlrSocketOptCodeRx(struct td_linkedlist* v_pSocketLinkList,int v_ofpSocketFd)
{
  logStr("ofpa Ctrlr Socket OptCode Rx",1);
  ofpSocketRx(v_pSocketLinkList,v_ofpSocketFd);
  return(0);
}

int ofpaCtrlrSocket(struct td_linkedlistNode* v_pMsg)
{
  logStr("ofpa Ctrlr Socket optcode: ",0);logInt(v_pMsg->m_optcode,1);
  switch(v_pMsg->m_optcode)
  {
    def_switch(def_ofpaCtrlrSocketOptCodeRx,ofpaCtrlrSocketOptCodeRx(g_pSocketLinkList,g_ofpSocketFd));
    default:
      break;    
  }
  ofpaMsgPut(def_ofpaCtrlrSocket,def_ofpaCtrlrSocketOptCodeRx,0,NULL,0);
  return(0);
}

int ofpaLldpInquiry(struct td_linkedlistNode* v_pMsg)
{
  logStr("ofpa Lldp Inquiry (portname): ",1);logStr(v_pMsg->m_pbuff,1);
  ofpaMsgPut(def_ofpaDevSocket,def_ofpaDevSocketOptCodeLldpInquiry,0,v_pMsg->m_pbuff,0);
  return(0);
}

int ofpaLoop()
{
  int t_rt=0;
  struct td_linkedlistNode* t_pMsg;
  while((t_pMsg=ofpaLinkListGet(g_pMsgLinkList))!=NULL)
  {
#ifdef def_debug    
    usleep(500000);
#endif
    usleep(1);
    switch(t_pMsg->m_handle)
    {
      def_switch(def_ofpaLoopStart,ofpaLoopStart());
      def_switch(def_ofpaLoopStop,ofpaLoopStop());
      def_switch(def_ofpaDevSocketInit,ofpaDevSocketInit());
      def_switch(def_ofpaDevMock,ofpaDevMock());
      def_switch(def_ofpaCtrlrSocketInit,ofpaCtrlrSocketInit());
      def_switch(def_ofpaDevSocket,ofpaDevSocket(t_pMsg));
      def_switch(def_ofpaCtrlrSocket,ofpaCtrlrSocket(t_pMsg));
      def_switch(def_ofpaLoopRun,ofpaLoopRun());
      def_switch(def_ofpaLldpInquiry,ofpaLldpInquiry(t_pMsg));
      default:
        break;
    }
    ofpaLinkListNodeFree(t_pMsg); 
  }
  return(t_rt);
}

int ofpaExec()
{
  int t_rt=0;
  ofpaMsgPut(def_ofpaLoopStart,0,0,NULL,0);
  logStr("ofpa Exec",1);
  t_rt=ofpaLoop();
  return(t_rt);
}

int ofpaFinalList(struct td_linkedlist* v_pList)
{
  return(linkedlistDestory(v_pList));
}

int ofpaFinal()
{
  int t_rt=0;
  logStr("ofpa final socket list!",1);
  t_rt=ofpaFinalList(g_pSocketLinkList);
  def_rtNonZeroAbnormal("ofpa final socket list Fail!");
  logStr("ofpa final msg list!",1);
  t_rt=ofpaFinalList(g_pMsgLinkList);
  def_rtNonZeroAbnormal("ofpa final msg list Fail!");
  ptnapiSocketFinal(g_ptnapiSocketFd);
  logStr("ofpa ptnapi Socket Final: ",0);logInt(g_ptnapiSocketFd,1);
  ofpSocketFinal(g_ofpSocketFd);
  logStr("ofpa ofp Socket Final: ",0);logInt(g_ofpSocketFd,1);
  return(t_rt);
}

#endif
