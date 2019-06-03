include master_lib/inc.mak

SOURCES += main.c

SOURCES += $(HAL)/source/stm32f2xx_hal_gpio.c \
		   $(HAL)/source/stm32f2xx_hal_rcc.c \
		   $(HAL)/source/stm32f2xx_hal_cortex.c \
		   $(HAL)/source/stm32f2xx_hal.c \
		   $(HAL)/source/stm32f2xx_hal_pcd.c \
		   $(HAL)/source/stm32f2xx_hal_pcd_ex.c \
		   $(HAL)/source/stm32f2xx_ll_usb.c \
		   $(HAL)/source/stm32f2xx_hal_dma.c \
		   $(HAL)/source/stm32f2xx_hal_spi.c \

SOURCES += $(SYSTEM)/startup_stm32f2xx.s \
		   $(SYSTEM)/stm32f2xx_hal_msp.c \
		   $(SYSTEM)/stm32f2xx_it.c \
		   $(SYSTEM)/system_stm32f2xx.c

SOURCES += $(DRIVERS)/boot.c \
			  $(DRIVERS)/flash.c \

SOURCES += $(UTILS)/doubleBuffer.c

INCLUDES += -I$(DEVICE)/include \
			-I$(HAL)/include \
			-I$(CMSIS)/include \
			-I$(SYSTEM) \
			-I$(DRIVERS)\
			-I$(PERIPH)\
			-I$(UTILS)\
			-I$(CONFIG)\
			-I.
	
CFLAGS += -DBOOTLOADER
PPFLAGS += -DBOOTLOADER

.PHONY: all flash dev flash-dev clean tags

all: $(BIN) $(HEX)

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

$(LDSCRIPT): $(LDSCRIPT_PRE)
	$(PP) $(PPFLAGS) $< -o $@

$(ELF): $(OBJECTS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -Wl,-Map=$(MAP) -o $@ $(OBJECTS) $(LDLIBS)
	arm-none-eabi-size $@

$(BUILDDIR)/$(HAL)/source/stm32f2xx_ll_usb.o : CFLAGS += -Wno-attributes
$(BUILDDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

flash: $(HEX)
	JLinkExe -CommanderScript $(FLASH_SCRIPT)

flash-stlink: CFLAGS += -DDEV
flash-stlink: $(HEX)
	st-flash --format ihex write $(HEX)

tags:
	ctags -R

clean:
	rm -rf build
