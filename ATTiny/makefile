
F_CPU = 8000000

all:
	avr-gcc -mmcu=attiny84 -DF_CPU=$(F_CPU) -O2 -o test.elf main.c usi.c
	avr-objcopy -O ihex test.elf test.hex
	avr-size test.elf

clean:
	rm -rf *.elf *.o *.hex

d:
	#may need to change /dev/tty to correct port on your computer
	#avrdude -p attiny84 -c avrispmkii -U flash:w:test.hex 
	avrdude -p t84 -c avrisp -P /dev/cu.usbmodem1411 -U flash:w:test.hex -C /usr/local/share/avr/etc/avrdude.conf -b 19200

