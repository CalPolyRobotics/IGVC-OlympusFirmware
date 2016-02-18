BUILDDIR = build

DEVICE = Drivers/CMSIS/Device/ST/STM32F2xx
CORE = Drivers/CMSIS
PERIPH = Drivers/STM32F2xx_HAL_Driver
UTILS = utils
DRIVERS = drivers
CONFIG = config
GENERATED = generatedPerif
USB = generatedPerif/USB


SOURCES += $(PERIPH)/Src/stm32f2xx_hal_gpio.c \
		   $(PERIPH)/Src/stm32f2xx_hal_i2c.c \
		   $(PERIPH)/Src/stm32f2xx_hal_rcc.c \
		   $(PERIPH)/Src/stm32f2xx_hal_spi.c \
		   $(PERIPH)/Src/stm32f2xx_hal_usart.c \
		   $(PERIPH)/Src/stm32f2xx_hal_uart.c \
		   $(PERIPH)/Src/stm32f2xx_hal_tim.c \
		   $(PERIPH)/Src/stm32f2xx_hal_tim_ex.c \
		   $(PERIPH)/Src/stm32f2xx_hal_dma.c \
		   $(PERIPH)/Src/stm32f2xx_hal_cortex.c \
		   $(PERIPH)/Src/stm32f2xx_hal_adc.c \
		   $(PERIPH)/Src/stm32f2xx_hal_adc_ex.c \
		   $(PERIPH)/Src/stm32f2xx_hal_dac.c \
		   $(PERIPH)/Src/stm32f2xx_hal_dac_ex.c \
		   $(PERIPH)/Src/stm32f2xx_hal_pcd.c \
		   $(PERIPH)/Src/stm32f2xx_hal_pcd_ex.c \
		   $(PERIPH)/Src/stm32f2xx_ll_usb.c \
		   $(PERIPH)/Src/stm32f2xx_hal.c \

SOURCES += system_stm32f2xx.c
SOURCES += startup_stm32f2xx.s
SOURCES += main.c
SOURCES += newlib_hooks.c
SOURCES += stm32f2xx_hal_msp.c
SOURCES += stm32f2xx_it.c
SOURCES += comms.c
SOURCES += fnr.c
SOURCES += speedDAC.c
SOURCES += kill.c
SOURCES += sevenSeg.c
#SOURCES += $(UTILS)/buffer8.c
SOURCES += $(GENERATED)/gpio.c
SOURCES += $(GENERATED)/usart.c
SOURCES += $(GENERATED)/dma.c
SOURCES += $(GENERATED)/dac.c
SOURCES += $(GENERATED)/adc.c
SOURCES += $(GENERATED)/spi.c
SOURCES += $(GENERATED)/i2c.c
SOURCES += $(GENERATED)/tim.c
SOURCES += $(GENERATED)/usb_otg.c
SOURCES += $(UTILS)/timerCallback.c
SOURCES += steering.c
SOURCES += $(USB)/usbd_cdc.c
SOURCES += $(USB)/usbd_conf.c
SOURCES += $(USB)/usbd_core.c
SOURCES += $(USB)/usbd_ctlreq.c
SOURCES += $(USB)/usbd_desc.c
SOURCES += $(USB)/usbd_ioreq.c

OBJECTS = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(SOURCES))))

INCLUDES += -I$(DEVICE)/Include \
			-I$(CORE)/include \
			-I$(PERIPH)/Inc \
			-I$(DISCOVERY)/include \
			-I$(USB)/include \
			-I$(CONFIG)\
			-I$(DRIVERS)\
			-I$(UTILS)\
			-I$(GENERATED)\
			-I$(USB)\
			-I\

ELF = $(BUILDDIR)/program.elf
HEX = $(BUILDDIR)/program.hex
BIN = $(BUILDDIR)/program.bin

CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
AR = arm-none-eabi-ar
OBJCOPY = arm-none-eabi-objcopy
 	
CFLAGS  = -O0 -g -Wall -I.\
   -mcpu=cortex-m3 -mthumb \
   -mfloat-abi=soft \
   $(INCLUDES) -DUSE_STDPERIPH_DRIVER \
   -D STM32F205xx -DUSE_USB_FS

LDSCRIPT = buildTools/stm32_flash.ld
LDFLAGS += -T$(LDSCRIPT) -mthumb -mcpu=cortex-m3 -nostdlib -LbuildTools

LDLIBS += -lc -lm -lnosys

FLASH_SCRIPT = buildTools/flash.jlink

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

$(ELF): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

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
