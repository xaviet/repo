#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import getopt
import math
import random

def martix(num):
  i=1
  j=i
  while(i<=math.sqrt(num)):
    if(((i*int(num/i))==num)and(i>j)):
      j=i
    i+=1
  return(j,int(num/j))

def main(switches,hosts):
  links=[]
  print('# switches:')
  for s in range(1,switches+1):
    print('    s%d=self.addSwitch(\'s%d\')'%(s,s))
  print('# hosts:')
  for h in range(1,hosts+1):
    print('    h%d=self.addHost(\'h%d\')'%(h,h))  for x0 in range(1,x+1):
    for y0 in range(1,y+1):
      node=((y0-1)*x)+x0
      if(x0<x):
        links.append([node,node+1])
        print('    self.addLink(s%d,s%d)'%(node,node+1))
      if(y0<y):
        links.append([node,node+x])
        print('    self.addLink(s%d,s%d)'%(node,node+x))
  print('# hosts to switches:')
  for l in range(1,(switches if(hosts>switches) else hosts)+1):
    print('    self.addLink(h%d,s%d)'%(l,l))
  x,y=martix(switches)
  node=0
  print('# switches to switches:')
  for x0 in range(1,x+1):
    for y0 in range(1,y+1):
      node=((y0-1)*x)+x0
      if(x0<x):
        links.append([node,node+1])
        print('    self.addLink(s%d,s%d)'%(node,node+1))
      if(y0<y):
        links.append([node,node+x])
        print('    self.addLink(s%d,s%d)'%(node,node+x))
  linkdelay=[]
  for el0 in links:
    linkdelay.append((el0[0],el0[1],256+int(random.random()*512)))
    linkdelay.append((el0[1],el0[0],256+int(random.random()*512)))
  #print(linkdelay)
  nm=[]
  for i in range(switches):
    el0=[]
    for j in range(switches):
      if(i==j):
        el0.append(0)
      else:
        for x,y,delay in linkdelay:
          if((i+1)==x and (j+1)==y):
            el0.append(delay)
            break
        else:
          el0.append(-1)
    nm.append(el0)
  #print(nm)
  #str1=''
  #for i in range(1,switches+1):
    #str1+='  %2d '%(i)
  #print(' NdA | NdZ (us) %s'%(str1))
  #print('-'*96)
  #for i in range(switches):
    #str=' %2d  |          '%(i+1)
    #for j in range(switches):
      #str+='%4d '%(nm[i][j])
    #print(str)
  #pass
  return(0)

if(__name__=='__main__'):
  try:
    opts,args=getopt.getopt(sys.argv[1:],'h:s:',['help'])
    for opt,val in opts:
      #print(opt,val)
      if(opt=='-h'):
        hosts=int(val)
      if(opt=='-s'):
        switches=int(val)
  except:
    print('-s ? -h ?')
  main(switches,hosts)
