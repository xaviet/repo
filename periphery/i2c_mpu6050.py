##!/usr/bin/env python3
## -*- coding: utf-8 -*-

import periphery
import time
import sys
import ctypes

#pos=0x6b
#value=0x00
#i2c=periphery.I2C('/dev/i2c-0')
#msgs=[periphery.I2C.Message([pos,value]),periphery.I2C.Message([0x00],read=False)]
#i2c.transfer(0x68,msgs)
#msgs=[periphery.I2C.Message([pos]),periphery.I2C.Message([0x00],read=True)]
#i2c.transfer(0x68,msgs)
#print('0x%02x : 0x%02x'%(pos,msgs[1].data[0]))
#i2c.close()

i2caddr=0x68
ic2dev='/dev/i2c-0'
scaleacc=2
scalegyr=250
i2c=0

def i2cread(pos):
    global i2c
    msgs=[periphery.I2C.Message([pos]),periphery.I2C.Message([0x00],read=True)]
    i2c.transfer(i2caddr,msgs)
    return(msgs[1].data[0])

def i2cwrite(pos,value):
    global i2c
    msgs=[periphery.I2C.Message([pos,value]),periphery.I2C.Message([0x00],read=False)]
    i2c.transfer(i2caddr,msgs)
    return(0)

def uint8toint16(int8_h,int8_l):
    return(ctypes.c_int16((int8_h<<8)+int8_l).value)

def getvalue(value,scale):
    return((value*scale)/32767)

def setscale(s_acc,s_gyr):
    i2cwrite(0x1b,s_gyr<<3)
    i2cwrite(0x1c,s_acc<<3)
    return(0)

def getscale():
    global scaleacc
    global scalegyr
    scaleacclist=[2,4,8,16]
    scalegyrlist=[250,500,1000,2000]
    scaleacc=scaleacclist[(i2cread(0x1c)>>3)%4]
    scalegyr=scalegyrlist[(i2cread(0x1b)>>3)%4]
    print('Accelerometer scale: \t%d g'%scaleacc)
    print('Gyroscope scale: \t%d degrees/s'%scalegyr)
    return(0)

def samplerate(sr=0):
    '''Set or get sample rate [32,8000].defaule sample rate is 8000Hz.'''
    if((sr<32)or(sr>8000)):
        print('sample out of range!')
        sr=0
    if(sr!=0):
        i2cwrite(0x19,8000//(sr%8000)-1)
    return(i2cread(0x19))

def test():
    global i2c
    i2c=periphery.I2C(ic2dev)
    i2cwrite(0x6b,0x00)
    print('PM1 is [0x%02x].'%i2cread(0x6b))
    print('I2C address is [0x%02x].'%i2cread(0x75))
    setscale(3,3)
    getscale()
    print('Sample rate: \t'+str(8000//(1+samplerate(50)))+'Hz')
    #sys.stdout.write('x_a_h\tx_a_l\ty_a_h\ty_a_l\tz_a_h\tz_a_l\tt_h\tt_l\tx_g_h\tx_g_l\ty_g_h\ty_g_l\tz_g_h\tz_g_l\n')
    sys.stdout.write('acc_x\tacc_y\tacc_z\ttemp\tgyr_x\tgyr_y\tgyr_z\n')
    while(True):
        a=[]
        for i in range(0x3b,0x49):
            #sys.stdout.write(str(i2cread(i))+'\t')
            #time.sleep(0.001)
            a.append(i2cread(i))
        #print('\n')
        #print(a)
        accx=getvalue(uint8toint16(a[0],a[1]),scaleacc)
        accy=getvalue(uint8toint16(a[2],a[3]),scaleacc)
        accz=getvalue(uint8toint16(a[4],a[5]),scaleacc)
        temp=(uint8toint16(a[6],a[7])/340)+36.53
        gyrx=getvalue(uint8toint16(a[8],a[9]),scalegyr)
        gyry=getvalue(uint8toint16(a[10],a[11]),scalegyr)
        gyrz=getvalue(uint8toint16(a[12],a[13]),scalegyr)
        sys.stdout.write('%04.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f'%(accx,accy,accz,temp,gyrz,gyry,gyrz))
        sys.stdout.flush()
        time.sleep(0.5)
        #print('\n')
        for i in range(0x3b,0x49):
            sys.stdout.write('\r')
            #sys.stdout.write('\b'*56)
            sys.stdout.write(' '*128)
            sys.stdout.write('\r')
            pass
        sys.stdout.flush()
    i2c.close()
    return(0)

if(__name__=="__main__"):
    test()
