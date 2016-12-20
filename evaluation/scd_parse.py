# /usr/bin/python3
# -*- coding: utf-8 -*-

from xml.etree.ElementTree import ElementTree
import time
import os
import timeit

def getnode(node,nodetag=None,attnm=None,attvl=None):
  for el0 in node.iter():

if(__name__=='__main__'):
  et=ElementTree()
  et.parse('/mnt/e/ts/scd/fengqiao220_08101419.scd')
  rt=et.getroot()
  print(rt.tag)
  for el0 in rt.iter():
    #print(el0.attrib)
    if('{http://www.iec.ch/61850/2003/SCL}ConnectedAP'==el0.tag):
      #print(el0.tag)
      for el1 in el0.iter():
        el1type=el1.get('type',default=None)
        #print(el1type)
        if(el1type=='APPID'):
            if(int(el1.text,16)==int('0x0041',16)):
              print(el0.attrib)