include master_lib/inc.mak

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
		   $(HAL)/source/stm32f2xx_hal_eth.c \
		   $(HAL)/source/stm32f2xx_hal.c

SOURCES += $(SYSTEM)/startup_stm32f2xx.s \
		   $(SYSTEM)/stm32f2xx_hal_msp.c \
		   $(SYSTEM)/stm32f2xx_it.c \
		   $(SYSTEM)/system_stm32f2xx.c

SOURCES += $(PERIPH)/dac.c \
		   $(PERIPH)/dma.c \
		   $(PERIPH)/tim.c \
		   $(PERIPH)/usart.c

SOURCES += $(ETH)/app_ethernet.c \
			$(ETH)/eth.c \
			$(ETH)/ethernetif.c \
			$(ETH)/tcp_server.c

SOURCES += $(LWIP)/src/core/mem.c \
			$(LWIP)/src/core/init.c \
			$(LWIP)/src/core/memp.c \
			$(LWIP)/src/core/netif.c \
			$(LWIP)/src/core/timeouts.c \
			$(LWIP)/src/core/inet_chksum.c \
			$(LWIP)/src/core/tcp.c \
			$(LWIP)/src/core/udp.c \
			$(LWIP)/src/core/tcp_in.c \
			$(LWIP)/src/core/tcp_out.c \
			$(LWIP)/src/core/def.c \
			$(LWIP)/src/core/pbuf.c \
			$(LWIP)/src/core/ip.c \
			$(LWIP)/src/netif/ethernet.c \
			$(LWIP)/src/core/ipv4/ip4.c \
			$(LWIP)/src/core/ipv4/icmp.c \
			$(LWIP)/src/core/ipv4/etharp.c \
			$(LWIP)/src/core/ipv4/dhcp.c \
			$(LWIP)/src/core/ipv4/ip4_addr.c \
			$(LWIP)/src/core/ipv4/ip4_frag.c

SOURCES += $(DRIVERS)/comms.c \
		   $(DRIVERS)/console.c \
		   $(DRIVERS)/kill.c \
		   $(DRIVERS)/led.c \
		   $(DRIVERS)/sevenSeg.c \
		   $(DRIVERS)/speedDAC.c \
		   $(DRIVERS)/pwradc.c \
		   $(DRIVERS)/error.c \
		   $(DRIVERS)/apollo.c \
		   $(DRIVERS)/hephaestus.c\
		   $(DRIVERS)/hermes.c \
		   $(DRIVERS)/janus.c \
		   $(DRIVERS)/zeus.c \
		   $(DRIVERS)/sounds.c \
		   $(DRIVERS)/status.c 


SOURCES += $(UTILS)/buffer8.c \
		   $(UTILS)/newlib_hooks.c \
		   $(UTILS)/timerCallback.c \
		   $(UTILS)/characterMapping.c \
		   $(UTILS)/crc8.c \
		   $(UTILS)/doubleBuffer.c

INCLUDES += -I$(DEVICE)/include \
			-I$(HAL)/include \
			-I$(CMSIS)/include \
			-I$(ETH) \
			-I$(LWIP)/src/include \
			-I$(LWIP)/system \
			-I$(SYSTEM) \
			-I$(CONFIG)\
			-I$(DRIVERS)\
			-I$(UTILS)\
			-I$(PERIPH)\
			-I.


WARNINGS += -Wno-unused-function

LDFLAGS += -nostdlib -L$(BUILD_TOOLS)

.PHONY: all flash tags dev flash-dev

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

flash: $(BIN)
	$(BOOT_APP) -m olympus -f $(BIN)

flash-jlink: $(HEX)
	JLinkExe -CommanderScript $(FLASH_SCRIPT)

dev: CFLAGS += -DDEV
dev: $(BIN) $(HEX)

flash-dev: dev
	$(BOOT_APP) -m olympus -f $(BIN)

flash-dev-stlink: dev
	st-flash --format ihex write $(HEX)

tags:
	ctags -R

clean:
	rm -rf build
	rm -f tags
