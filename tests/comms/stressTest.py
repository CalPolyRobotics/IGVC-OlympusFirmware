#!/usr/bin/python

import serial
import time
import sys
import binascii

def sendPacket(ser):
    packet = bytearray([0xF0,0x5A,0,0,0,8,0x60,0x70])
    retPacket = bytearray([0xF0,0x5A,0xCC,1,0,8,0x60,0x70])
    ser.write(packet)
    recvPacket = ser.read(len(packet))
    if (len(packet) != len(recvPacket)):
        print binascii.hexlify(recvPacket)
        return 0
    elif (retPacket != recvPacket):
        print binascii.hexlify(recvPacket)
        return 0
    else:
        print binascii.hexlify(recvPacket)
        return 1

if (len(sys.argv) < 2):
    print "Not enough arguments"


ser = serial.Serial(sys.argv[1], 115200, timeout=1)

for i in range(1,100000):
    if (not sendPacket(ser)):
        print "Invalid Packet: " + str(i)
        break




ser.close()


