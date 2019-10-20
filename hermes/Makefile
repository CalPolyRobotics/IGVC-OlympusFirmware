BOOT_APP = ../usbFlasher/Boot.py

DRIVERS = drivers
PERIPHS = peripherals

SOURCES += main.c

SOURCES += $(STD_DRIVERS)/commsLib.c \
			$(STD_DRIVERS)/boot.c \

SOURCES += $(STD_PERIPHS)/spi.c \
			$(STD_PERIPHS)/systemClock.c

SOURCES += $(DRIVERS)/comms.c\
			$(DRIVERS)/speed.c\
			$(DRIVERS)/throttle.c

SOURCES += $(PERIPHS)/gpio.c \
			$(PERIPHS)/tim.c\
			$(PERIPHS)/i2c.c\
			$(PERIPHS)/adc.c

SOURCES += $(HAL)/src/stm32f0xx_hal_rcc.c \
			$(HAL)/src/stm32f0xx_hal_rcc_ex.c \
			$(HAL)/src/stm32f0xx_hal_flash.c \
			$(HAL)/src/stm32f0xx_hal_flash_ex.c \
			$(HAL)/src/stm32f0xx_hal_gpio.c \
			$(HAL)/src/stm32f0xx_hal_cortex.c \
			$(HAL)/src/stm32f0xx_hal_i2c.c\
			$(HAL)/src/stm32f0xx_hal_i2c_ex.c\
			$(HAL)/src/stm32f0xx_hal_dma.c\
			$(HAL)/src/stm32f0xx_hal.c

SOURCES += $(SYSTEM)/startup_stm32f042x6.s \
		   $(SYSTEM)/stm32f0xx_hal_msp.c \
		   $(SYSTEM)/stm32f0xx_it.c \
		   $(SYSTEM)/system_stm32f0xx.c

OBJECTS = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(SOURCES))))

INCLUDES += -I$(HAL)/inc \
			-I$(DRIVERS) \
			-I$(PERIPHS) \
			-I.


WARNINGS += -Wno-unused-function
	
LDSCRIPT_PRE = $(BUILD_TOOLS)/stm32f042_32k.ld
LDSCRIPT = $(BUILDDIR)/linker.ld
LDFLAGS += -T$(LDSCRIPT) \
           -mthumb \
           -mcpu=cortex-m0 \
           -LbuildTools

LDLIBS += -lc -lm -lnosys

START_ADDR = 0x8001400

.PHONY: flash

include slave_lib/inc.mak

flash: $(BIN)
	$(BOOT_APP) -m hermes -f $(BIN)
