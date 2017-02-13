#!/usr/bin/env python3
# -*- coding: utf-8 -*-

def readMatrix():
  fd=open('nbrmatrix','r')
  return(eval(fd.read()))

def dispMatrix(nbrDelay,comment):
  str1=''
  switches=16
  for i in range(1,switches+1):
    str1+='%6d '%(i)
  print(' NdA | NdZ%8s%s'%(comment,str1))
  print('-'*129)
  for i in range(switches):
    str1=' %2d  |            '%(i+1)
    for j in range(switches):
      str1+='%6d '%(nbrDelay[i][j])
    print(str1)

def aveg(a):
  p=8
  x=0
  minx=999999
  maxx=0
  for el0 in a:
    for el1 in el0:
      if(el1==0):
        continue
      minx=el1 if(el1<minx) else minx
      maxx=el1 if(el1>maxx) else maxx
      x+=el1
  return(minx+p,int(x/240)+p,maxx+p)

def main():
  nbrMatrix=readMatrix()
  nbrDelay=nbrMatrix[0:16]
  nbrNexthop=nbrMatrix[16:32]
  floydDelay=nbrMatrix[32:48]
  floydNexthop=nbrMatrix[48:64]
  dispMatrix(nbrDelay,'(100us)')
  print()
  dispMatrix(nbrNexthop,'(Next)')
  print()
  dispMatrix(floydDelay,'(100us)')
  print()
  dispMatrix(floydNexthop,'(Next)')
  
  nodeNum=16
  for k in range(nodeNum):
    for i in range(nodeNum):
      for j in range(nodeNum):
        if((nbrDelay[i][k]!=-1) and (nbrDelay[k][j]!=-1) and (nbrDelay[i][j]==-1)): 
          nbrDelay[i][j]=nbrDelay[i][k]+nbrDelay[k][j]
          nbrNexthop[i][j]=nbrNexthop[i][k]
  print()
  dispMatrix(nbrDelay,'(100us)')
  print()
  dispMatrix(nbrNexthop,'(Next)')
  
  oc=0
  ec=0
  for i in range(nodeNum):
    for j in range(nodeNum):
      if(floydDelay[i][j]<nbrDelay[i][j]):
        oc+=1
        print(i,j,floydDelay[i][j],nbrDelay[i][j])
      if(floydDelay[i][j]==nbrDelay[i][j]):
        ec+=1
  print(oc,ec-16)
  print(aveg(floydDelay),aveg(nbrDelay))
  
if(__name__=='__main__'):
  main()
