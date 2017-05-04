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

@spentTime  
def main():
  mbSend('ls -la\ncd /\nuptime')
  mbSend('uname -a\nps aux|grep python3\nls -la')
  t_rt=mbRx(getMbOuter(),getPp(),'server')
  for t_el0 in t_rt:
    for t_el1 in t_el0:
      print(t_el1)
  
if(__name__=='__main__'):
  main()

