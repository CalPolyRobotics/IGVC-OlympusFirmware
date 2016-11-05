#!/usr/bin/python

import serial
import time

packet = bytearray([0xF0,0x5A,0,0,0,8,0,0])

ser = serial.Serial('/dev/cu.usbmodem14141', 115200, timeout=0)

ser.write(packet)

#time.sleep(3)

#print ser.read(1024)

ser.close()


