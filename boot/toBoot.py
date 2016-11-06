#!/usr/bin/env python
import serial
import sys
import time

progComplete = bytearray([0x8B,0xEF,0xF2,0x1F])

if(len(sys.argv) != 3):
   print "Usage: toBoot filename portName"
   sys.exit()

page = open(sys.argv[1], 'r')
ser = serial.Serial(sys.argv[2], 115200, timeout=0)

data = bytearray(page.read())
page.close()

# Make last byte 32 bits if not
if(len(data) % 4 != 0):
   for i in range(0, (4 - len(data) % 4)):
      data.append(0)

toWrite = [data[i:i + 4] for i in range(0, len(data), 4)]

print toWrite[0]

ser.flush()

ser.write(toWrite[0])
time.sleep(.01)

resp = bytearray(ser.readline())
while(len(resp) != 0):
   print(resp[0:-1])
   resp = ser.readline()

ser.write(progComplete)
time.sleep(.01)

resp = bytearray(ser.readline())
while(len(resp) != 0):
   print(resp[0:-1])
   resp = ser.readline()


"""
#try:
#except IOError:
#   print "Usage: toBoot filename portName"

#data = bytearray(serial.read(data_len))
# Clear the buffer

# ser.flush()
#ser.write(packet)
"""
