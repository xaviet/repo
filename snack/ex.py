# /usr/bin/python3
# -*- coding: utf-8 -*-

import traceback
from snack import *
 
def window(screen):
  btn1=Button('B1')  #实例化一个按钮组件
  btn2=Button('B2')  #实例化一个按钮组件
  g=Grid(2,1)   #实例化一个两列，一行的网格
  g.setField(btn1,0,0)  #把组件填充到网格中
  g.setField(btn2,1,0)
  screen.gridWrappedWindow(g,'Windows')
  f=Form()  #实例化一个form
  f.add(g)    #把网格填充到form
  result=f.run()
  screen.popWindow()
  btn1.setCallback() 
  btn2.setCallback() 

def main():
  try:
    screen=SnackScreen()  #实例化一个snack界面
    window(screen)
  except:
    print(traceback.format_exc())
  finally:
    screen.finish()  #关闭snack界面
    return('')

if(__name__=='__main__'):
  main()