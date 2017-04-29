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
  mbTx(getMbOuter(),getMbInner(),'pioevh\'s operation','\nc/s client 0\n%s\nc/s client 1\n'%(v_content,),getPp())
  
def main():
  mbSend('ioc > ios')
  print(mbRx(getMbOuter(),getPp(),'server'))
  
if(__name__=='__main__'):
  main()

