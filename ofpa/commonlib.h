//  filename  commonlib.h
//  desc      common library h file
//  contact   pioevh@163.com
//  date      20160809

//  g_    global value
//  td_   type define
//  m_    member value
//  p_    point value
//  def_  macro define

#ifndef __commonlibH__
#define __commonlibH__

#include "linkedlistlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "sys/types.h"
#include "string.h"
#include "sys/time.h"
#include "time.h"
#include "unistd.h"
#include "arpa/inet.h"
#include "regex.h"

#ifndef def_debug
#define def_debug
//#undef def_debug
#endif

#define def_stringBuff 65535

int senseBigEndian(void)
{
  int t_rt=1;
  uint16_t tTestEndian=0x0001;
  if(*((char*)&tTestEndian)==0x01)
  {
    t_rt=0;
  }
  return(t_rt);
}

int logStr(const char v_chars[def_stringBuff],int v_rt)
{
#ifdef def_debug
  time_t t_time=0;
  t_time=time(&t_time);
  if(strlen(v_chars)>0)
  {
    printf("[%012ld]  %s",t_time,v_chars);
  }
  if(v_rt)
  {
    printf("\n");
  }
#endif
  return(0);
}

int logInt(const int v_int,int v_rt)
{
#ifdef def_debug
  printf("%d",v_int);
  if(v_rt)
  {
    printf("\n");
  }
#endif
  return(0);
}

uint16_t swapEndian16(const uint16_t v_int16)
{
  uint16_t t_rt=v_int16;
  if(senseBigEndian()==0)
  {
    t_rt=((v_int16&0xff00)>>8)|((v_int16&0x00ff)<<8);
  }
  return(t_rt);
}

uint32_t swapEndian32(const uint32_t v_int32)
{
  uint32_t t_rt=v_int32;
  if(senseBigEndian()==0)
  {
    t_rt=((v_int32&0xff000000)>>24)|((v_int32&0x00ff0000)>>8)|\
        ((v_int32&0x0000ff00)<<8)|((v_int32&0x000000ff)<<24);
  }
  return(t_rt);
}

uint64_t swapEndian64(const uint64_t v_int64)
{
  uint64_t t_rt=v_int64;
  if(senseBigEndian()==0)
  {
    t_rt=(uint64_t)(((v_int64&(uint64_t)0xff00000000000000LL)>>56)|\
        ((v_int64&(uint64_t)0x00ff000000000000LL)>>40)|\
        ((v_int64&(uint64_t)0x0000ff0000000000LL)>>24)|\
        ((v_int64&(uint64_t)0x000000ff00000000LL)>>8)|\
        ((v_int64&(uint64_t)0x00000000ff000000LL)<<8)|\
        ((v_int64&(uint64_t)0x0000000000ff0000LL)<<24)|\
        ((v_int64&(uint64_t)0x000000000000ff00LL)<<40)|\
        ((v_int64&(uint64_t)0x00000000000000ffLL)<<56));
  }
  return(t_rt);
}

char* regexGetLine(const char* v_pOriginStr,const char* v_reg)
{
  char* t_str=NULL;
  t_str=(char*)malloc(def_stringBuff);
  regex_t t_oRegex;
  regmatch_t t_match[1];
  const size_t t_nmatch=1;
  int t_len=0;
  regcomp(&t_oRegex,v_reg,0);
  if(regexec(&t_oRegex,v_pOriginStr,t_nmatch,t_match,0)!=0)
  {
    return(0);
  }
  t_len=t_match[0].rm_eo-t_match[0].rm_so;
  memcpy(t_str,v_pOriginStr+t_match[0].rm_so,t_len);
  *(t_str+t_len)='\0';
  regfree(&t_oRegex);
  return(t_str);
}

int mac6CharsFrom6Ints(char v_ch[6],int v_int[6])
{
  int t_i=0;
  for(t_i=0;t_i<6;t_i++)
  {
    v_ch[t_i]=(char)(v_int[t_i]);
  }
  return(0);
}

#endif
