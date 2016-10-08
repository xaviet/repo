#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Usage example

1. Join switches (use your favorite method):
$ mn --custom /opt/topo/ex.py --topo tree,depth=3 --switch ovs,protocols=OpenFlow13 --controller=remote,ip=10.0.0.250,port=6633 --mac

2. Run this application:
$ PYTHONPATH=. /usr/bin/ryu-manager --verbose --observe-links ./exctrl.py

3. Access http://<ip address of ryu host>:8080 with your web browser.
#http://127.0.0.1:8080/exctrl/index.html  
"""

import os
import time
import json

import logging
LOG = logging.getLogger('exctrl: ')

from webob.static import DirectoryApp
from webob import Response

from ryu.app.wsgi import ControllerBase, WSGIApplication, route
from ryu.base import app_manager
from ryu.app import simple_switch_13

ex_instance_name = 'ex_api_app'
PATH = os.path.dirname(__file__)

# Serving static files
class exapp(simple_switch_13.SimpleSwitch13):
  _CONTEXTS = {'wsgi': WSGIApplication,}

  def __init__(self, *args, **kwargs):
    LOG.debug('*************************exapp-__init__')
    super(exapp, self).__init__(*args, **kwargs)
    wsgi = kwargs['wsgi']
    wsgi.register(exctrl,{ex_instance_name : self})

class exctrl(ControllerBase):
  def __init__(self, req, link, data, **config):
    LOG.debug('*************************exctrl-__init__')
    super(exctrl, self).__init__(req, link, data, **config)
    LOG.debug('*************************%s/html/' % PATH)
    self.static_app = DirectoryApp('%s/html/' % PATH)
    #self.exapp = data[ex_instance_name]

  @route('exctrl', '/exctrl/info', methods=['GET'])
  def info(self, req, **kwargs):
    LOG.debug('*************************exctrl-info')
    body = json.dumps([1,2,3,4,5,6,7,8])
    return(Response(content_type='application/json', body=body))
  
  @route('exctrl', '/exctrl/f22', methods=['GET'])
  def f22(self, req, **kwargs):
    LOG.debug('*************************exctrl-f22')
    body = json.dumps([1,2])
    return(Response(content_type='application/json', body=body))
  
  @route('exctrl', '/exctrl/{filename:.*}')
  def static_handler(self, req, **kwargs):
    LOG.debug('*************************exctrl-static_handler')
    if kwargs['filename']:
      req.path_info = kwargs['filename']
    return(self.static_app(req))
    
if(__name__=='__main__'):
  pass

app_manager.require_app('ryu.app.rest_topology')
app_manager.require_app('ryu.app.ws_topology')
app_manager.require_app('ryu.app.ofctl_rest')

