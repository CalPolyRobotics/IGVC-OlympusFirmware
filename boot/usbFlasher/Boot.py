#!/usr/bin/env python3

"""
Boot

Bootloader application for STM32F2xx
"""

import sys
import os
import math

from BootSerial import BootSerial, USB_BUFF_SIZE, NUM_CHUNKS_32K

def openBinary(binName):
    """Opens binName in byte read mode and returns the file pointer and size of the file"""

    statinfo = os.stat(binName)
    return open(binName, 'rb'), statinfo.st_size


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('usage: Boot.py filename')
        sys.exit(1)

    filename = sys.argv[1]

    ser = BootSerial('/dev/igvc_boot_1')

    fp, size = openBinary(filename)

    print('Writing application size ' + str(size) + '...')
    ser.writeSize(size)

    print('Erasing flash...') 
    ser.eraseFlash()

    print('Writing flash...')
    ser.initData()
    print('Write ready...')

    # Write in CHUNK_SIZE Word Chunks
    for i in range(0, math.ceil(size/USB_BUFF_SIZE)):
        ser.writeData(fp.read(USB_BUFF_SIZE))
        if i != 0 and i % NUM_CHUNKS_32K == 0:
            print("Data 32K Written")

    print('Checking application...')
    ser.writeChecksum([0xAAAA])

    print('Terry Flaps Installed')
