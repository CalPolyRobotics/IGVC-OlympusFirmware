BUILDDIR = build

SLIB = slave_lib

BUILD_TOOLS = $(SLIB)/buildTools
LIBRARIES = $(SLIB)/libraries
SYSTEM = $(SLIB)/system
STD_PERIPHS = $(SLIB)/peripherals
STD_DRIVERS = $(SLIB)/drivers

CMSIS = $(LIBRARIES)/CMSIS
HAL = $(LIBRARIES)/STM32F0xx_HAL_Driver

INCLUDES += -I$(HAL)/inc \
			-I$(CMSIS)/inc \
			-I$(SYSTEM) \
			-I$(STD_PERIPHS) \
			-I$(STD_DRIVERS)

ELF = $(BUILDDIR)/program.elf
HEX = $(BUILDDIR)/program.hex
BIN = $(BUILDDIR)/program.bin
MAP = $(BUILDDIR)/program.map

CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
AR = arm-none-eabi-ar

OBJCOPY = arm-none-eabi-objcopy

FLASH_SCRIPT = $(BUILD_TOOLS)/app.jlink

WARNINGS += -Wall -Werror

CFLAGS  += $(INCLUDES) \
			-Os -g \
			-mcpu=cortex-m0 -mthumb \
			-mfloat-abi=soft \
			-DSTM32F042x6 \
			$(WARNINGS)

PP += cpp
PPFLAGS += -P

.PHONY: all flash-jlink flash-stlink clean ctags

all: $(BIN) $(HEX)

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

$(BUILDDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

$(LDSCRIPT): $(LDSCRIPT_PRE)
	$(PP) $(PPFLAGS) $< -o $@

$(ELF): $(OBJECTS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -Wl,-Map=$(MAP) -o $@ $(OBJECTS) $(LDLIBS)
	arm-none-eabi-size $@

flash-jlink: $(HEX)
	cp $(HEX) $(BUILD_TOOLS)/program.hex
	JLinkExe -CommanderScript $(FLASH_SCRIPT)
	rm $(BUILD_TOOLS)/program.hex

flash-stlink: $(BIN)
	st-flash write $(BIN) $(START_ADDR) 

tags:
	ctags -R

clean:
	rm -rf build
	rm -f tags
