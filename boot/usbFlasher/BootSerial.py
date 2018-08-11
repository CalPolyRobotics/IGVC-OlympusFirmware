#!/usr/bin/env python3

"""
BootSerial

An extended Serial class for communicating with the IGVC bootloader
"""

import time
from serial import Serial, SerialException
from CRC8 import crc8


DEVICE_KEY_LUT = {
    'olympus': 0x4F4C594D,
    'hera': 0x48455241,
    'hephaestus': 0x48455048,
    'hermes': 0x4845524D,
    'janus': 0x4A414E55
}

USB_BUFF_SIZE = 64
NUM_CHUNKS_32K = int(32768/USB_BUFF_SIZE)

OK_BYTE_STRING = b'\x00'
FAIL_BYTE_STRING = b'\x01'

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


    def writeData(self, data):
        """Writes data to the STM MCU"""
        self._writeMessage(data=data, messageStr='writeData')


    def initData(self):
        """Initiates a data transfer"""
        self._writeMessage(0, [], 'initData')


    def writeChecksum(self, checksum):
        """Writes the checksum to check validity of application on MCU"""
        self._writeMessage(1, checksum, 'writeData')


    def writeHeader(self, device, size):
        """Writes the header packet to initiate flash erase"""
        self._writeMessage(2, [int(DEVICE_KEY_LUT[device]), int(size), int(FLASH_KEY)])


    def resetDevice(self):
        """Resets the device from its running state"""
        reset_pkt = [START_BYTE_1, START_BYTE_2, RESET_MTYPE, 0x00, HEADER_SIZE + RESET_DATA_SIZE]
        reset_pkt.extend(RESET_KEY_LE)

        crc = crc8(reset_pkt)
        reset_pkt.append(crc)

        self.write(bytearray(reset_pkt))