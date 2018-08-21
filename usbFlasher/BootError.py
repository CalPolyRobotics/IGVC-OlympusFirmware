"""
BootError

An extended enum for managing errors while bootloading
"""

from enum import Enum

class BootError(Enum):
    NO_ERR = 0x00
    ERR_DATA_WRITE = 0x01
    ERR_CHECKSUM = 0x02
    ERR_INVALID_HEADER = 0x03
    ERR_FLASH_ERASE = 0x04

    def __str__(self):
        return 'Errror: ' + CERR_LUT[self]

CERR_LUT = {
    BootError.NO_ERR: 'Communication is OK',
    BootError.ERR_DATA_WRITE: 'Device timed out after writing data',
    BootError.ERR_CHECKSUM: 'Checksum ',
    BootError.ERR_INVALID_HEADER: 'Header info rejected by device',
    BootError.ERR_FLASH_ERASE: 'Device timed out on flash erase'
}
