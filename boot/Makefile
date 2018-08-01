LIBRARIES = libraries
DEVICE = $(LIBRARIES)/CMSIS/device/ST/STM32F2xx
CMSIS = $(LIBRARIES)/CMSIS
HAL = $(LIBRARIES)/STM32F2xx_HAL_Driver

DRIVERS = drivers
PERIPH = peripherals
SYSTEM = system
UTILS = utils

USB = $(PERIPH)/USB

BUILDTOOLS = ../buildTools
BUILDDIR = build

SOURCES += main.c

SOURCES += $(HAL)/source/stm32f2xx_hal_gpio.c \
		   $(HAL)/source/stm32f2xx_hal_rcc.c \
		   $(HAL)/source/stm32f2xx_hal_cortex.c \
		   $(HAL)/source/stm32f2xx_hal.c \
		   $(HAL)/source/stm32f2xx_hal_pcd.c \
		   $(HAL)/source/stm32f2xx_hal_pcd_ex.c \
		   $(HAL)/source/stm32f2xx_ll_usb.c

SOURCES += $(SYSTEM)/startup_stm32f2xx.s \
		   $(SYSTEM)/stm32f2xx_hal_msp.c \
		   $(SYSTEM)/stm32f2xx_it.c \
		   $(SYSTEM)/system_stm32f2xx.c

SOURCES += $(USB)/usbd_cdc.c \
		   $(USB)/usbd_conf.c \
		   $(USB)/usbd_core.c \
		   $(USB)/usbd_ctlreq.c \
		   $(USB)/usbd_ioreq.c \
		   $(USB)/usbd_desc.c \
		   $(USB)/usb_otg.c

SOURCES += $(DRIVERS)/boot.c \
			  $(DRIVERS)/flash.c

SOURCES += $(PERIPH)/gpio.c

SOURCES += $(UTILS)/doubleBuffer.c

OBJECTS = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(SOURCES))))

INCLUDES += -I$(DEVICE)/include \
			-I$(HAL)/include \
			-I$(CMSIS)/include \
			-I$(SYSTEM) \
			-I$(DRIVERS)\
			-I$(PERIPH)\
			-I$(USB)\
			-I$(UTILS)\
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
	
CFLAGS  = -O0 -g \
   -mcpu=cortex-m3 -mthumb \
   -mfloat-abi=soft \
   $(INCLUDES) -DUSE_STDPERIPH_DRIVER \
   -DUSE_USB_FS \
   $(WARNINGS)

LDSCRIPT = $(BUILDTOOLS)/stm32_boot.ld
LDFLAGS += -T$(LDSCRIPT) -mthumb -mcpu=cortex-m3

LDLIBS += -lc -lm -lnosys

FLASH_SCRIPT = $(BUILDTOOLS)/flash.jlink

.PHONY: all flash dev flash-dev clean

all: CFLAGS += -DSTM32F205xx
all: $(BIN)

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

$(ELF): $(OBJECTS)
	$(LD) $(LDFLAGS) -Wl,-Map=$(MAP) -o $@ $(subst ../,,$(OBJECTS)) $(LDLIBS)
	arm-none-eabi-size $(subst ../,,$@)

$(BUILDDIR)/%.o: %.c
	mkdir -p $(dir $(subst ../,,$@))
	$(CC) -c $(CFLAGS) $< -o $(subst ../,,$@)

$(BUILDDIR)/%.o: %.s
	mkdir -p $(dir $(subst ../,,$@))
	$(CC) -c $(CFLAGS) $< -o $(subst ../,,$@)

flash: CFLAGS += -DSTM32F205xx
flash: $(BIN)
	JLinkExe -Device STM32F205RG -CommanderScript $(FLASH_SCRIPT)

dev: CFLAGS += -DSTM32F207xx
dev: $(BIN)

flash-dev: dev
	st-flash write $(BIN) 0x8000000

clean:
	rm -rf build
