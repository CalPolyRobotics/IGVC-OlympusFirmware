BUILDDIR = build

DEVICE = Libraries/CMSIS/Device
CORE = Libraries/CMSIS
PERIPH = Libraries/periph
UTILS = utils
DRIVERS = drivers
CONFIG = config

SOURCES += $(PERIPH)/src/stm32f2xx_gpio.c \
		   $(PERIPH)/src/stm32f2xx_i2c.c \
		   $(PERIPH)/src/stm32f2xx_rcc.c \
		   $(PERIPH)/src/stm32f2xx_spi.c \
		   $(PERIPH)/src/stm32f2xx_exti.c \
		   $(PERIPH)/src/stm32f2xx_syscfg.c \
		   $(PERIPH)/src/misc.c \
		   $(PERIPH)/src/stm32f2xx_usart.c

SOURCES += startup_stm32f2xx.s
SOURCES += $(CONFIG)/stm32f2xx_it.c
SOURCES += $(CONFIG)/system_stm32f2xx.c

SOURCES += main.c

SOURCES += $(DRIVERS)/usart.c

SOURCES += $(UTILS)/buffer8.c

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
 	
CFLAGS  = -O0 -g -Wall -I.\
   -mcpu=cortex-m3 -mthumb \
   -mfloat-abi=soft \
   $(INCLUDES) -DUSE_STDPERIPH_DRIVER

LDSCRIPT = buildTools/stm32_flash.ld
LDFLAGS += -T$(LDSCRIPT) -mthumb -mcpu=cortex-m3 -nostdlib -LbuildTools

LDLIBS += -lc -lm -lnosys

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
	JLinkExe -Device STM32F205RG -CommanderScript flash.jlink

clean:
	rm -rf build
