BUILDDIR = build/bin

#DEVICE = libraries/CMSIS/device/ST/STM32F0xx
CMSIS = ../libraries/CMSIS
HAL = ../libraries/STM32F0xx_HAL_Driver
UTILS = ../utils
CONFIG = ../config
SYSTEM = ../system
INC = ../inc
PERIPHERALS = ../peripherals
DRIVERS = drivers
SHRDRIVERS = ../drivers
BUILD_TOOLS = ../buildTools

SOURCES += main.c

SOURCES += $(DRIVERS)/comms.c

SOURCES += $(SHRDRIVERS)/commsLib.c \
			$(SHRDRIVERS)/boot.c \

SOURCES += $(PERIPHERALS)/spi.c

SOURCES += $(HAL)/src/stm32f0xx_hal_rcc.c \
			$(HAL)/src/stm32f0xx_hal_rcc_ex.c \
			$(HAL)/src/stm32f0xx_hal_flash.c \
			$(HAL)/src/stm32f0xx_hal_flash_ex.c \
			$(HAL)/src/stm32f0xx_hal_spi.c \
			$(HAL)/src/stm32f0xx_hal_spi_ex.c \
			$(HAL)/src/stm32f0xx_hal_dma.c \
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
			-I$(SYSTEM) \
			-I$(INC) \
			-I$(PERIPHERALS) \
			-I$(SHRDRIVERS) \
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
	
CFLAGS  = -Os -g \
   -mcpu=cortex-m0 -mthumb \
   -mfloat-abi=soft \
   $(INCLUDES) -DUSE_STDPERIPH_DRIVER \
   -D STM32F042x6 \
   $(WARNINGS)

LDSCRIPT = $(BUILD_TOOLS)/STM32F042C6Tx_FLASH.ld
LDFLAGS += -T$(LDSCRIPT) -mthumb -mcpu=cortex-m0 -LbuildTools

LDLIBS += -lc -lm -lnosys

FLASH_SCRIPT = $(BUILD_TOOLS)/flashF042C6.jlink

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

$(ELF): $(OBJECTS)
	$(LD) $(LDFLAGS) -Wl,-Map=$(MAP) -o $@ $(OBJECTS) $(LDLIBS)
	arm-none-eabi-size $@

$(BUILDDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

flash: $(BIN)
	cp $(BIN) $(BUILD_TOOLS)/program.bin
	JLinkExe -Device STM32F042K6 -CommanderScript $(FLASH_SCRIPT)
	rm $(BUILD_TOOLS)/program.bin

flash-st-link: $(BIN)
	st-flash write $(BIN) 0x8000000

clean:
	rm -rf build
