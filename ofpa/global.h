//  filename  global.h
//  desc      global h file
//  contact   pioevh@163.com
//  date      20160809

//  g_    global value
//  td_   type define
//  m_    member value
//  p_    point value
//  def_  macro define

#ifndef __globalH__
#define __globalH__

#include "linkedlistlib.h"
#include "commonlib.h"

#undef def_mockDev
//#define def_mockDev

#define def_ipaddressBuff 15
#define def_msgBuff 2048

#define def_ptnapiTcpPort 3000
#define def_ptnapiAddress "127.0.0.1"
#define def_ofpTcpPort 6633
#define def_ofpAddress "10.0.0.250"

#define def_SocketTx 1
#define def_SocketRx 2

#define def_ofpaLoopStart 101
#define def_ofpaLoopStop 102

#define def_ofpaLoopRun 1001

#define def_ofpaDevSocketInit 2001
#define def_ofpaDevSocket 2002
#define def_ofpaDevSocketOptCodeLogin 2003
#define def_ofpaDevSocketOptCodeDefaultCfg 2004
#define def_ofpaDevMock 2005

#define def_ofpaCtrlrSocketInit 3001
#define def_ofpaCtrlrSocket 3002
#define def_ofpaCtrlrSocketOptCodeRx 3004

#define def_ptnapiPortMax 16
#define def_ptnapiLocalPortId 0xfffffffe

typedef int (*td_callbackLinklistNode)(struct td_linkedlistNode* v_linklistNode,void* v_p,int v_len);

struct td_ptnapiPort
{
  int portId;
  char portName[16];
  char portMac[6];
  int status;
  int peerNeId;
  int peerPortId;
  char peerPortName[16];
  char peerPortMac[6];
};

struct td_ptnapiData
{
  int neId;
  struct td_ptnapiPort local;
  struct td_ptnapiPort port[def_ptnapiPortMax];
};

struct td_ptnapiData g_ptnapiData={0};

char g_ptnapiAddress[def_ipaddressBuff]={0};
char g_ofpAddress[def_ipaddressBuff]={0};

struct td_linkedlist* g_pSocketLinkList=NULL;
struct td_linkedlist* g_pMsgLinkList=NULL;

int g_ptnapiSocketFd=0;
int g_ofpSocketFd=0;

#define def_rtNonZeroAbnormal(v_msg) \
  if(t_rt!=0) \
  { \
    logStr(v_msg,0); \
    logInt(t_rt,1); \
    ofpaMsgPut(def_ofpaLoopStop,0,0,NULL,0); \
    return(t_rt); \
  }

#define def_rtZeroAbnormal(v_msg) \
  if(t_rt==0) \
  { \
    logStr(v_msg,1); \
    t_rt=-1; \
    ofpaMsgPut(def_ofpaLoopStop,0,0,NULL,0); \
    return(t_rt); \
  } \
  else \
  { \
    t_rt=0; \
  }

#define def_switch(v_Id,v_function) \
  case v_Id: \
    v_function; \
    break;
    
#endif
