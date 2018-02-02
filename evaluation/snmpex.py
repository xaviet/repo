#! /usr/bin/python3
# coding=utf-8
'''
  name = snmpex.py
'''


from pysnmp.hlapi import *


def test():
  '''
    'SNMPv2-MIB', 'sysDescr', 0
    1.3
    IF-MIB
    1.3.6.1.4.1 = iso.org.dod.internet.private.enterprises
    1.3.6.1.4.1.2011.2.23.65 
    1.3.6.1.2.1.25.3.3
    1.3.6.1.4.1.25506.2.6.1.1.1.1.6 h3c cpu usage
  '''
  #fd = open('sunos', 'wt')
  for (errorIndication, errorStatus, errorIndex, varBinds) in nextCmd(SnmpEngine(), CommunityData('xa', mpModel=0), UdpTransportTarget(('192.168.1.1', 161)), ContextData(), ObjectType(ObjectIdentity('1.3.6.1.4.1.25506.2.6.1.1.1.1.6'))):
    if errorIndication:
      print(errorIndication)
      break
    elif errorStatus:
      print('%s at %s' % (errorStatus.prettyPrint(), errorIndex and varBinds[int(errorIndex) - 1][0] or '?'))
      break
    else:
      for varBind in varBinds:
        line = ' = '.join([x.prettyPrint() for x in varBind])
        if(line.find('IF-MIB') != -12):
          print(line)
        #fd.write(line+'\n')
  #fd.close()


def main():
  test()
  

if(__name__ == '__main__'):
  main()

