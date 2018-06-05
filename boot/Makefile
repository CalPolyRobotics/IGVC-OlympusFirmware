BUILDDIR = build/bin

DEVICE = ../libraries/CMSIS/device/ST/STM32F2xx
CMSIS = ../libraries/CMSIS
LIBRARIES = ../libraries/STM32F2xx_HAL_Driver
DRIVERS = drivers
PERIPH_LOC = peripherals
PERIPH_SHR = ../peripherals
SYSTEM = system
USB = $(PERIPH_SHR)/USB
BUILDTOOLS = ../buildTools
UTILS = utils

SOURCES += main.c

SOURCES += $(LIBRARIES)/source/stm32f2xx_hal_gpio.c \
		   $(LIBRARIES)/source/stm32f2xx_hal_rcc.c \
		   $(LIBRARIES)/source/stm32f2xx_hal_cortex.c \
		   $(LIBRARIES)/source/stm32f2xx_hal.c \
		   $(LIBRARIES)/source/stm32f2xx_hal_pcd.c \
		   $(LIBRARIES)/source/stm32f2xx_hal_pcd_ex.c \


SOURCES += $(SYSTEM)/startup_stm32f2xx.s \
		   $(SYSTEM)/stm32f2xx_hal_msp.c \
		   $(SYSTEM)/stm32f2xx_it.c \
		   $(SYSTEM)/system_stm32f2xx.c

SOURCE += $(PERIPH_SHR)/usb_otg.c

SOURCES += $(USB)/usbd_cdc.c \
		   $(USB)/usbd_conf.c \
		   $(USB)/usbd_core.c \
		   $(USB)/usbd_ctlreq.c \
		   $(USB)/usbd_desc.c \
		   $(USB)/usbd_ioreq.c

SOURCES += $(PERIPH_LOC)/gpio.c

SOURCES += $(DRIVERS)/boot.c \
			  $(DRIVERS)/flash.c

SOURCES += $(UTILS)/doubleBuffer.c

OBJECTS = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(SOURCES))))

INCLUDES += -I$(DEVICE)/include \
			-I$(LIBRARIES)/include \
			-I$(CMSIS)/include \
			-I$(USB)/include \
			-I$(SYSTEM) \
			-I$(DRIVERS)\
			-I$(PERIPH_LOC)\
			-I$(PERIPH_SHR)\
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
   -D STM32F205xx -DUSE_USB_FS \
   $(WARNINGS)

LDSCRIPT = $(BUILDTOOLS)/stm32_flash.ld
LDFLAGS += -T$(LDSCRIPT) -mthumb -mcpu=cortex-m3

LDLIBS += -lc -lm -lnosys

FLASH_SCRIPT = $(BUILDTOOLS)/flash.jlink

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
	JLinkExe -Device STM32F205RG -CommanderScript $(FLASH_SCRIPT)

clean:
	rm -rf build
