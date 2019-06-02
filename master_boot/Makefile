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

SOURCES += $(USB)/usbd_cdc.c \
		   $(USB)/usbd_conf.c \
		   $(USB)/usbd_core.c \
		   $(USB)/usbd_ctlreq.c \
		   $(USB)/usbd_ioreq.c \
		   $(USB)/usbd_desc.c \
		   $(USB)/usb_otg.c

SOURCES += $(DRIVERS)/boot.c \
			  $(DRIVERS)/flash.c \

SOURCES += $(PERIPH)/gpio.c \
           $(PERIPH)/spi.c

SOURCES += $(UTILS)/doubleBuffer.c

INCLUDES += -I$(DEVICE)/include \
			-I$(HAL)/include \
			-I$(CMSIS)/include \
			-I$(SYSTEM) \
			-I$(DRIVERS)\
			-I$(PERIPH)\
			-I$(USB)\
			-I$(UTILS)\
			-I$(CONFIG)\
			-I.
	
CFLAGS += -DBOOTLOADER


.PHONY: all flash dev flash-dev clean tags

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

$(BUILDDIR)/$(HAL)/source/stm32f2xx_ll_usb.o : CFLAGS += -Wno-attributes
$(BUILDDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

flash: CFLAGS += -DSTM32F205xx
flash: $(BIN)
	JLinkExe -Device STM32F205RG -CommanderScript $(FLASH_SCRIPT)

dev: CFLAGS += -DSTM32F207xx
dev: $(BIN)

flash-stlink: dev
	st-flash write $(BIN) 0x8000000

tags:
	ctags -R

clean:
	rm -rf build
