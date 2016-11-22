#!/usr/bin/env python
import serial
import sys
import time

POLYNOMIAL = 0xE7
TOPBIT     = 0x80
CRC        = [ 0 for i in range(256) ]

START_BYTE = 0xFA
SIZE_BASIS = 1024

PROG_COMP  = bytearray([0x8B,0xEF,0xF2,0x1F])

INFO_SIZE  = 2
DATA_LOC   = 2

INFO_DATA_SIZE = 1

DATA_SIZE  = 16
MSG_TYPE   = {   'DATA'     : 0x00,
                 'SIZE'     : 0x01,
                 'SECTOR'   : 0x02,
                 'CHECKSUM' : 0x03,
                 'READY'    : 0x04,
                 'START'    : 0x05,
                 'END'      : 0x06,
                 'ERROR'    : 0x07 }

# Creates and sends packets from the given data bytearray
def sendData ( ser, data ):
   for indx in range( 0, len(data), DATA_SIZE ):
      size = DATA_SIZE

      if( len(data) - indx < DATA_SIZE ) :
         size = len(data) - indx

      packet = genPacket ( 'DATA', INFO_SIZE + size )

      for dataIndx in range( indx, indx + size):
         packet[DATA_LOC + (dataIndx - indx)] = data[dataIndx]

      sendPacket ( packet )

def sendInfoMessage ( ser, msgType, data ):
   packet = genPacket ( msgType, INFO_SIZE + INFO_DATA_SIZE )
   packet [DATA_LOC] = (data & 0xFF)

   sendPacket ( packet )

def sendInfoMessage ( ser, msgType ):
   packet = genPacket ( msgType, INFO_SIZE )

   sendPacket ( packet )

def readFile ( fileName ):
   page = open(sys.argv[1], 'r')

   data = bytearray(page.read())
   page.close()
   return data

"""
Packet Helper Methods
   -- genPacket
   -- sendPacket
"""
def genPacket ( msgType, msgLength, dataSize ):
   packet = bytearray( [ 0 for i in range(msgLength) ] )
   packet[0] = START_BYTE
   packet[1] = (MSG_TYPE[msgType] & 0x07) << 7 | ((msgLength - INFO_SIZE) & 0x15)

   return packet

def sendPacket ( ser, packet ):
   packet = addCRC( packet )
   ser.write( packet )

def readPacket ( ser ):
   resp = bytearray(ser.read(INFO_SIZE))
   dataSize = resp[DATA_LOC]
   return bytearray(ser.read(dataSize))


def addCRC ( packet ):
   global CRC

   for byte in packet:
      data = byte ^ remainder
      remainder = CRC[byte] ^ remainder

   packet.add(remainder)

   return packet

def genLookupCRC ():
   global CRC

   remainder = 0
   for byte in range(0, 256):
      remainder = byte 

      for bit in range(0, 8):
         if(remainder & TOPBIT):
            remainder = (remainder << 1) ^ POLYNOMIAL
         else:
            remainder = remainder << 1

      CRC[byte] = remainder

# START MAIN
if(len(sys.argv) != 3):
   print "Usage: toBoot filename portName"
   sys.exit()

genLookupCRC()
print ( CRC )

# Setup Serial Connection and Save Data from File
ser = serial.Serial(sys.argv[2], 115200, timeout=0)
data = readFile(argv[1])


sendInfoMessage( ser, 'START')
sendInfoMessage( ser, 'SIZE', int(len(data) / SIZE_BASIS) )
sendInfoMessage( ser, 'SECTOR', START_SECTOR )
sendData(ser, data)

#sendInfoMessage( ser, 'CHECKSUM', checksum )
sendInfoMessage( ser, 'STOP')
# END MAIN



#try:
#except IOError:
#   print "Usage: toBoot filename portName"

#data = bytearray(serial.read(data_len))
# Clear the buffer

# ser.flush()
#ser.write(packet)

"""
# Make last byte 32 bits if not
if(len(data) % 4 != 0):
   for i in range(0, (4 - len(data) % 4)):
      data.append(0)

toWrite = [data[i:i + 4] for i in range(0, len(data), 4)]

ser.flush()

for i in range(0, len(toWrite)):
   ser.write(toWrite[i])
   time.sleep(0.01)
ser.write(PROG_COMP)
"""
