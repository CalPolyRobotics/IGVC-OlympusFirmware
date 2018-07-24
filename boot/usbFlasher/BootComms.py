#!/usr/bin/env python3

"""
BootComms

Python script to flash the microcontroller
"""

import os
import sys
import time
import math
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
    print('usage: BootCooms.py filename')
    exit(1)
else:
    binname = sys.argv[1]


# Attempt to connect to serial port
ser = None
count = 0
while count < 10:
    try:
        ser = serial.Serial(port, timeout=0)
        break
    except:
        time.sleep(1)
        count = count + 1

if ser is None:
    print('Failed to connect to device')
    sys.exit(1)

# Check for Ready Flag
"""
flag = True
while flag:
    data = ser.read()
    if(data == b'\x05'):
        print('Device Ready')
        flag = False
    print(data)
    """

statinfo = os.stat(binname)
header = buildHeader("OLYM", statinfo.st_size)

ser.write(header)
fp = open(binname, 'rb')

for i in range(0, math.ceil((statinfo.st_size)/64)):
    ser.write(fp.read(64))
    time.sleep(.0003)

fp.close()
print("Flash Complete")
