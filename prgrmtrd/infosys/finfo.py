#!/usr/bin/env python3
# coding=utf-8

'''
finfo.py
'''

import sys
import os
sys.path.append('..{0}libpy'.format(os.sep))
import sqlite3lib
import httplib
import commonlib
import time
import socketlib

def init():
  commonlib.getcfg()
  socketlib.initudpCliSock()
  return(0)

def destory():
  socketlib.closeudpCliSock()
  return(0)
  
def getwebdatadate():
  t_touchcode=['sh000001','sz399001']
  t_array=httplib.getsinadata(t_touchcode)
  t_date=0
  if(len(t_array)>0):
    if(t_array[0][31]==t_array[1][31]):
      t_date=t_array[0][31]
  return(int(t_date))

def getlastdaydate():
  t_date=0
  if(os.path.exists(sqlite3lib.bigvolumedb)):
    t_opt='select lastdaydate from \'lastdaydate\' where recod=1 order by lastdaydate desc;'
    try:
      t_date=sqlite3lib.optrecod(sqlite3lib.bigvolumedb,t_opt)[0][0]
    except:
      pass
  return(t_date)

def savedailytradedata(v_lastdaydata,v_lastdaydate):
  commonlib.msg('savedailytradedata running',True)
  t_opt='create table if not exists \'lastdaydata\'(code integer primary key NOT NULL,volume integer,lastend real,bvcount integer,win integer,lose integer,meanincome real);'
  sqlite3lib.optrecod(sqlite3lib.bigvolumedb,t_opt)
  t_optbatch=[]
  for t_el in v_lastdaydata:
    if(len(t_el)==3 and float(t_el[2])<256):
      t_opt='select * from \'lastdaydata\' where code={0};'.format(str(t_el[0]))
      t_olddata=sqlite3lib.optrecod(sqlite3lib.bigvolumedb,t_opt)
      t_str='0,0,0,0'
      if(len(t_olddata)>0):
        t_str='{0},{1},{2},{3}'.format(t_olddata[0][3],t_olddata[0][4],t_olddata[0][5],t_olddata[0][6])
      t_optbatch+=['replace into \'lastdaydata\'(code,volume,lastend,bvcount,win,lose,meanincome) values({0},{1},{2},{3});'.format(t_el[0],t_el[1],t_el[2],t_str)]
  sqlite3lib.optrecodbatch(sqlite3lib.bigvolumedb,t_optbatch)
  t_opt='create table if not exists \'lastdaydate\'(recod integer primary key NOT NULL,lastdaydate integer);'
  sqlite3lib.optrecod(sqlite3lib.bigvolumedb,t_opt)
  t_opt='replace into \'lastdaydate\'(recod,lastdaydate) values({0},{1});'.format('1',str(v_lastdaydate))
  sqlite3lib.optrecod(sqlite3lib.bigvolumedb,t_opt)
  commonlib.msg('savedailytradedata run',True)
  return(0)

def getdaily():
  commonlib.msg('getdaily running',True)
  t_lastdaydata=[]
  t_prcs=int(commonlib.cfgvalue('processes'))
  t_codegroup=commonlib.arraysplit(commonlib.codelist(True),256)
  t_codegroup=commonlib.arraysplit(t_codegroup,(len(t_codegroup)//int(commonlib.cfgvalue('dailyprocesses'))+1))
  t_paras=[]
  for t_el0 in t_codegroup:
    t_paras.append((t_el0,))
  t_lastdaydata=commonlib.multiprcs(t_prcs,commonlib.getdailyprcs,t_paras)    
  commonlib.msg('getdaily run',True)
  return(t_lastdaydata)
 
def savebigvolume(v_bigvolume,v_webdate):
  commonlib.msg('savebigvolume running',True)
  t_opt='create table if not exists \'bigvolume\'(bigvolumedate integer primary key NOT NULL,bigvolumedata text,analyzed integer);'
  sqlite3lib.optrecod(sqlite3lib.bigvolumedb,t_opt)
  t_str=commonlib.array2d2str(v_bigvolume,',','\n')
  t_opt='replace into \'bigvolume\'(bigvolumedate,bigvolumedata,analyzed) values({0},\'{1}\',0);'.format(str(v_webdate),t_str)
  sqlite3lib.optrecod(sqlite3lib.bigvolumedb,t_opt)
  commonlib.msg('savebigvolume run',True)
  return(0)
  
def getbigvolume(v_webdate):
  commonlib.msg('getbigvolume running',True)
  t_prcs=int(commonlib.cfgvalue('processes'))
  t_threshold=float(commonlib.cfgvalue('bigvolumethreshold'))
  t_threshold=t_threshold if(t_threshold!=0) else 0.01
  t_opt='select * from \'lastdaydata\';'
  t_codegroup=sqlite3lib.optrecod(sqlite3lib.bigvolumedb,t_opt)
  t_codegroup=commonlib.arraysplit(t_codegroup,(len(t_codegroup)//int(commonlib.cfgvalue('bigvolumeprocesses'))+1))
  t_paras=[]
  for t_el0 in t_codegroup:
    t_paras.append([t_el0,t_threshold])
  t_bigvolume=commonlib.multiprcs(t_prcs,commonlib.getbigvolumeprcs,t_paras)
  savebigvolume(t_bigvolume,v_webdate)
  commonlib.msg('getbigvolume run',True)
  return(0)

def countbvdata(v_array,v_bvdate,v_winlimit,v_loselimit):  #v_array:price,time,code,lastend,
  t_db='{0}{1}.sdb'.format(sqlite3lib.dailydb,str(v_array[2]))
  t_opt='select * from \'daily\' where updatetime<{0} order by updatetime desc;'.format(str(v_bvdate))
  t_array=sqlite3lib.optrecod(t_db,t_opt)
  t_code=v_array[2]
  t_price=float(v_array[0])
  t_high=float(t_array[0][3])
  t_end=float(t_array[0][2])
  t_str='{0},{1},{2},{3}'.format(str(t_code),str(t_price),str(t_high),str(t_end))
  t_opt='select * from \'lastdaydata\' where code={0};'.format(t_code) 
  t_array=sqlite3lib.optrecod(sqlite3lib.bigvolumedb,t_opt)
  t_bvcount=(int(t_array[0][3]) if(t_array[0][3]!=None) else 0)+1
  t_win=(int(t_array[0][4]) if(t_array[0][4]!=None) else 0)
  t_lose=(int(t_array[0][5]) if(t_array[0][5]!=None) else 0)
  t_lastincome=(float(t_array[0][6]) if(t_array[0][6]!=None) else 0.0)
  t_income=0
  if(((t_high-t_price)/t_price)>v_winlimit):
    t_win+=1
    t_income=v_winlimit
  else:
    t_income=(t_end-t_price)/t_price
    if(((t_end-t_price)/t_price)<=v_loselimit):
      t_lose+=1
  t_meanincome=(((t_bvcount-1)*t_lastincome)+t_income)/t_bvcount
  t_opt='replace into \'lastdaydata\'(code,volume,lastend,bvcount,win,lose,meanincome)\
      values({0},{1},{2},{3},{4},{5},{6});'.\
      format(t_array[0][0],t_array[0][1],t_array[0][2],str(t_bvcount),str(t_win),str(t_lose),str(t_meanincome)) 
  t_array=sqlite3lib.optrecod(sqlite3lib.bigvolumedb,t_opt)
  return(t_str) #code,price,high,end,
  
def analyzebigvolume():
  commonlib.msg('analyzebigvolume running',True)
  t_winlimit=float(commonlib.cfgvalue('winlimit'))
  t_loselimit=float(commonlib.cfgvalue('loselimit'))
  t_opt='select lastdaydate from \'lastdaydate\' where recod=1;'
  t_dailydate=int(sqlite3lib.optrecod(sqlite3lib.bigvolumedb,t_opt)[0][0])
  t_opt='select * from \'bigvolume\' where analyzed=0 order by bigvolumedate;'
  t_codegroup=sqlite3lib.optrecod(sqlite3lib.bigvolumedb,t_opt)
  for t_el0 in t_codegroup:
    if(len(t_el0)>0):
      t_bvdate=int(t_el0[0])
      if(t_dailydate>t_bvdate):
        t_str=''
        t_a0=t_el0[1].split('\n')
        if(len(t_a0)>0):
          for t_el1 in t_a0:
            t_a1=t_el1.split(',')
            if(len(t_a1)>=4): #price,time,code,lastend,
              t_str+=countbvdata(t_a1,t_bvdate,t_winlimit,t_loselimit)+'\n'
        t_opt='replace into \'bigvolume\'(bigvolumedate,bigvolumedata,analyzed) values({0},\'{1}\',1);'.format(t_el0[0],t_str)
        sqlite3lib.optrecod(sqlite3lib.bigvolumedb,t_opt)
  commonlib.msg('analyzebigvolume run',True)
  return(0)
  
def intime(v_low=930,v_high=1500):
  t_rt=False
  t_ct=int(time.strftime('%2H%2M',time.localtime()))
  if(t_ct>=v_low and t_ct<=v_high):
    t_rt=True
  return(t_rt)

def outtime(v_low=859,v_high=1501):
  t_rt=False
  t_ct=int(time.strftime('%2H%2M',time.localtime()))
  if(t_ct<=v_low or t_ct>=v_high):
    t_rt=True
  return(t_rt)
  
def policysllep():
  time.sleep(commonlib.policyperoid)
  return(0)
  
def timesleep(v_time):
  time.sleep(v_time)
  return(0)

def msg(v_str,v_rollmode=False,v_newlinemode=False):
  commonlib.msg(v_str,v_rollmode,v_newlinemode)
  return(0)
    
def dailyrun(v_webdate,v_dbdate):
  commonlib.msg('dailyrun running',True)
  if(v_webdate>v_dbdate and v_dbdate!=0):
    getbigvolume(v_webdate)
  else:
    commonlib.msg('getbigvolume is not run',True)
  if(v_webdate!=v_dbdate and v_webdate!=0):
    t_dailydata=getdaily()
    savedailytradedata(t_dailydata,v_webdate)
  analyzebigvolume()
  commonlib.msg('dailyrun run',True)
  return('run')

def bvpolicypoolupdate(v_msgarray): #price;time;code;lastend
  t_array=[]
  for t_el in commonlib.bvpolicyarray:  #code,volume,lastend,threshold,weight,price,volume,successflag,
    t_array.append([commonlib.array2dremove(t_el[0],v_msgarray,0,2),t_el[1]])  
  for t_el in v_msgarray:
    t_opt='select bvcount,win,lose,meanincome from lastdaydata where code={0};'.format(t_el[2])
    t_hismsg=sqlite3lib.optrecod(sqlite3lib.bigvolumedb,t_opt)  #bvcount,win,lose,meanincome,
    commonlib.msg('[{0},\t{1},\t{2},\t{3},\t{4},\t{5},\t{6}]'.
    format(str(t_el[1]),str(t_el[2]),str(t_el[0]),t_hismsg[0][0],t_hismsg[0][1],t_hismsg[0][2],t_hismsg[0][3]),True,True)
  return(t_array) #code,
  
def getpool(v_poolvolume=0):
  commonlib.msg('policyrun running',True)
  t_pool=v_poolvolume if(v_poolvolume!=0) else int(commonlib.cfgvalue('pool'))
  t_opt='select * from \'lastdaydata\' order by meanincome;'
  t_array=sqlite3lib.optrecod(sqlite3lib.bigvolumedb,t_opt) #code,volume,lastend,bvcount,win,lose,meanincome,
  t_i=len(t_array)
  t_policypool=[]
  if(t_i>0):
    t_pool=(t_pool if(t_i>=t_pool) else t_i)
    t_threshold=float(commonlib.cfgvalue('bigvolumethreshold'))
    t_policyprice=float(commonlib.cfgvalue('policyprice'))
    t_threshold=t_threshold if(t_threshold!=0) else 0.01
    for t_el0 in t_array:
      if(float(t_el0[2])<t_policyprice):
        t_weight=(int(t_el0[4])/(t_el0[5] if(t_el0[5]>0) else 0.382))+t_el0[6]*382  #win/lose+meanincome*382
        t_policypool.append([t_el0[0],t_el0[1],t_el0[2],t_threshold,t_weight,0.0,0,0])  #code,volume,lastend,threshold,weight,price,volume,successflag,
    t_policypool.sort(key=lambda t_policypool:t_policypool[4],reverse=True)
    t_policypool=t_policypool[0:t_pool]
  else:
    commonlib.msg('get empty pool',True)
  return(t_policypool)

def policyrun():
  commonlib.msg('policyrun running',True)
  if(len(commonlib.bvpolicyarray)==0):
    t_policypool=getpool()
    if(len(t_policypool)>0):  
      t_policypool=commonlib.arraysplit(t_policypool,(len(t_policypool)//int(commonlib.cfgvalue('bigvolumeprocesses'))+1))
      t_threshold=float(commonlib.cfgvalue('bigvolumethreshold'))
      t_threshold=t_threshold if(t_threshold!=0) else 0.01
      for t_el0 in t_policypool:
        commonlib.bvpolicyarray.append([t_el0,t_threshold])
  if(len(commonlib.bvpolicyarray)>0):
    t_prcs=int(commonlib.cfgvalue('processes'))
    t_bvpolicymsg=(commonlib.multiprcs(t_prcs,commonlib.getbigvolumeprcs,commonlib.bvpolicyarray))  #price,time,code,lastend,
    if(len(t_bvpolicymsg)>0):
      socketlib.bvrunmsg(t_bvpolicymsg) #price,time,code,lastend,
      commonlib.bvpolicyarray=bvpolicypoolupdate(t_bvpolicymsg) #price,time,code,lastend,
  #print(commonlib.bvmsg)
  commonlib.msg('policyrun run',True,True)
  return(0)
  
if(__name__=='__main__'):
  #t_webdate=finfo.getwebdatadate()
  #t_dbdate=finfo.getlastdaydate() 
  #finfo.msg('webdate:{0},lastdaydate:{1}'.format(t_webdate,t_dbdate),True)
  #if(t_webdate!=t_dbdate and t_webdate!=0):
  #analyzebigvolume()
  #t_opt='select * from \'lastdaydata\';'
  #t_array=sqlite3lib.optrecod(sqlite3lib.bigvolumedb,t_opt)
  #for t_el in t_array:
    #t_opt='replace into \'lastdaydata\'(code,volume,lastend,bvcount,win,lose,meanincome)\
      #values({0},{1},{2},0,0,0,0);'.format(t_el[0],t_el[1],t_el[2]) 
    #t_array=sqlite3lib.optrecod(sqlite3lib.bigvolumedb,t_opt)
  #savedailytradedata([[1600000,2345,2],[1600004,1234,1]],20160331)
  s1=0
  s9=0
  t_a=getpool(382)
  for el in t_a:
    print(str(el))
    if(el[0]<9000000):
      s1+=1
    else:
      s9+=1
  print(s1,s9)

