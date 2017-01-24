#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""/usr/bin/ryu-manager
Usage example

mount -t nfs -o nolock 10.0.0.250:/opt/nfs /mnt/nfs

1. Join switches (use your favorite method):
$ mn --topo tree,depth=4 --switch ovs,protocols=OpenFlow13 --controller=remote,ip=10.0.0.250,port=6633 --mac
$ mn --custom /opt/topo/ex.py --topo topoex --switch ovs,protocols=OpenFlow13 --controller=remote,ip=10.0.0.250,port=6633 --mac

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

import logging
LOG = logging.getLogger('ex: ')

from webob.static import DirectoryApp
from webob import Response

from ryu.app.wsgi import ControllerBase, WSGIApplication, route
from ryu.base import app_manager
from ryu.app import rest_router
from ryu.lib import stplib

ex_instance_name = 'ex_api_app'
PATH = os.path.dirname(__file__)

# Serving static files
class exapp(rest_router.RestRouterAPI):
  _CONTEXTS = {'wsgi': WSGIApplication,'stplib': stplib.Stp}

  def __init__(self, *args, **kwargs):
    super(exapp, self).__init__(*args, **kwargs)
    wsgi = kwargs['wsgi']
    wsgi.register(exctrl,{ex_instance_name : self})

class exctrl(rest_router.RouterController):
  def __init__(self, req, link, data, **config):
    super(exctrl, self).__init__(req, link, data, **config)
    self.static_app = DirectoryApp('%s/html/' % PATH)
    self.exapp = data[ex_instance_name]
    
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

app_manager.require_app('ryu.app.rest_topology')
app_manager.require_app('ryu.app.ws_topology')
app_manager.require_app('ryu.app.ofctl_rest')

