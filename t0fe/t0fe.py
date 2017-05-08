#!/usr/bin/env python3
# -*- coding: <encoding name> -*-

'''
  t0fe.py
  by Pioevh@163.com 2010505
'''

import random
import time
import sys,tty,termios
import copy
from matplotlib import pyplot
import numpy
import pylab

class element():
  
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
  opt={ 'a':[(0,4,8,12),1],
        'w':[(0,1,2,3),4],
        'd':[(3,7,11,15),-1],
        's':[(12,13,14,15),-4]}
  
  def __init__(self,a=None):
    self.el=[]
    self.zeroNumber=16
    self.maxValue=0
    if(a==None):
      random.choice(time.gmtime())
      for i in range(0,16):
        self.el.append(element())
  
  def init(self,a):
    zeroNumber=0
    for i in range(0,16):
      if(self.el[i]==0):
        zeroNumber+=1
    maxValue=self.total[1]
    for i in range(0,16):
      self.el[i]=a[i]
  
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
    for j in range(0,2):
      for i in range(2,-1,-1):
        if(line[i]!=0 and line[i]==line[i+1]):
          line[i]*=2
          self.maxValue=line[i] if(line[i]>self.maxValue) else self.maxValue
          line[i+1]=0
          self.zeroNumber+=1
          line=self.press(line)
    return(line)
  
  def calcLine(self,firstV,interval):
    line=[]
    for i in range(0,4):
      line.append(self.el[firstV+i*interval].v)
    line=self.calc(line)
    for i in range(0,4):
      self.el[firstV+i*interval].v=line[i]
      
  def move(self,direct):
    for line in self.opt[direct][0]:
      self.calcLine(line,self.opt[direct][1])
    if(self.zeroNumber>0):
      self.setElement(random.randint(1,self.zeroNumber),2)
    else:
      self.zeroNumber=-1
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
    return(2048 if(self.maxValue==2048) else 0 if(self.zeroNumber==-1) else 1)
  
  @property
  def total(self):
    sum=0
    maxV=0
    for i in range(0,16):
      sum+=self.el[i].v
      maxV=self.el[i].v if(self.el[i].v>maxV) else maxV
    return(sum,maxV)    
  
  @property
  def cS(self):
    return(self.el[0].v+self.el[3].v+self.el[12].v+self.el[15].v)
      
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

def autoDetect(a): 
  mA={0:['a',[0,0]],1:['s',[0,0]],2:['d',[0,0]],3:['w',[0,0]]}
  oL=[]
  maxV=0
  totalV=0
  connerSum=0
  zeroN=0
  for i in range(0,4):
    mockG=panel()
    mockG.init(copy.deepcopy(a))
    totalV,maxV=mockG.total
    mockG.move(mA[i][0])
    mA[i][1][0],mA[i][1][1]=mockG.total
    if(mA[i][1][1]>maxV):
      oL=[]
      oL.append(mA[i][0])
      totalV,maxV=mA[i][1][0],mA[i][1][1]
    elif(mockG.zeroNumber>zeroN):
      oL=[]
      oL.append(mA[i][0])
      zeroN=mockG.zeroNumber
      #maxV=mA[i][1][1] if(mA[i][1][1]>maxV) else maxV
      #totalV=mA[i][1][1] if(mA[i][1][0]>totalV) else totalV
      #connerSum=mockG.cS if(mockG.cS>connerSum) else connerSum
    elif(mockG.zeroNumber==zeroN):
      oL.append(mA[i][0])
    if(oL==[]):
      ol.append(mA[randint(0,3)][0])
  return(oL[random.randint(0,len(oL)-1)])
      
def t0fe(auto=False):
  gm=panel()
  gm.creatPnl()
  while(True):
    pass
    #print(gm)
    #gm.getch()
    ch=autoDetect(copy.deepcopy(gm.el)) if(auto) else gm.getch()
    if(ch!=''):
      if(ch=='t'):
        break
      if(ch in ('a','s','d','w')):
        rt=gm.move(ch)
        if(rt!=1):
          #print(gm)
          #print('win max=%d total=%s'%(gm.maxValue,str(gm.total),) if(rt==2048) else 'lose max=%d total=%s'%(gm.maxValue,str(gm.total),))
          break
    else:
      time.sleep(1)
      continue
  return(gm.total)

def main(auto=False):
  x=[]
  y=[]
  z=[]
  for i in range(0,256):
    print(i)
    a=t0fe(auto)
    x.append(i)
    y.append(a[0])
    z.append(a[1])
  pyplot.figure(figsize=(8,4))
  pyplot.plot(x,y,label='$sum$',color='red',linewidth=2)
  pyplot.plot(x,z,'b--',label='$max$')
  pyplot.xlabel('x')
  pyplot.ylabel('y')
  pyplot.title('t0fe')
  pyplot.ylim(0,2048)
  pyplot.legend()
  pyplot.show()
  
if(__name__==('__main__')):
  pass
  main(True)

