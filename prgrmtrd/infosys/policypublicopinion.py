#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  policypublicopinion.py
#  
#  Copyright 2017 pv <pv@pv.f25>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.
#  
#  


def main(args):
  
  minorValue=-32767
  poData={}
  fileData=open('publicopinion.csv','r').readlines()
  timedata=fileData[0].strip().split(',')[1:]
  for el0 in range(1,len(fileData)):
    lineData=fileData[el0].strip().split(',')
    poData[lineData[0]]=lineData[1:]

  tvData={}
  fileData=open('totalvalue.csv','r').readlines()
  for el0 in range(1,len(fileData)):
    lineData=fileData[el0].strip().split(',')
    tvData[lineData[0]]=lineData[1:]  
    
  tdData={}
  fileData=open('tradedata.csv','r').readlines()
  for el0 in range(1,len(fileData)):
    lineData=fileData[el0].strip().split(',')
    tdData[lineData[0]]=lineData[1:] 
   
  loopCount=sorted([len(timedata),len(poData['000001.SZ']),len(tvData['000001.SZ']),len(tdData['000001.SZ'])])[0]
  
  for el0 in range(1,loopCount):
    dayPoResult=[]
    dayPo={}
    for el1 in poData:
      if(len(poData[el1][el0].strip())>0 and len(poData[el1][el0-1].strip())>0):
        dayPo[el1]=float(poData[el1][el0])-float(poData[el1][el0-1])
      else:
        dayPo[el1]=minorValue
    dayPoSorted=sorted(dayPo.items(),key=lambda item:item[1],reverse=True)
    if(minorValue==dayPoSorted[0][1]):
      continue
      
    for el1 in range(0,len(dayPoSorted)):
      if(tdData.get(dayPoSorted[el1][0])==None):
        continue
      if(len(tdData[dayPoSorted[el1][0]][el0])==0):
        continue
      if(float(tdData[dayPoSorted[el1][0]][el0])>=20):
        continue
      dayPoResult.append(dayPoSorted[el1][0])
      
      if(float(tdData['000016.sh'][el0])>=float(tdData['000906.sh'][el0])):
        for el1 in dayPoResult:
          if(len(tvData[el1][el0])==0):
            dayPoResult.remove(el1)
          if(float(tvData[el1][el0])<500):
            dayPoResult.remove(el1)
            
      else:
        for el1 in dayPoResult:
          if(len(tvData[el1][el0])==0):
            dayPoResult.remove(el1)
          if(float(tvData[el1][el0])>=500):
            dayPoResult.remove(el1)
    
    policyResult={}
    weekMeans=0
    for el1 in dayPoResult:
      if(len(policyResult)==5):
        break
      policyResult[el1]=float(tdData[el1][el0+1])
      weekMeans+=policyResult[el1]
    
    print(timedata[el0],weekMeans/5,policyResult)
    
    #break
    
  print([len(timedata),len(poData['000001.SZ']),len(tvData['000001.SZ']),len(tdData['000001.SZ'])])
     
  return(0)

if __name__ == '__main__':
  import sys
  sys.exit(main(sys.argv))
