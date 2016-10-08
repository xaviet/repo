#!/usr/bin/env python3
# coding=utf-8

'''
commonlib.py
'''

import sys
import re
import os
import time
import multiprocessing
import httplib
import sqlite3lib
import array
import socket

bvmsg=[]
bvpolicyarray=[]
puteveryday=0
serverip=''
serverport=0
winlimit=0
maxeverybuy=0
policyperoid=0

def getcfg():
  global puteveryday
  global serverip
  global serverport
  global winlimit
  global maxeverybuy
  global policyperoid
  puteveryday=float(cfgvalue('puteveryday'))
  serverip=cfgvalue('serverip')
  serverport=int(cfgvalue('serverport'))
  winlimit=float(cfgvalue('winlimit'))
  maxeverybuy=float(cfgvalue('maxeverybuy'))
  policyperoid=int(cfgvalue('policyperoid'))
  return(0)

def cfgvalue(v_str):
  t_dict={}
  t_cfgpath='..{0}prgrmtrd.cfg'.format(os.sep)
  t_cfg=open(t_cfgpath,mode='rt',encoding='utf-8')
  for t_el in t_cfg:
    t_el=re.sub(' ','',t_el)
    t_array=t_el.strip().split('=')
    t_dict[t_array[0]]=t_array[1]
  return(t_dict.get(v_str,'0'))
  
def array2string(v_array,v_split=','):
  return(v_split.join(v_array))

def codei2o(v_in):
  t_str=''
  if(v_in[0]=='1'):
    t_str='sh'+v_in[1:7]
  elif(v_in[0]=='9'):
    t_str='sz'+v_in[1:7]
  return(t_str)

def codeo2i(v_out):
  t_str=re.findall('[hz]\d{6}',v_out)[0]
  if(t_str[0]=='h'):
    t_str='1'+t_str[1:7]
  elif(t_str[0]=='z'):
    t_str='9'+t_str[1:7]
  return(t_str)

def codelist(v_index=True):
  t_sa=[]
  if(v_index):
    for i in range(0,2):
      t_sa.append('sh{0:06}'.format(1+i))
    for i in range(0,2):
      t_sa.append('sz{0:06}'.format(399001+i))
  for i in range(0,4500):
    t_sa.append('sh{0:06}'.format(600000+i))
  for i in range(0,3300): 
    t_sa.append('sz{0:06}'.format(000000+i))
  for i in range(0,1000):
    t_sa.append('sz{0:06}'.format(300000+i))        
  return(t_sa)

def arraysplit(v_array,v_group=512):
  t_len=len(v_array)
  t_start=0
  t_array=[]
  while(t_start<t_len):
    t_array.append(v_array[t_start:(t_start+v_group)])
    t_start+=v_group
  return(t_array)

def array2str(v_array,v_splitor=','):
  t_str=''
  if(len(v_array)>0):
    for t_el0 in v_array:
      t_str+=str(t_el0)+v_splitor
  return(t_str)

def array2d2str(v_array,v_splitor1=',',v_splitor2='\n'):
  t_str=''
  if(len(v_array)>0):
    for t_el0 in v_array:
      t_str+=array2str(t_el0,v_splitor1)+v_splitor2
  return(t_str)

def array2dremove(v_sarray,v_rmarray,v_selementpos=0,v_rmelementpos=0):
  t_rtarray=[]
  for t_el0 in v_sarray:
    for t_el1 in v_rmarray:
      if(t_el0[v_selementpos]==t_el1[v_rmelementpos]):
        break
    else:
      t_rtarray.append(t_el0)
  return(t_rtarray)
  
def savedaily(v_array):
  t_array=[]
  t_db=''
  t_opt=''
  for t_el in v_array:
    if(len(t_el)>31 and int(t_el[9])!=0):
      t_db='{0}{1}.sdb'.format(sqlite3lib.dailydb,t_el[0])
      t_opt='create table if not exists \'daily\' (updatetime integer primary key NOT NULL,todaybegin real,todayend real,todayhigh real,todaylow real,volume integer,turnover real);'
      sqlite3lib.optrecod(t_db,t_opt)
      t_opt='replace into \'daily\'(updatetime,todaybegin,todayend,todayhigh,todaylow,volume,turnover) values({0},{1},{2},{3},{4},{5},{6});'.format(t_el[31],float(t_el[2]),float(t_el[4]),float(t_el[5]),float(t_el[6]),int(t_el[9]),float(t_el[10]))
      sqlite3lib.optrecod(t_db,t_opt)
      t_array+=[[t_el[0],t_el[9],t_el[4]]]
  return(t_array)
    
def getdailyprcs(v_codegroup):
  msg('daily: '+str(v_codegroup[0][0])+' start',False)
  t_array=[]
  if(len(v_codegroup)>0):
    for t_el in v_codegroup:
      t_array+=httplib.getsinadata(t_el)
  t_array=savedaily(t_array)
  msg('daily: '+str(v_codegroup[0][0])+' over',False)
  return(t_array)

def getbigvolumeprcs(v_codegroup,v_threshold=0.01,v_bvrunmsgmode=False):
  t_array=[]
  t_level=0
  msg('bv: '+str(v_codegroup[0][0])+' start',False)
  for t_el in v_codegroup:  #code,volume,lastend,threshold,weight,
    t_specvalue=int((t_el[1]*0.01)*v_threshold)
    if(t_specvalue>2000):
      t_level=2000
    elif(t_specvalue>1000):
      t_level=1000
    elif(t_specvalue>500):
      t_level=500
    elif(t_specvalue>400):
      t_level=400
    else:
      continue
    t_data=httplib.getbigvolumedata(t_el[0],t_specvalue,t_level,float(t_el[2]))
    if(len(t_data)>0):
      t_array.append(t_data)  
  msg('bv: '+str(v_codegroup[0][0])+' over',False)
  return(t_array)
    
def multiprcs(v_prcs,v_prcsfunction,v_paras):
  t_lastdaydata=[]
  t_prcspool=multiprocessing.Pool(processes=v_prcs)
  t_result=[]
  for t_i in range(len(v_paras)):
    t_result.append(t_prcspool.apply_async(v_prcsfunction,v_paras[t_i]))
  t_prcspool.close()
  t_prcspool.join()
  for t_res in t_result:
    t_lastdaydata+=t_res.get()
  return(t_lastdaydata)
   
def msg(v_msg,v_rollmode=False,v_newlinemode=False):
  t_ctime=time.strftime("[%04Y-%02m-%02d_%2H:%2M:%2S] ",time.localtime())
  if(v_rollmode==False):
    sys.stdout.write('\r'+(' '*80)+'\r'+('\n' if(v_newlinemode) else '')+t_ctime+v_msg)
  else:
    print(('\n' if(v_newlinemode) else '')+t_ctime+v_msg)
  sys.stdout.flush()
  return([])

def checksum(v_data):
  print(v_data)
  if len(v_data) % 2:
    v_data+=b'\x00'
  print(v_data)
  t_data=[]
  for t_i in range(0,len(v_data)>>1):
    t_data.append(((int(v_data[2*t_i])&0xff)<<8)+(int(v_data[2*t_i+1])&0xff))
  print(t_data)
  s=sum(array.array('H',t_data))
  print('0x%08x'%s)
  s=(s&0xffff)+(s>>16)
  print('0x%08x'%s)
  s=(~(s+(s>>16)))&0xffff
  print('0x%08x'%s)
  return(s)

if(__name__=='__main__'):
  print(array2dremove([[1,'1'],[2,'2'],[3,'3',3],[4,4,4],[5,5,5,5]],[[2,1],[3,4]]))
  a=checksum([1,2,3,4,5,6,7,8])
  print(checksum([1,2,3,4,5,6,7,8,(a>>8)&0xff,(a&0xff)]))
  a=checksum([255,255,255,255])
  print(checksum([255,255,255,255,(a>>8)&0xff,(a&0xff)]))
  print(checksum([0x00,0x01,0xf2,0x03,0xf4,0xf5,0xf6,0xf7]))
    
  
