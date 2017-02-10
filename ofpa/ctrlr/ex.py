#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""/usr/bin/ryu-manager
Usage example

mount -t nfs -o nolock 10.0.0.250:/opt/nfs /mnt/nfs

1. Join switches (use your favorite method):
$ mn --topo tree,depth=4 --switch ovs,protocols=OpenFlow13 --controller=remote,ip=10.0.0.250,port=6633 --mac
$ mn --topo topoex --custom /opt/topo/ex.py --mac --switch ovsk,protocols=OpenFlow13 --controller=remote,ip=10.0.0.250,port=6633 -x --arp --link tc

2. Run this application:
$ PYTHONPATH=. /usr/bin/ryu-manager --verbose --observe-links ./exctrl.py
$ PYTHONPATH=. /usr/bin/ryu-manager --observe-links ./ex.py

3. Access http://<ip address of ryu host>:8080 with your web browser.
#http://10.0.0.250:8080/ex/index.html
#http://10.0.0.250:8080/v1.0/topology/switches
"""

import os
import time
import json

from ryu import cfg
import multiprocessing

import logging
LOG = logging.getLogger('ex: ')

from webob.static import DirectoryApp
from webob import Response

from ryu.app.wsgi import ControllerBase, WSGIApplication, route
from ryu.base import app_manager
from ryu.app import simple_switch_13
#from ryu.lib import stplib

ex_instance_name = 'ex_api_app'
PATH = os.path.dirname(__file__)

# Serving static files
class exapp(simple_switch_13.SimpleSwitch13):
  _CONTEXTS = {'wsgi': WSGIApplication}
  #_CONTEXTS = {'wsgi': WSGIApplication,'stplib': stplib.Stp}
  
  def __init__(self, *args, **kwargs):
    super(exapp, self).__init__(*args, **kwargs)
    wsgi = kwargs['wsgi']
    wsgi.register(exctrl,{ex_instance_name : self})

class exctrl(ControllerBase):
  def __init__(self, req, link, data, **config):
    super(exctrl, self).__init__(req, link, data, **config)
    self.static_app = DirectoryApp('%s/html/' % PATH)
    self.exapp = data[ex_instance_name]

  @route('ex', '/ex/about', methods=['POST'])
  def exAbout(self, req, **kwargs):
    body = json.dumps('Xa@whnec by 20161111!')
    return(Response(content_type='application/json', body=body))
    
  @route('ex', '/ex/about', methods=['GET'])
  def exAbout(self, req, **kwargs):
    body = json.dumps('Xa@whnec by 20161111!')
    return(Response(content_type='application/json', body=body))
  
  @route('ex', '/ex/{filename:.*}')
  def static_handler(self, req, **kwargs):
    if kwargs['filename']:
      req.path_info = kwargs['filename']
    return(self.static_app(req))
    
if(__name__=='__main__'):
  pass

def floyd():
  time.sleep(8.0) 
  print('floyd starting')
  while(True):
    startTime=time.time()
    
    lldpDelayDict={}
    dpDelayDict={}
    linkDelayDict={}
    
    t_opt='select * from \'{0}\';'.format(cfg.lldpDelay)
    t_codegroup=cfg.optrecod(cfg.lldpDelay,t_opt)
    for el in t_codegroup:
      lldpDelayDict[(el[0],el[1])]=(el[2],el[3],el[4])
    #print(lldpDelayDict)
    
    t_opt='select * from \'{0}\';'.format(cfg.dpDelay)
    t_codegroup=cfg.optrecod(cfg.dpDelay,t_opt)
    for el in t_codegroup:
      dpDelayDict[el[0]]=el[1]
    #print(dpDelayDict)
    
    for key,value in lldpDelayDict.items():
      linkDelay=value[0]-dpDelayDict[key[0]]-dpDelayDict[key[1]]
      if(linkDelay>0):
        linkDelayDict[key]=(linkDelay,value[1],value[2])
      else:
        linkDelayDict[key]=value
        
    linkCount=0
    nm=[]
    pm=[]
    for i in range(16):
      el=[]
      el0=[]
      for j in range(16):
        delay=linkDelayDict[(i+1,j+1)][0]
        el.append(delay)
        destport=-1
        if(i==j):
          destport=0
        if(delay>0):
          linkCount+=1
          destport=linkDelayDict[(i+1,j+1)][1]
        el0.append(destport)
      nm.append(el)
      pm.append(el0)
      
    print('*'*128)  
    for el in nm:
      print(el)
    for el in pm:
      print(el)
    print('*'*128)  
    
    t_optA=[]
    nodeNum=16
    
    #floyd算法
    for k in range(nodeNum):
      for i in range(nodeNum):
        for j in range(nodeNum):
          #if((nm[i][k]!=-1) and (nm[k][j]!=-1) and (nm[i][j]==-1)): 
          if((nm[i][k]!=-1) and (nm[k][j]!=-1) and ((nm[i][k] + nm[k][j] < nm[i][j]) or (nm[i][j]==-1))): 
            #print(nm[i][k],nm[k][j],nm[i][j])
            nm[i][j]=nm[i][k]+nm[k][j]
            #print(nm[i][k],nm[k][j],nm[i][j]);
            pm[i][j]=pm[i][k]

    for i in range(nodeNum):
      for j in range(nodeNum):      
        t_optA.append('update \'{0}\' set egressport={1} where srcedp={2} and destdp={3};'.format(cfg.rttbl,pm[i][j],i+1,j+1))
    cfg.optrecodbatch(cfg.rttbl,t_optA)
            
    for el in nm:
      print(el)
    for el in pm:
      print(el)            
        
    print('Spent %f seconds'%(time.time()-startTime))
    time.sleep(120.0) 
  pass
  
app_manager.require_app('ryu.app.rest_topology')
app_manager.require_app('ryu.app.ws_topology')
app_manager.require_app('ryu.app.ofctl_rest')

pcs=multiprocessing.Process(target=floyd)
pcs.start()
