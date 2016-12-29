# /usr/bin/python3
# -*- coding: utf-8 -*-

import traceback
from snack import *
 
def window(screen):
  btn1=Button('B1')  #ʵ����һ����ť���
  btn2=Button('B2')  #ʵ����һ����ť���
  g=Grid(2,1)   #ʵ����һ�����У�һ�е�����
  g.setField(btn1,0,0)  #�������䵽������
  g.setField(btn2,1,0)
  screen.gridWrappedWindow(g,'Windows')
  f=Form()  #ʵ����һ��form
  f.add(g)    #��������䵽form
  result=f.run()
  screen.popWindow()
  btn1.setCallback() 
  btn2.setCallback() 

def main():
  try:
    screen=SnackScreen()  #ʵ����һ��snack����
    window(screen)
  except:
    print(traceback.format_exc())
  finally:
    screen.finish()  #�ر�snack����
    return('')

if(__name__=='__main__'):
  main()