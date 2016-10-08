##!/usr/bin/env python3
## -*- coding: utf-8 -*-

import periphery
import time
import sys

com1=periphery.Serial('/dev/ttyS0',115200)
com1.write(b'Hello World!')
print('s0 waitting...')
time.sleep(0.1)
buf=com1.read(128,0.5)
print('read %d bytes: %s'%(len(buf),buf))
com1.close()

com1=periphery.Serial('/dev/ttyS2',115200)
com1.write(b'Hello World!')
print('s2 waitting...')
time.sleep(0.1)
buf=com1.read(128,0.5)
print('read %d bytes: %s'%(len(buf),buf))
com1.close()

com1=periphery.Serial('/dev/ttyS3',115200)
com1.write(b'Hello World!')
print('s3 waitting...')
time.sleep(0.1)
buf=com1.read(128,0.5)
print('read %d bytes: %s'%(len(buf),buf))
com1.close()
