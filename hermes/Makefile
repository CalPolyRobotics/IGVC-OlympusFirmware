BUILDDIR = build

SLIB = slave_lib

BUILD_TOOLS = $(SLIB)/buildTools
LIBRARIES = $(SLIB)/libraries
SYSTEM = $(SLIB)/system
STD_PERIPHS = $(SLIB)/peripherals
STD_DRIVERS = $(SLIB)/drivers

CMSIS = $(LIBRARIES)/CMSIS
HAL = $(LIBRARIES)/STM32F0xx_HAL_Driver

BOOT_APP = ../usbFlasher/Boot.py

DRIVERS = drivers
PERIPHS = peripherals

SOURCES += main.c

SOURCES += $(STD_DRIVERS)/commsLib.c \
			$(STD_DRIVERS)/boot.c \

SOURCES += $(STD_PERIPHS)/spi.c \
			$(STD_PERIPHS)/systemClock.c

SOURCES += $(DRIVERS)/comms.c

SOURCES += $(HAL)/src/stm32f0xx_hal_rcc.c \
			$(HAL)/src/stm32f0xx_hal_rcc_ex.c \
			$(HAL)/src/stm32f0xx_hal_flash.c \
			$(HAL)/src/stm32f0xx_hal_flash_ex.c \
			$(HAL)/src/stm32f0xx_hal_gpio.c \
			$(HAL)/src/stm32f0xx_hal_cortex.c \
			$(HAL)/src/stm32f0xx_hal.c

SOURCES += $(SYSTEM)/startup_stm32f042x6.s \
		   $(SYSTEM)/stm32f0xx_hal_msp.c \
		   $(SYSTEM)/stm32f0xx_it.c \
		   $(SYSTEM)/system_stm32f0xx.c

OBJECTS = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(SOURCES))))

INCLUDES += -I$(HAL)/inc \
			-I$(CMSIS)/inc \
			-I$(DRIVERS) \
			-I$(PERIPHS) \
			-I$(SYSTEM) \
			-I$(STD_PERIPHS) \
			-I$(STD_DRIVERS) \
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

WARNINGS = -Wall -Werror -Wno-unused-function
	
CFLAGS  = $(INCLUDES) \
	-Os -g \
   -mcpu=cortex-m0 -mthumb \
   -mfloat-abi=soft \
   -D STM32F042x6 \
   $(WARNINGS)

PP = cpp
PPFLAGS = -P

LDSCRIPT_PRE = $(BUILD_TOOLS)/stm32f042_32k.ld
LDSCRIPT = $(BUILDDIR)/linker.ld
LDFLAGS += -T$(LDSCRIPT) \
           -mthumb \
           -mcpu=cortex-m0 \
           -LbuildTools

LDLIBS += -lc -lm -lnosys

FLASH_SCRIPT = $(BUILD_TOOLS)/app.jlink

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

flash: $(BIN)
	$(BOOT_APP) -m hermes -f $(BIN)

flash-stlink: $(BIN)
	st-flash write $(BIN) 0x8001400

flash-jlink: $(BIN)
	cp $(BIN) $(BUILD_TOOLS)/program.bin
	JLinkExe -Device STM32F042K6 -CommanderScript $(FLASH_SCRIPT)
	rm $(BUILD_TOOLS)/program.bin

tags:
	ctags -R

clean:
	rm -rf build
	rm -f tags
