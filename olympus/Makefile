BUILDDIR = build

MLIB = master_lib
DEVICE = $(MLIB)/CMSIS/device/ST/STM32F2xx
CMSIS = $(MLIB)/CMSIS
HAL = $(MLIB)/STM32F2xx_HAL_Driver
BUILD_TOOLS = $(MLIB)/buildTools

UTILS = utils
DRIVERS = drivers
CONFIG = config
PERIPHERALS = peripherals
USB = peripherals/USB
SYSTEM = system

BOOT_APP = ../usbFlasher/Boot.py

SOURCES += main.c

SOURCES += $(HAL)/source/stm32f2xx_hal_gpio.c \
		   $(HAL)/source/stm32f2xx_hal_i2c.c \
		   $(HAL)/source/stm32f2xx_hal_rcc.c \
		   $(HAL)/source/stm32f2xx_hal_spi.c \
		   $(HAL)/source/stm32f2xx_hal_usart.c \
		   $(HAL)/source/stm32f2xx_hal_uart.c \
		   $(HAL)/source/stm32f2xx_hal_tim.c \
		   $(HAL)/source/stm32f2xx_hal_tim_ex.c \
		   $(HAL)/source/stm32f2xx_hal_dma.c \
		   $(HAL)/source/stm32f2xx_hal_cortex.c \
		   $(HAL)/source/stm32f2xx_hal_adc.c \
		   $(HAL)/source/stm32f2xx_hal_adc_ex.c \
		   $(HAL)/source/stm32f2xx_hal_dac.c \
		   $(HAL)/source/stm32f2xx_hal_dac_ex.c \
		   $(HAL)/source/stm32f2xx_hal_pcd.c \
		   $(HAL)/source/stm32f2xx_hal_pcd_ex.c \
		   $(HAL)/source/stm32f2xx_ll_usb.c \
		   $(HAL)/source/stm32f2xx_hal.c

SOURCES += $(SYSTEM)/startup_stm32f2xx.s \
		   $(SYSTEM)/stm32f2xx_hal_msp.c \
		   $(SYSTEM)/stm32f2xx_it.c \
		   $(SYSTEM)/system_stm32f2xx.c

SOURCES += $(PERIPHERALS)/dac.c \
		   $(PERIPHERALS)/dma.c \
		   $(PERIPHERALS)/gpio.c \
		   $(PERIPHERALS)/spi.c \
		   $(PERIPHERALS)/tim.c \
		   $(PERIPHERALS)/usart.c \
		   $(PERIPHERALS)/usb_otg.c

SOURCES += $(USB)/usbd_cdc.c \
		   $(USB)/usbd_conf.c \
		   $(USB)/usbd_core.c \
		   $(USB)/usbd_ctlreq.c \
		   $(USB)/usbd_desc.c \
		   $(USB)/usbd_ioreq.c

SOURCES += $(DRIVERS)/comms.c \
		   $(DRIVERS)/console.c \
		   $(DRIVERS)/kill.c \
		   $(DRIVERS)/led.c \
		   $(DRIVERS)/sevenSeg.c \
		   $(DRIVERS)/speedDAC.c \
		   $(DRIVERS)/pwradc.c \
		   $(DRIVERS)/error.c \
		   $(DRIVERS)/submoduleComms.c \
		   $(DRIVERS)/apollo.c \
		   $(DRIVERS)/janus.c \
		   $(DRIVERS)/hera.c \
		   $(DRIVERS)/hephaestus.c\
		   $(DRIVERS)/sounds.c
  

SOURCES += $(UTILS)/buffer8.c \
		   $(UTILS)/newlib_hooks.c \
		   $(UTILS)/timerCallback.c \
		   $(UTILS)/characterMapping.c \
		   $(UTILS)/crc8.c \
		   $(UTILS)/doubleBuffer.c

OBJECTS = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(SOURCES))))

INCLUDES += -I$(DEVICE)/include \
			-I$(HAL)/include \
			-I$(CMSIS)/include \
			-I$(USB)/include \
			-I$(SYSTEM) \
			-I$(CONFIG)\
			-I$(DRIVERS)\
			-I$(UTILS)\
			-I$(PERIPHERALS)\
			-I$(USB)\
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

CFLAGS  = -O0 -g \
   -mcpu=cortex-m3 -mthumb \
   -mfloat-abi=soft \
   $(INCLUDES) -DUSE_STDPERIPH_DRIVER \
   -DUSE_USB_FS \
   $(WARNINGS)

PP = cpp
PPFLAGS = -P

LDSCRIPT_PRE = $(BUILD_TOOLS)/stm32f20.ld
LDSCRIPT = $(BUILDDIR)/linker.ld
LDFLAGS += -T$(LDSCRIPT) -mthumb -mcpu=cortex-m3 -nostdlib -L$(BUILD_TOOLS)

LDLIBS += -lc -lm -lnosys

FLASH_SCRIPT = $(BUILD_TOOLS)/flash.jlink


.PHONY: all flash tags dev flash-dev
all: CFLAGS += -DSTM32F205xx
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

# Devices using STM32F205xx
std: CFLAGS += -DSTM32F205xx
std: $(BIN)

flash: std
	$(BOOT_APP) -m olympus -f $(BIN)

flash-jlink: std
	JLinkExe -Device STM32F205RG -CommanderScript $(FLASH_SCRIPT)


# Devices using STM32F207xx
dev: CFLAGS += -DSTM32F207xx
dev: $(BIN)

flash-dev: dev
	$(BOOT_APP) -m olympus -f $(BIN)

flash-dev-stlink: dev
	st-flash write $(BIN) 0x8020000

tags:
	ctags -R

clean:
	rm -rf build
	rm -f tags
