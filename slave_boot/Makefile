DRIVERS = drivers
PERIPHS = peripherals

# This must come before all interrupt handlers to ensure -flto does not
# optimize out interrupt service routines
SOURCES += $(SYSTEM)/startup_stm32f042x6.s \
		   $(SYSTEM)/system_stm32f0xx.c

SOURCES += main.c

SOURCES += $(STD_DRIVERS)/commsLib.c

SOURCES += $(DRIVERS)/comms.c

SOURCES += $(STD_PERIPHS)/spi.c \
			$(STD_PERIPHS)/systemClock.c \

SOURCES += $(PERIPHS)/flash.c


OBJECTS = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(SOURCES))))

INCLUDES += -I$(DRIVERS) \
			-I$(PERIPHS) \
			-I.

CFLAGS += -flto \
			-DBOOTLOAD \
			-DTINY

PPFLAGS += -DBOOTLOAD

LDSCRIPT_PRE = $(BUILD_TOOLS)/stm32f042_32k.ld
LDSCRIPT = $(BUILDDIR)/linker.ld
LDFLAGS += -T$(LDSCRIPT) \
			  --specs=nosys.specs \
			  -flto \
           -mthumb \
           -mcpu=cortex-m0 \
           -LbuildTools

LDLIBS += -lnosys

FLASH_SCRIPT = $(BUILD_TOOLS)/app.jlink

START_ADDR = 0x8000000

.PHONY: all flash flash-dev clean tags

include slave_lib/inc.mak
