BUILDDIR = build

DEVICE = Libraries/CMSIS/Device
CORE = Libraries/CMSIS
PERIPH = Libraries/periph
UTILS = utils
DRIVERS = drivers
CONFIG = config

SOURCES += $(PERIPH)/src/stm32f2xx_hal_gpio.c \
		   $(PERIPH)/src/stm32f2xx_hal_i2c.c \
		   $(PERIPH)/src/stm32f2xx_hal_rcc.c \
		   $(PERIPH)/src/stm32f2xx_hal_spi.c \
		   $(PERIPH)/src/stm32f2xx_hal_usart.c \
		   $(PERIPH)/src/stm32f2xx_hal_tim.c \
		   $(PERIPH)/src/stm32f2xx_hal_tim_ex.c \
		   $(PERIPH)/src/stm32f2xx_hal_dma.c \
		   $(PERIPH)/src/stm32f2xx_hal_cortex.c \
		   $(PERIPH)/src/stm32f2xx_hal.c \

SOURCES += startup_stm32f2xx.s
SOURCES += $(CONFIG)/stm32f2xx_it.c
SOURCES += $(CONFIG)/system_stm32f2xx.c

SOURCES += main.c

SOURCES += $(DRIVERS)/usart.c
SOURCES += $(DRIVERS)/i2c.c
SOURCES += $(DRIVERS)/gpio.c

SOURCES += $(UTILS)/buffer8.c
SOURCES += $(UTILS)/timer.c

SOURCES += $(CONFIG)/newlib_hooks.c

OBJECTS = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(SOURCES))))

INCLUDES += -I$(DEVICE)/include \
			-I$(CORE)/include \
			-I$(PERIPH)/include \
			-I$(DISCOVERY)/include \
			-I$(USB)/include \
			-I$(CONFIG)\
			-I$(DRIVERS)\
			-I$(UTILS)\
			-I\

ELF = $(BUILDDIR)/program.elf
HEX = $(BUILDDIR)/program.hex
BIN = $(BUILDDIR)/program.bin

CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
AR = arm-none-eabi-ar
OBJCOPY = arm-none-eabi-objcopy
 	
CFLAGS  = -O0 -g -Wall -Werror -I.\
   -mcpu=cortex-m3 -mthumb \
   -mfloat-abi=soft \
   $(INCLUDES) -DUSE_STDPERIPH_DRIVER

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
