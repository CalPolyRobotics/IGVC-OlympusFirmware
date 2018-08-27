#!/usr/bin/env python3

"""
Boot

Bootloader application for STM32F2xx
"""

import sys
import os
import math
import argparse

from BootError import BootError
from BootSerial import BootSerial, USB_BUFF_SIZE, NUM_CHUNKS_32K, DEVICE_KEY_LUT

def openBinary(binName):
    """Opens binName in byte read mode and returns the file pointer and size of the file"""

    statinfo = os.stat(binName)
    return open(binName, 'rb'), statinfo.st_size

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-m', '--mcu', nargs='+', required=True, choices=DEVICE_KEY_LUT.keys(),
                        help='MCUs to be flashed in the order of their corresponding binary files')
    parser.add_argument('-f', '--file', nargs='+', required=True,
                        help='Path to binary programs being flashed')
    args = parser.parse_args()

    if len(args.mcu) != len(args.file):
        print('Number of MCUs must match number of files')
        sys.exit(1)

    # Make sure olympus is always the last bootloader run
    if len(args.mcu) > 1 and args.mcu[-1] != 'olympus':
        print('Olympus must be the last bootloader run')
        sys.exit(1)

    serComm = BootSerial('/dev/igvc_comm')

    print('Restarting Device...')
    serComm.resetDevice()
    serComm.close()

    serBoot = BootSerial('/dev/igvc_boot')

    for i, mcu in enumerate(args.mcu):
        fp, size = openBinary(args.file[i])
        print('Application size for ' + mcu + ': ' + str(size))

        print('Erasing Flash...')
        resp = serBoot.writeHeader(mcu, size)
        if resp != BootError.NO_ERR:
            print(resp)
            continue


        print('Writing Flash...')
        resp = serBoot.initData()
        if resp != BootError.NO_ERR:
            print(resp)
            continue

        # Write in CHUNK_SIZE Word Chunks
        for i in range(0, math.ceil(size/USB_BUFF_SIZE)):
            resp = serBoot.writeData(fp.read(USB_BUFF_SIZE))
            if resp != BootError.NO_ERR:
                print(resp)
                break

            if i != 0 and i % NUM_CHUNKS_32K == 0:
                print("Data 32K Written")

        if resp != BootError.NO_ERR:
            continue

        print('Checking Application Checksum...')
        resp = serBoot.writeChecksum()
        if resp != BootError.NO_ERR:
            print(resp)
            continue

        serBoot.close()
        print('Terry Flaps Installed')
