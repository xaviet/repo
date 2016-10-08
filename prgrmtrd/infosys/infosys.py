#!/usr/bin/env python3
# coding=utf-8

'''
infosys.py
'''

import finfo

def infosysrun():
  finfo.init()
  while(True):
    t_webdate=finfo.getwebdatadate()
    t_dbdate=finfo.getlastdaydate() 
    t_date='webdate:{0},lastdaydate:{1}'.format(t_webdate,t_dbdate)
    if(finfo.outtime(859,1501)):
      finfo.msg(t_date,True,True)
      finfo.dailyrun(t_webdate,t_dbdate) 
      finfo.msg('sleep 60s.',True)
      finfo.timesleep(60)
    elif(finfo.intime(940,1130) or finfo.intime(1300,1457)):
      pass
      finfo.msg(t_date,True,True)
      finfo.policyrun() 
      finfo.policysllep()
    else:
      finfo.msg('no operate! sleep 60s.',True,True)
      finfo.timesleep(60)
    finfo.destory()
  return(0)

if(__name__=='__main__'):
  infosysrun()
