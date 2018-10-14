"""
BootError

An extended enum for managing errors while bootloading
"""

from enum import Enum


class BootError(Enum):
    '''Enum class for describing errors during bootloading'''

    NO_ERR = 0x00
    ERR_DATA_WRITE = 0x01
    ERR_CHECKSUM = 0x02
    ERR_INVALID_HEADER = 0x03
    ERR_FLASH_ERASE = 0x04
    ERR_PRE_REBOOT_STATUS = 0x05
    ERR_REBOOT_START = 0x06
    ERR_REBOOT_TIMEOUT = 0x07
    ERR_REBOOT_STATUS = 0x08
    ERR_WRITE_TIMEOUT = 0x09

    def __str__(self):
        return 'Errror: ' + CERR_LUT[self]


CERR_LUT = {
    BootError.NO_ERR: 'Communication is OK',
    BootError.ERR_DATA_WRITE: 'Device timed out after writing data',
    BootError.ERR_CHECKSUM: 'Checksum ',
    BootError.ERR_INVALID_HEADER: 'Header info rejected by device',
    BootError.ERR_FLASH_ERASE: 'Device timed out on flash erase',
    BootError.ERR_PRE_REBOOT_STATUS: 'Check status failed on slave device before reset',
    BootError.ERR_REBOOT_START: 'Master device could not initiate a reset for the slave device',
    BootError.ERR_REBOOT_TIMEOUT: 'Master device timed out waiting for slave device reset',
    BootError.ERR_REBOOT_STATUS: 'Status check on slave device failed after reset',
    BootError.ERR_WRITE_TIMEOUT: 'USB write timed out waiting for response'
}
