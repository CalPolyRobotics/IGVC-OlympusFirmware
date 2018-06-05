#!/usr/bin/env python
import serial
import sys
import time
import CRC

"""

Message Structure
Each  space is one byte unless specified otherwise

--------------------------------------------
|         |        |         |             |
|  START  |  MSG   |  DATA   |  DATA       |
|  BYTE   |  TYPE  |  LENGTH |  ANY -SIZE  |
|         |        |         |             |
--------------------------------------------

Note: For start mesage, there is one byte of data that represents the
      start sector

"""

POLYNOMIAL = 0xE7
TOPBIT     = 0x80


START_BYTE = 0xFA

START_BYTE_LOC  = 0
MSG_TYPE_LOC    = 1
DATA_LENGTH_LOC = 2
DATA_LOC        = 3

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
      packet = genPacket ( 'DATA', HEADER_SIZE + len(data) )

      indx = DATA_LOC + 1
      for dataIndx in range( indx, indx + len(data) ):
         packet[ dataIndx ] = data[ dataIndx - (DATA_LOC + 1) ]

      sendPacket ( packet )

def sendInfoMessage ( ser, msgType, data ):
   packet = genPacket ( msgType, INFO_SIZE + INFO_DATA_SIZE )
   packet [DATA_LOC] = (data & 0xFF)

   sendPacket ( packet )

def sendInfoMessage ( ser, msgType ):
   packet = genPacket ( msgType, HEADER_SIZE )

   sendPacket ( packet )

def sendStartMessage ( ser, startSector ):
   packet = genPacket ( msgType, HEADER_SIZE + 1 );

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
def genPacket ( msgType,  dataSize ):
   packet = bytearray( [ 0 for i in range( dataSize + HEADER_LENGTH) ] )
   packet[0] = START_BYTE
   packet[1] = MSG_TYPE[msgType]
   packet[2] = dataSize 

   return packet

def sendPacket ( ser, packet ):
   packet = addCRC( packet )
   ser.write( packet )

def readPacket ( ser ):
   resp = bytearray(ser.read(HEADER_SIZE))
   dataSize = resp[DATA_LENGTH_LOC]
   return bytearray(ser.read(dataSize))

def addCRC ( packet ):
   remainder = CRC.calcCRC8( packet )
   packet.add(remainder & 0xFF)

   return packet

# START MAIN
if(len(sys.argv) != 3):
   print "Usage: toBoot filename portName"
   sys.exit()


# Setup Serial Connection and Save Data from File
ser = serial.Serial(sys.argv[2], 115200, timeout=0)
data = readFile(sys.argv[1])

sendStartMessage( ser, 10)
#sendInfoMessage( ser, 'SIZE', int(len(data) / SIZE_BASIS) )
#sendInfoMessage( ser, 'SECTOR', START_SECTOR )
sendData(ser, data)

#sendInfoMessage( ser, 'CHECKSUM', checksum )
sendInfoMessage( ser, 'STOP')
# END MAIN
