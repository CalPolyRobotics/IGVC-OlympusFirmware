#!/usr/bin/env python3

"""
BootSerial

An extended Serial class for communicating with the IGVC bootloader
"""

import time
from serial import Serial, SerialException

USB_BUFF_SIZE = 64
NUM_CHUNKS_32K = int(32768/USB_BUFF_SIZE)

OK_BYTE_STRING = b'\x00'
FAIL_BYTE_STRING = b'\x01'

class BootSerial(Serial):
    """
    Extends the Serial class with functions specific to bootloader communication with the
    STM Microcontroller
    """

    def __init__(self, port, readTimeout=15, connectTimeout=2):
        self.data_count = 0
        count = 0

        while count < connectTimeout:
            try:
                Serial.__init__(self, port, timeout=readTimeout)
                break
            except SerialException:
                time.sleep(1)
                count += 1

        if count == connectTimeout:
            raise Exception('Failed to connect to Device')


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

        resp = self.read(1)
        if resp != OK_BYTE_STRING:
            raise Exception(messageStr + ' received response: 0x' + resp.hex())


    def writeSize(self, size):
        """Writes the size message for the given size"""
        self._writeMessage(2, [size], 'writeSize')


    def eraseFlash(self):
        """Initiates a flash erase"""
        self._writeMessage(3, [int(0x666C6170)], 'eraseFlash')


    def initData(self):
        """Initiates a data transfer"""
        self._writeMessage(0, [], 'initData')


    def writeData(self, data):
        """Writes data to the STM MCU"""
        self._writeMessage(data=data, messageStr='writeData')


    def writeChecksum(self, checksum):
        """Writes the checksum to check validity of application on MCU"""
        self._writeMessage(1, checksum, 'writeData')
