#!/usr/bin/env python
# -*- coding: <encoding name> -*-

'''
  o0fe.py
  by Pioevh@163.com 2010505
'''

import random
import time
import sys,tty,termios

class element():
  value=0
  
  def __init__(self):
    self.value=0
    
  def __str__(self):
    return('    'if(self.value==0) else '%4d'%(self.value,))
    
  @property
  def v(self):
    return(self.value)
  
class panel():
  el=[]
  zeroNumber=16
  maxValue=0
  
  def __init__(self):
    for i in range(0,16):
      self.el.append(element())
  
  def move(self,direct):
    return(self.rt)
  
  def __str__(self):
    hs=' ----'*4+'\n'
    string=''
    for i in range(0,4):
      line='{}|{}|{}|{}'.format(self.el[i*4],self.el[i*4+1],self.el[i*4+2],self.el[i*4+3])
      string+=hs+'|'+line+'|\n'
    string+=hs
    return(string)
  
  @property
  def rt(self):
    return(2048 if(self.maxValue==2048) else 0 if(self.zeroNumber==0) else 1)

  def getch(self):
    ch=''
    fd=sys.stdin.fileno()
    old_settings=termios.tcgetattr(fd)
    try:
      tty.setraw(sys.stdin.fileno(),termios.TCSANOW)
      ch=sys.stdin.read(1)
      #sys.stdout.write(ch)
    finally:
      termios.tcsetattr(fd,termios.TCSADRAIN,old_settings)
    return ch

def main():
  gm=panel()
  print(gm)
  while(True):
    pass
    ch=gm.getch()
    if(ch!=''):
      if(ch=='t'):
        break
      rt=gm.move(ch)
      if(rt!=1):
        print('win' if(rt==2048) else 'lose')
        break
    else:
      time.sleep(1)
      continue
    print(gm)

if(__name__==('__main__')):
  main()
