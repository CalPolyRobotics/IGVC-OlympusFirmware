BUILDDIR = build

SLIB = slave_lib

BUILD_TOOLS = $(SLIB)/buildTools
LIBRARIES = $(SLIB)/libraries
SYSTEM = $(SLIB)/system
STD_PERIPHS = $(SLIB)/peripherals
STD_DRIVERS = $(SLIB)/drivers

CMSIS = $(LIBRARIES)/CMSIS

DRIVERS = drivers
PERIPHS = peripherals

# This must come before all interrupt handlers to ensure -flto does not
# optimize out interrupt service routines
SOURCES += $(SYSTEM)/startup_stm32f042x6.s \
		   $(SYSTEM)/system_stm32f0xx.c

SOURCES += main.c

SOURCES += $(STD_DRIVERS)/commsLib.c

SOURCES += $(DRIVERS)/comms.c

SOURCES += $(STD_PERIPHS)/spi.c \
			$(STD_PERIPHS)/systemClock.c \

SOURCES += $(PERIPHS)/flash.c


OBJECTS = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(SOURCES))))

INCLUDES += -I$(CMSIS)/inc \
			-I$(DRIVERS) \
			-I$(SYSTEM) \
			-I$(STD_DRIVERS) \
			-I$(STD_PERIPHS) \
			-I$(PERIPHS) \
			-I.

ELF = $(BUILDDIR)/program.elf
HEX = $(BUILDDIR)/program.hex
BIN = $(BUILDDIR)/program.bin
MAP = $(BUILDDIR)/program.map

CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
AR = arm-none-eabi-ar
OBJCOPY = arm-none-eabi-objcopy

WARNINGS = -Wall -Werror

CFLAGS  = $(INCLUDES) \
	-Os -g \
   -mcpu=cortex-m0 -mthumb \
   -mfloat-abi=soft \
	-flto \
   -D STM32F042x6 \
   -D BOOTLOAD \
   -D TINY \
   $(WARNINGS)

PP = cpp
PPFLAGS = -P -D BOOTLOAD

LDSCRIPT_PRE = $(BUILD_TOOLS)/stm32f042_32k.ld
LDSCRIPT = $(BUILDDIR)/linker.ld
LDFLAGS += -T$(LDSCRIPT) \
			  --specs=nosys.specs \
			  -flto \
           -mthumb \
           -mcpu=cortex-m0 \
           -LbuildTools

LDLIBS += -lnosys

FLASH_SCRIPT = $(BUILD_TOOLS)/boot.jlink

.PHONY: all flash flash-dev clean tags

all: $(BIN)

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

$(LDSCRIPT): $(LDSCRIPT_PRE)
	$(PP) $(PPFLAGS) $< -o $@

$(ELF): $(OBJECTS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -Wl,-Map=$(MAP) -o $@ $(OBJECTS) $(LDLIBS)
	arm-none-eabi-size $@

$(BUILDDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

flash-k6: $(BIN)
	cp $(BIN) $(BUILD_TOOLS)/program.bin
	JLinkExe -Device STM32F042K6 -CommanderScript $(FLASH_SCRIPT)
	rm $(BUILD_TOOLS)/program.bin

flash-c6: $(BIN)
	cp $(BIN) $(BUILD_TOOLS)/program.bin
	JLinkExe -Device STM32F042C6 -CommanderScript $(FLASH_SCRIPT)
	rm $(BUILD_TOOLS)/program.bin

flash-stlink: $(BIN)
	st-flash write $(BIN) 0x8000000

tags:
	ctags -R

clean:
	rm -rf build
