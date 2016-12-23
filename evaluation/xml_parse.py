# /usr/bin/python3
# -*- coding: utf-8 -*-

from xml.etree.ElementTree import ElementTree
import time
import os

xmlfile='packetanalysis.xml'

def funtime(fun):
  def decoratorfun(para):
    starttime=time.time()
    rt=fun(para)
    print('funtime: '+str(int((time.time()-starttime)*1000000))+'us spent.')
    return(rt)
  return(decoratorfun)

def getnode(node,nodetag=None,attnm=None,attvl=None):
  rt=None
  for el0 in node.iter():
    if(el0!=node):
      print('Node: ',el0.tag,el0.attrib,el0.text)
      getnode(el0,nodetag,attnm,attvl)
  return(rt)
  
@funtime
def getnodebyft(node,nodetag=None,attnm=None,attvl=None):
  return(getnode(node,nodetag,attnm,attvl))
 
def openxml(xmlfile):
  et=ElementTree()
  et.parse(xmlfile)
  return(et.getroot())

if(__name__=='__main__'):
  et=ElementTree()
  et.parse(xmlfile)
  rt=et.getroot()
  print(getnodebyft(rt).attrib)
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