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
 
  @v.setter
  def v(self,setValue):
    self.value=setValue
  
class panel():
  el=[]
  zeroNumber=16
  maxValue=0
  opt={ 'a':[0,4,8,12,1],
        'w':[0,1,2,3,4],
        'd':[3,7,11,15,-1],
        's':[12,13,14,15,-4]}
  
  def __init__(self):
    random.choice(time.gmtime())
    for i in range(0,16):
      self.el.append(element())
  
  def setElement(self,pos,v=2):
    count=0
    for i in range(0,16):
      if(self.el[i]==0):
        count+=1
        if(count==pos):
          break
    self.el[i]=v
    self.zeroNumber-=1    
    
  def creatPnl(self):
    setElement(random.randint(1,self.zeroNumber),2)
    setElement(random.randint(1,self.zeroNumber),2)
        
  def move(self,direct):
    calc=self.opt[direct]
    
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
      #sys.stdout.write(ch) #echo
    finally:
      termios.tcsetattr(fd,termios.TCSADRAIN,old_settings)
    return ch

def main():
  gm=panel()
  gm.creatPnl()
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
