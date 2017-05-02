## /usr/bin/python3
## -*- coding: utf-8 -*-
# g_    global value
# c_    class value
# v_    parameter value
# m_    member value
# t_    temporary value

'''
  ios.py
  by Pio evh@163.com 2010427
'''

from commlib import *
import time
import os

def mbSend(v_content):
  mbTx(getMbInner(),getMbOuter(),'pioevh\'s operation','\nc/s server 0\n%s\nc/s server 1\n'%(v_content,),getPp())

def main():
  pass
  while(True):
    try:
      pass
      t_rx=[]
      t_rx=mbRx(getMbInner(),getPp(),'client')
    except Exception as e:  
      pass
      print(e)
    t_mbNumber=len(t_rx)
    msg(str(t_mbNumber))
    for t_el0 in range(0,t_mbNumber):
      t_rt=''
      for t_el1 in t_rx[t_el0]:
        t_cmd=os.popen(t_el1)
        t_rt=t_rt+'[# {0}]\n{1}\n'.format(t_el1,t_cmd.read())
      mbSend(t_rt)
    time.sleep(64)
    
if(__name__=='__main__'):
  main()
