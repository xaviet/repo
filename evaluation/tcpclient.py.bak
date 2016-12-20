# /usr/bin/python
# -*- coding: utf-8 -*-

from multiprocessing import Process,Pool,Manager
import socket
import time
import os

def run(index):
  ip_port=('172.28.72.213',60013)
  try:
    sfd=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    sfd.connect(ip_port)
  except:
    print('socket index %d error'%(index))
    pass  
    return(1)
  #print(os.popen('netstat -s|grep tcpCurrEstab').readlines())
  servertime='0'
  recvdata=''
  delaytime=0
  loop=0
  sdtime=0
  while(1):
    sfd.sendall('%32s%32s%32s%768s'%(str(int(time.time()*1000000)),servertime,'0','0'))
    while(1):
      recvdata=sfd.recv(1024)
      if(len(recvdata)>=96):
        #print(recvdata)
        delaytime=int(time.time()*1000000)-int(recvdata[0:32])
        sdtime=int(recvdata[64:96])
        servertime=recvdata[32:64]
        if((index%256)==0):
          loop+=1
          pass
          if(loop==256):
            print('socket %6d delaytime %16dus(client)/%16dus(server)'%(index,delaytime,sdtime-100000));
            loop=0
        break
      time.sleep(0)
    time.sleep(0.1) 
  sfd.close()
  return(0)
  
if(__name__=='__main__'):
  connnumber=2048
  starttime=time.time()
  pcslist=[]
  for index in range(connnumber):
    pcs=Process(target=run,args=(index,))
    pcs.start()
    pcslist.append(pcs)
    time.sleep(0.01) 
  #for el in pcslist:
    #pass
    #el.join()
  print('tcp %d connections in %d seconds'%(connnumber,time.time()-starttime))
  while(1):
    time.sleep(1) 
    pass