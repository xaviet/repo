#! /usr/bin/python3
# coding=utf-8
'''
  name = snmpex.py
'''


from pysnmp.hlapi import *


def test():
  '''
    '1.3.6.1.6.3.1.1.5.2'
    'SNMPv2-MIB', 'sysDescr', 0
  '''
  #fd = open('h3cer3108g', 'wt')
  for (errorIndication, errorStatus, errorIndex, varBinds) in nextCmd(SnmpEngine(), CommunityData('xa', mpModel=0), UdpTransportTarget(('192.168.1.201', 161)), ContextData(), ObjectType(ObjectIdentity('1.2'))):
    if errorIndication:
      print(errorIndication)
      break
    elif errorStatus:
      print('%s at %s' % (errorStatus.prettyPrint(), errorIndex and varBinds[int(errorIndex) - 1][0] or '?'))
      break
    else:
      for varBind in varBinds:
        line = ' = '.join([x.prettyPrint() for x in varBind])
        print(line)
        #fd.write(line+'\n')
  #fd.close()


def main():
  test()
  

if(__name__ == '__main__'):
  main()

