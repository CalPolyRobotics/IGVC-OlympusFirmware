
F_CPU = 1000000

all:
	avr-gcc -mmcu=attiny84 -DF_CPU=$(F_CPU) -O2 -o test.elf main.c
	avr-objcopy -O ihex test.elf test.hex
	avr-size test.elf

clean:
	rm -rf *.elf *.o *.hex

d:
	#may need to change /dev/tty to correct port on your computer
	avrdude -p attiny84 -P /dev/cu.usbmodem00079671 -c avrisp2 -u -U flash:w:test.hex 

