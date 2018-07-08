#!/usr/bin/env python

"""
BootComms

Python script to flash the microcontroller
"""

import os
import sys
import serial

def buildHeader(key, size):
    """
    buildHeader
    """
    pkt = bytes(key, 'utf8')
    pkt += size.to_bytes(4, byteorder='little')
    return bytearray(pkt)

port = '/dev/ttyACM1'
binname = ''
if len(sys.argv) < 2:
    print 'usage: rosrun communication_ros BoardComms.py [ttyACM1]'
    exit(1)
else:
    binname = sys.argv[1]

statinfo = os.stat(binname)
header = buildHeader("OLYM", statinfo.st_size)

ser = serial.Serial(port, timeout=1)
while True:
    data = ser.read()
    if data != b'':
        print data

ser.write(header)
fp = open(binname, 'rb')

for i in range(0, int((statinfo.st_size)/128)):
    ser.write(fp.read(128))
    flag = False
    while not flag:
        data = ser.read()
        if data == '\x05':
            flag = True
        print data

fp.close()
print "Flash Complete"
