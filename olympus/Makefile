BUILDDIR = build

BUILD_TOOLS = ../buildTools

MLIB = master_lib
DEVICE = $(MLIB)/CMSIS/device/ST/STM32F2xx
CMSIS = $(MLIB)/CMSIS
HAL = $(MLIB)/STM32F2xx_HAL_Driver

UTILS = utils
DRIVERS = drivers
CONFIG = config
PERIPHERALS = peripherals
USB = peripherals/USB
SYSTEM = system

BOOT_APP = ../master_boot/usbFlasher/Boot.py

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
		   $(DRIVERS)/submoduleComms.c \
		   $(DRIVERS)/apollo.c \
		   $(DRIVERS)/janus.c \
		   $(DRIVERS)/hera.c \
		   $(DRIVERS)/hephaestus.c

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

LDSCRIPT = $(BUILD_TOOLS)/stm32_flash.ld
LDFLAGS += -T$(LDSCRIPT) -mthumb -mcpu=cortex-m3 -nostdlib -L$(BUILD_TOOLS)

LDLIBS += -lc -lm -lnosys

FLASH_SCRIPT = $(BUILD_TOOLS)/flash.jlink


all: CFLAGS += -DSTM32F205xx
all: $(BIN)

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

.PHONY: flash
flash: CFLAGS += -DSTM32F205xx
flash: $(BIN)
	JLinkExe -Device STM32F205RG -CommanderScript $(FLASH_SCRIPT)

.PHONY: dev
dev: CFLAGS += -DSTM32F207xx -DBOOTLOAD
dev: $(BIN)

.PHONY: flash-dev
flash-dev: dev
	$(BOOT_APP) -m olympus -f $(BIN)

clean:
	rm -rf build
	rm -f tags