//  filename  main.c
//  desc      main c file (mount -t nfs -o nolock 10.0.0.250:/opt/nfs /mnt/nfs)
//  contact   pioevh@163.com
//  date      20160809

//  g_    global value
//  td_   type define
//  m_    member value
//  p_    point value
//  def_  macro define

#include "commonlib.h"
#include "ofpa.c"
#include "global.h"

//  command:  ofpa ctrlr_ip_address dev_ip_address
int main(int argc,char *argv[])
{
  int t_rt=0;
  mc=0;
  mf=0;
  g_startTime=time(&g_startTime);
  t_rt=ofpaInit(argc,argv);
  def_rtNonZeroAbnormal("ofpa init Fail! code: ");
  t_rt=ofpaExec();
  def_rtNonZeroAbnormal("ofpa loop Fail! code: ");
  t_rt=ofpaFinal();
  def_rtNonZeroAbnormal("ofpa Final Fail! code: ");
  return(t_rt);
}
