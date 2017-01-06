# /usr/bin/python3
# -*- coding: utf-8 -*-

import time
import xml_parse 

def openxml(xmlfile):
  return(xml_parse.openxml(xmlfile))
  
def dens(xmltag):
  nsend=xmltag.find('}')
  return(xmltag if(nsend==-1) else xmltag[nsend+1:])
  
def main(xmlnode):
  rt=0
  return(rt)

if(__name__=='__main__'):
  pass
  xmlfile='/mnt/e/ts/scd/fengqiao220_08101419.scd'
  root=openxml(xmlfile)
  main(root)  