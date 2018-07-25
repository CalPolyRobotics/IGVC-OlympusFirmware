# Calculate Size
# Write Size Word
# Wait for OK

# Write Erase Flash
# Wait for OK

# for math.floor(size/(4*256))
#   write 256 words
#   Wait for OK

# Write Checksum Message
# Write Checksum
# Wait for OK

# Program is Flashed

# b'' - No Repsponse
# b'\x00' - OK
# b'\x01' - FAILED

from serial import Serial
import time

class BootSerial(Serial):
    """
    Extends the Serial class with functions specific to bootloader communication with the
    STM Microcontroller
    """

    def __init__(self, port, readTimeout=1, connectTimeout=5):
        count = 0

        while count < connectTimeout:
            try:
                Serial.__init__(self, port, timeout=readTimeout)
                break
            except:
                time.sleep(1)
                count += 1


    def _writeMessage(self, msgType=None, data=[], messageStr='writeMessage'):
        """Writes the msgType followed by the data, throws exception with messageStr if fails"""

        pkt = b''

        if msgType is not None:
            pkt += msgType.to_bytes(4, byteorder='little')

        for datum in data:
            pkt += datum.to_bytes(4, byteorder='little')

        self.write(bytearray(pkt))

        resp = self.read(1)
        if resp != b'\x00':
            raise Exception(messageStr + ' received response: 0x' + resp.hex())


    def writeSize(self, size):
        """Writes the size message for the given size"""
        self._writeMessage(2, [size], 'writeSize')


    def eraseFlash(self):
        """Initiates a flash erase"""
        self._writeMessage(3, [], 'eraseFlash')


    def initData(self):
        """Initiates a data transfer"""
        self._writeMessage(0, [], 'initData')


    def writeData(self, data):
        """Writes data to the STM MCU"""
        self._writeMessage(0, data, 'writeData')

    def writeChecksum(self, checksum):
        """Writes the checksum to check validity of application on MCU"""
        self._writeMessage(1, checksum, 'writeData')
