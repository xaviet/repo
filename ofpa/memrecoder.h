//  filename  memRecoder.h
//  desc      memRecoder h file (mount -t nfs -o nolock 10.0.0.250:/opt/nfs /mnt/nfs)
//  contact   pioevh@163.com
//  date      20161101

//  g_    global value
//  td_   type define
//  m_    member value
//  p_    point value
//  def_  macro define

#ifndef __memrecoderH__
#define __memrecoderH__

#include "stdio.h"

#ifndef def_debug
#define def_debug
#endif
//#undef def_debug

struct td_memOverflow
{
  void* m_addr;
  unsigned int m_malloc;
  unsigned int m_free;
  int m_id;
};

#define def_memRecoderNumber 2048
struct td_memOverflow g_memRecoder[def_memRecoderNumber]={0};

int memRecoder(void* v_addr,unsigned int v_malloc,unsigned int v_free,int v_id)
{
#ifdef def_debug
  int t_i=0,t_j=0;
  for(t_i=0;t_i<def_memRecoderNumber;t_i++)
  {
    if(g_memRecoder[t_i].m_addr==v_addr)
    {
      g_memRecoder[t_i].m_malloc+=v_malloc;
      g_memRecoder[t_i].m_free+=v_free;
      if(v_id!=0)
      {
        g_memRecoder[t_i].m_id=v_id;
      }
      for(t_j=0;t_j<def_memRecoderNumber;t_j++)
      {
        if((g_memRecoder[t_j].m_addr!=NULL)&&(g_memRecoder[t_j].m_malloc!=g_memRecoder[t_j].m_free))
        {
          printf("\n[%d malloc:%d free:%d]\n",g_memRecoder[t_j].m_id,g_memRecoder[t_j].m_malloc,g_memRecoder[t_j].m_free);
        }
      }
      return(0);
    }
  }
  for(t_i=0;t_i<def_memRecoderNumber;t_i++)
  {
    if(g_memRecoder[t_i].m_addr==NULL)
    {
      g_memRecoder[t_i].m_addr=v_addr;
      g_memRecoder[t_i].m_malloc+=v_malloc;
      g_memRecoder[t_i].m_free+=v_free;
      if(v_id!=0)
      {
        g_memRecoder[t_i].m_id+=v_id;
      }
      return(0);
    }
  }
#endif
  return(0);
}

#endif
