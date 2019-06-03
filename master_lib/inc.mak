MLIB = master_lib
DEVICE = $(MLIB)/CMSIS/device/ST/STM32F2xx
CMSIS = $(MLIB)/CMSIS
HAL = $(MLIB)/STM32F2xx_HAL_Driver
CONFIG = $(MLIB)/config
BUILD_TOOLS = $(MLIB)/buildTools

BUILDDIR = build

DRIVERS = drivers
UTILS = utils
PERIPH = peripherals
USB = $(PERIPH)/USB
ETH = peripherals/ETH
LWIP = $(MLIB)/LwIP
SYSTEM = system

MPERIPH = $(MLIB)/$(PERIPH)
MDRIVERS =  $(MLIB)/$(DRIVERS)

ELF = $(BUILDDIR)/program.elf
HEX = $(BUILDDIR)/program.hex
BIN = $(BUILDDIR)/program.bin
MAP = $(BUILDDIR)/program.map

CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
AR = arm-none-eabi-ar
OBJCOPY = arm-none-eabi-objcopy

PP = cpp
PPFLAGS = -P

LDSCRIPT_PRE = $(BUILD_TOOLS)/stm32f20.ld
LDSCRIPT = $(BUILDDIR)/linker.ld
LDFLAGS += -T$(LDSCRIPT) -mthumb -mcpu=cortex-m3 -nostdlib -L$(BUILD_TOOLS)

LDLIBS += -lc -lm -lnosys

FLASH_SCRIPT = $(BUILD_TOOLS)/flash.jlink

WARNINGS = -Wall -Werror

CFLAGS  = -O0 -g \
   -mcpu=cortex-m3 -mthumb \
   -mfloat-abi=soft \
   $(INCLUDES) -DUSE_STDPERIPH_DRIVER \
   -DUSE_USB_FS \
   -DSTM32F207xx \
   $(WARNINGS)

OBJECTS = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(SOURCES))))

SOURCES += $(MDRIVERS)/submoduleComms.c
SOURCES += $(MPERIPH)/sysclock.c \
			$(MPERIPH)/spi.c \
			$(MPERIPH)/gpio.c

INCLUDES += -I$(MDRIVERS) \
				-I$(MPERIPH)
