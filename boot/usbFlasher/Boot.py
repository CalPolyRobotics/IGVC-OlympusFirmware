#!/usr/bin/env python3

"""
Boot

Bootloader application for STM32F2xx
"""

import sys
import os
import math

from BootSerial import BootSerial

CHUNK_SIZE = 1024

def openBinary(binName):
    """Opens binName in byte read mode and returns the file pointer and size of the file"""

    statinfo = os.stat(binName)
    return open(binName, 'rb'), statinfo.st_size


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('usage: Boot.py filename')
        sys.exit(1)

    filename = sys.argv[1]

    ser = BootSerial('/dev/ttyACM1')

    fp, size = openBinary(filename)

    print('Writing application size ' + str(size) + '...')
    ser.writeSize(size)

    print('Erasing flash...') 
    ser.eraseFlash()

    print('Writing flash...')
    ser.initData()

    # Write in 256 Word Chunks
    for i in range(0, math.ceil(size)/CHUNK_SIZE):
        ser.writeData(fp.read(CHUNK_SIZE))

    print('Checking application...')
    ser.writeChecksum([0xAAAA])

    print('Terry Flaps Installed')
