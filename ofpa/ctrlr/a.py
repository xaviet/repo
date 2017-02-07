#!/usr/bin/env python3
# -*- coding: utf-8 -*-

def readMatrix():
  fd=open('nbrmatrix','r')
  return(eval(fd.read()))

def main():
  nbrMatrix=readMatrix()
  print(nbrMatrix)
  
if(__name__=='__main__'):
  main()
