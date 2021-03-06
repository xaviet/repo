# /usr/bin/python
# -*- coding: utf-8 -*-

from multiprocessing import Process,Pool,Manager
import socket
import time
import os

def run(v_conn,index):
  loop=0
  conn=v_conn
  recvdata=''
  clienttime=''
  delaytime=0
  while(1):
    recvdata=conn.recv(1024).decode('utf-8')
    if(len(recvdata)>=96):
      #print(recvdata)
      delaytime=int(time.time()*1000000)-int(recvdata[32:64])
      clienttime=recvdata[0:32]
      try:
        msg='%32s%32s%32s%768s'%(clienttime,str(int(time.time()*1000000)),str(delaytime),'0')
        conn.sendall(msg.encode('utf-8'))
      except:
        print('socket index %d error'%(index))
        conn.close();
        return(1)
        #if((index/256)==0):
          #loop+=1
          #if(loop==10):
            #print('socket %6d delaytime %d'%(index,delaytime));
            #loop=0
    time.sleep(0)
  conn.close()
  return(0)

if(__name__=='__main__'):
  ip_port=('10.255.255.1',60013)
  skt=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
  skt.bind(ip_port)
  skt.listen(8192)
  index=0
  print('tcp server running')
  while(1):
    conn,addr=skt.accept()
    ps=Process(target=run,args=(conn,index,))
    ps.start()
    #ps.join()
    index+=1
    conn.close()
    print('%d connect'%index)
  skt.close()
