##!/usr/bin/env python3
## -*- coding: utf-8 -*-
## ex.py
## Pioevh 2015/10/21

import ann
import pylab

anninput=16

bpnn=ann.NN(anninput,8,1)  

darray=[]
df=open('sh','rt')
lastel=0.0
for el0 in df:
    if(lastel!=0.0):
        darray.append((float(el0[0:len(el0)-1])-lastel)*10/lastel)
    lastel=float(el0[0:len(el0)-1])
#print(darray)    
trainarray=[]
for i in range(0,len(darray)-anninput): 
    trainarray.append([darray[i:i+anninput],[darray[i+anninput]]])
    
bpnn.train(trainarray)

el0=(bpnn.update(darray[len(darray)-anninput:len(darray)]))[0]
darray.append(el0)
print(el0)
    
for i in range(0,60):
    predict=[]
    predict=darray[len(darray)-anninput+1:len(darray)]
    predict.append(el0)
    el0=(bpnn.update(predict))[0]
    print(el0)
    darray.append(el0)

#for i in range(0,len(darray)):
#    pylab.plot(i,darray[i],'r^')
pylab.plot(range(0,len(darray)),darray,'r')
   
for i in range(0,len(darray)-anninput):
    pylab.plot(i+anninput,bpnn.update(darray[i:i+anninput]),'bv')
pylab.plot(i+anninput+1,bpnn.update(darray[len(darray)-anninput:len(darray)]),'bv')

pylab.show()


