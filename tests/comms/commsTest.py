#!/usr/bin/python

import serial
import time
import sys

packet = bytearray([0xF0,0x5A,0,0,0,8,0,0])

ser = serial.Serial(sys.argv[1], 115200, timeout=0)

ser.write(packet)

time.sleep(.5)

print ','.join(x.encode('hex') for x in ser.read(1024))

# ser.close()


