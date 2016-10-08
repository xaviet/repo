//  filename  ofpa.h
//  desc      openflow protocol adapter h file
//  contact   pioevh@163.com
//  date      20160901

//  g_    global value
//  td_   type define
//  m_    member value
//  p_    point value
//  def_  macro define

#ifndef __ofpaH__
#define __ofpaH__

char g_socketBuff[def_stringBuff]={0};

int ofpaFinalList();
int ofpaMsgPut(int v_msgId,int v_optcode,int v_status,void* v_p,int v_len);

#endif
