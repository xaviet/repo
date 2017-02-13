from mininet.topo import Topo
from mininet.net import Mininet
from mininet.node import CPULimitedHost
from mininet.link import TCLink
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel
import sys
import getopt
import math
import random

class topoEx(Topo):
  '''Simple topology example.
       mn --topo topoex --custom /opt/topo/ex.py --mac --switch ovsk,protocols=OpenFlow13 --controller=remote,ip=10.0.0.250,port=6633 -x --arp --link tc
  '''

  def __init__(self):
    "Create custom topo."
    
    # Initialize topology
    Topo.__init__(self)
    
    switches=16
    hosts=16
    
    swarray=[]
    harray=[]
    #linkopts=dict(bw=512,delay='8ms',max_queue_size=512,loss=1)
    delayArray=[7.0, 2.6, 4.4, 8.1, 7.9, 1.4, 2.6, 1.8, 3.6, 9.3, 6.9, 5.9, 1.3, 1.5, 2.3, 1.5, 9.8, 6.5, 2.5, 7.3, 1.9, 9.0, 7.8, 2.0, 5.6, 3.9, 2.8, 5.1, 7.3, 9.9, 7.5, 1.9, 1.4, 4.8, 2.9, 2.8, 3.7, 2.6, 1.0, 8.8, 5.5, 2.1, 8.7, 1.5, 1.2, 3.3, 8.5, 9.0, 8.9, 9.0, 6.3, 7.4, 8.0, 6.9, 3.4, 8.5, 6.0, 5.2, 7.5, 5.1, 1.1, 8.7, 8.6, 2.3]
    linkSwitch=dict(delay='8ms')
    linkHost=dict(delay='0ms')
    
    for s in range(switches):
      swarray.append(self.addSwitch('s%d'%(s+1)))
    for h in range(hosts):
      harray.append(self.addHost('h%d'%(h+1)))
    for l in range((switches if(hosts>switches) else hosts)):
      self.addLink(node1=swarray[l],node2=harray[l],**linkHost)  
    num=switches
    i=1
    j=i
    while(i<=math.sqrt(num)):
      if(((i*int(num/i))==num)and(i>j)):
        j=i
      i+=1
    x,y=j,int(num/j)
    c=0
    for x0 in range(x):
      for y0 in range(y):
        node=((y0)*x)+x0
        if(x0<(x-1)):
          linkOpt=dict(delay='%fms'%(delayArray[c]))
          c+=1
          self.addLink(node1=swarray[node],node2=swarray[node+1],**linkOpt)
        if(y0<(y-1)):
          linkOpt=dict(delay='%fms'%(delayArray[c]))
          c+=1
          self.addLink(node1=swarray[node],node2=swarray[node+x],**linkOpt)

          # switches:
    #s1=self.addSwitch('s1')
# hosts:
    #h1=self.addHost('h1')
# hosts to switches:
    #self.addLink(h1,s1)
# switches to switches:
    #self.addLink(s1,s2)

topos={'topoex':(lambda:topoEx())}
