from ryu.ofproto.ofproto_v1_3_parser import OFPPort
import copy

class PortState(dict):
    def __init__(self):
        super(PortState, self).__init__()

    def add(self, port_no, port):
        self[port_no] = port

    def remove(self, port_no):
        del self[port_no]

    def modify(self, port_no, port):
        self[port_no] = port

d1={5250946477030637671: {4294967294:OFPPort(port_no=135397631,hw_addr='48:df:3c:00:00:fe',name=b'ge.1.1',config=0,state=0,curr=2080,advertised=0,supported=0,peer=0,curr_speed=1000000,max_speed=0),4294967295:OFPPort(port_no=135397632,hw_addr='48:df:3c:00:00:fe',name=b'ge.1.1',config=0,state=0,curr=2080,advertised=0,supported=0,peer=0,curr_speed=1000000,max_speed=0)},5250946477030637670: {4294967296:OFPPort(port_no=135397633,hw_addr='48:df:3c:00:00:fe',name=b'ge.1.1',config=0,state=0,curr=2080,advertised=0,supported=0,peer=0,curr_speed=1000000,max_speed=0),4294967297:OFPPort(port_no=135397634,hw_addr='48:df:3c:00:00:fe',name=b'ge.1.1',config=0,state=0,curr=2080,advertised=0,supported=0,peer=0,curr_speed=1000000,max_speed=0)}}

print(d1.get(5250946477030637670,{}).values())

d2=copy.deepcopy(d1.get(5250946477030637670,{}))

for port in d1.get(5250946477030637670,{}).values():
  print('*************************************')
  print(port)
