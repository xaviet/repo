#!/usr/bin/env python
# -*- coding: <encoding name> -*-

'''
  t0fe.py
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
    return(' ' if(self.value==0) else '%d'%(self.value,))
    
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
  opt={ 'a':[(0,4,8,12),1],
        'w':[(0,1,2,3),4],
        'd':[(3,7,11,15),-1],
        's':[(12,13,14,15),-4]}
  
  def __init__(self):
    random.choice(time.gmtime())
    for i in range(0,16):
      self.el.append(element())
  
  def setElement(self,pos,setValue=2):
    count=0
    i=0
    for i in range(0,16):
      if(self.el[i].v==0):
        count+=1
        if(count==pos):
          break
    self.el[i].v=setValue
    self.zeroNumber-=1 
    
  def creatPnl(self):
    self.setElement(random.randint(1,self.zeroNumber),2)
    self.setElement(random.randint(1,self.zeroNumber),2)
  
  def press(self,line):
    pos=0
    zero=0
    while(pos<4):
      if(line[pos]==0):
        zero=pos if(pos<zero) else zero
        pos+=1
        continue
      else:
        if(line[zero]==0):
          line[zero]=line[pos]
          line[pos]=0
        pos=zero+1
        zero=pos
    return(line)
  
  def calc(self,line):
    self.press(line)
    for i in range(2,-1,-1):
      if(line[i]!=0 and line[i]==line[i+1]):
        line[i]*=2
        self.maxValue=line[i] if(line[i]>self.maxValue) else self.maxValue
        line[i+1]=0
        self.zeroNumber+=1
        self.press(line)
    return(line)
  
  def calcLine(self,firstV,interval):
    line=[]
    for i in range(0,4):
      line.append(self.el[firstV+i*interval].v)
    self.calc(line)
    for i in range(0,4):
      self.el[firstV+i*interval].v=line[i]
      
  def move(self,direct):
    for line in self.opt[direct][0]:
      self.calcLine(line,self.opt[direct][1])
    self.setElement(random.randint(1,self.zeroNumber),2)
    return(self.rt)
  
  def __str__(self):
    hs=' ----'*4+'\n'
    string=''
    for i in range(0,4):
      line='%4s|%4s|%4s|%4s'%(self.el[i*4],self.el[i*4+1],self.el[i*4+2],self.el[i*4+3],)
      string+=hs+'|'+line+'|\n'
    string+=hs
    return(string)
  
  @property
  def rt(self):
    return(2048 if(self.maxValue==2048) else 0 if(self.zeroNumber==0) else 1)
  
  @property
  def total(self):
    sum=0
    for i in range(0,16):
      sum+=self.el[i].v
    return(sum)    
    
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
  while(True):
    pass
    print(gm)
    ch=gm.getch()
    if(ch!=''):
      if(ch=='t'):
        break
      if(ch in ('a','s','d','w')):
        rt=gm.move(ch)
        if(rt!=1):
          print(gm)
          print('win max=%d total=%d'%(gm.maxValue,gm.total,) if(rt==2048) else 'lose max=%d total=%d'%(gm.maxValue,gm.total,))
          break
    else:
      time.sleep(1)
      continue

if(__name__==('__main__')):
  main()
