#!/usr/bin/env python3

"""
BootSerial

An extended Serial class for communicating with the IGVC bootloader
"""

import time
import sys
from serial import Serial, SerialException
from CRC8 import crc8
from BootError import BootError


DEVICE_KEY_LUT = {
    'olympus': 0x4F4C594D,
    'apollo': 0x41504F4C,
    'hermes': 0x4845524D,
    'hephaestus': 0x48455048,
    'zeus': 0x5A455553,
    'janus': 0x4A414E55
}

USB_BUFF_SIZE = 64
NUM_CHUNKS_32K = int(32768/USB_BUFF_SIZE)

FLASH_KEY = 0x666C6170

# Constants For Reset
START_BYTE_1 = 0xF0
START_BYTE_2 = 0x5A
RESET_MTYPE = 0x22
HEADER_SIZE = 6

# Reset Key in Little Endian
RESET_KEY_LE = [0x50, 0x41, 0x4C, 0x46]
RESET_DATA_SIZE = len(RESET_KEY_LE)

class BootSerial(Serial):
    """
    Extends the Serial class with functions specific to bootloader communication with the
    STM Microcontroller
    """

    def __init__(self, port, readTimeout=15, connectTimeout=5):
        self.checksum = 0
        count = 0

        while count < connectTimeout:
            try:
                Serial.__init__(self, port, timeout=readTimeout)
                break
            except SerialException:
                time.sleep(1)
                count += 1

        if count == connectTimeout:
            print('Failed to connect to Device')
            sys.exit(1)

    def _writeMessage(self, msgType=None, data=[], messageStr='writeMessage'):
        """Writes the msgType followed by the data, throws exception with messageStr if fails"""

        pkt = b''

        if msgType is not None:
            pkt += msgType.to_bytes(4, byteorder='little')

        # If data is already in bytes format, add it to the pkt
        if type(data) is bytes:
            pkt += data
        else:
            for datum in data:
                pkt += datum.to_bytes(4, byteorder='little')

        self.write(bytearray(pkt))

        bts = self.read(1)
        if len(bts) == 0:
            return BootError.ERR_WRITE_TIMEOUT

        resp = BootError(int.from_bytes(bts, byteorder='little'))
        return resp

    def writeData(self, data):
        """Writes data to the STM MCU and adds to cheksum"""
        for i in range(0, len(data), 4):
            self.checksum = (self.checksum + int.from_bytes(data[i:i+4], byteorder='little')) & 0xFFFFFFFF
        return self._writeMessage(data=data, messageStr='writeData')

    def initData(self):
        """Initiates a data transfer"""
        self.checksum = 0
        return self._writeMessage(0, [], 'initData')

    def writeChecksum(self):
        """Writes the checksum to check validity of application on MCU"""
        return self._writeMessage(1, [self.checksum], 'writeChecksum')

    def writeHeader(self, device, size):
        """Writes the header packet to initiate flash erase"""
        return self._writeMessage(2, [int(DEVICE_KEY_LUT[device]), int(size), int(FLASH_KEY)])

    def writeJump(self):
        """Writes the jump message to cause the master mcu to jump to the main application"""
        return self._writeMessage(3, [], 'writeJump')

    def resetDevice(self):
        """Resets the device from its running state"""
        reset_pkt = [START_BYTE_1, START_BYTE_2, RESET_MTYPE, 0x00, HEADER_SIZE + RESET_DATA_SIZE]
        reset_pkt.extend(RESET_KEY_LE)

        crc = crc8(reset_pkt)
        reset_pkt.append(crc)

        self.write(bytearray(reset_pkt))
