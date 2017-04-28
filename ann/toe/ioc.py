## /usr/bin/python3
## -*- coding: utf-8 -*-
# g_    global value
# c_    class value
# v_    parameter value
# m_    member value
# t_    temporary value

'''
  ioc.py
  by Pioevh@163.com 2010427
'''

from commlib import *

def mbSend(v_content):
  mbTx(getMbOuter(),getMbInner(),'test ready','\nc/s server 0\n%s\nc/s server 1\n'%(v_content,),getPp())
  
def main():
  print(mbRx(getMbOuter(),getPp(),'server'))
  
if(__name__=='__main__'):
  main()
