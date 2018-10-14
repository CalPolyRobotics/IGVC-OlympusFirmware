#!/bin/bash

../../usbFlasher/BootComms.py clearLights.bin
st-flash read temp.bin 0x8020000 0x222C0
hexdump temp.bin > tempdump
hexdump clearLights.bin > pgrmdump
vimdiff tempdump pgrmdump
