#!/usr/bin/env python3
# coding=utf-8

'''
socketlib.py
'''

import commonlib
import socket
import time

udpCliSock=None

def initudpCliSock():
  global udpCliSock
  udpCliSock=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
  udpCliSock.bind(('',commonlib.serverport))
  udpCliSock.setblocking(0)
  return(0)
  
def closeudpCliSock():
  global udpCliSock
  udpCliSock.close()
  return(0)
  
def sendbvrunmsg(v_bvrunmsg): #price,time,code,lastend,
  global udpCliSock
  for t_el0 in v_bvrunmsg:
    if(len(t_el0)==4):
      t_code=str(t_el0[2])[1:7]
      t_pv='%d'%(int((t_el0[0]*(commonlib.winlimit+1))*100)+1)
      while(len(t_pv)<3):
        t_pv='0'+t_pv
      t_price=str(t_el0[0])+'/'+t_pv[0:len(t_pv)-2]+'.'+t_pv[len(t_pv)-2:len(t_pv)]
      t_volume=str(int(commonlib.maxeverybuy//(t_el0[0]*100))*100)
      t_successflag='0'
      t_array=[t_code,t_price,t_volume,t_successflag]  #code,price,volume,successflag,
      commonlib.bvmsg.append(t_array)
      t_addr=(commonlib.serverip,commonlib.serverport)
      t_bvmsg=commonlib.array2string(t_array,',') #code,price,volume,successflag, 
      t_bvmsg='['+t_bvmsg+']'
      while(True):
        try:
          udpCliSock.sendto(t_bvmsg.encode(),t_addr) 
          #commonlib.msg(t_bvmsg,True,True)
          break
        except:
          closeudpCliSock()
          initudpCliSock()
          #print(str(udpCliSock))
  return(0)

def recvbvrunmsg(): #code,price,volume,successflag,
  global udpCliSock
  try:
    t_data,t_addr=udpCliSock.recvfrom(65535)
    print(t_data.decode())
    print(t_addr)
  except:
    pass
  return(0)
  
def bvrunmsg(v_bvrunmsg): #price,time,code,lastend,
  global bvrunmsg
  sendbvrunmsg(v_bvrunmsg)  #price,time,code,lastend,
  return(0)
    
if(__name__=='__main__'):
  commonlib.getcfg()
  initudpCliSock()
  t_a=[[19.07, 112424, 1600055, 19.37],[1.07, 112424, 1600099, 9.37]]
  while(True):
    recvbvrunmsg()
    sendbvrunmsg(t_a)
    time.sleep(1)
  closeudpCliSock()


