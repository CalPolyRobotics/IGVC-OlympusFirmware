MODDIRS = apollo \
          hephaestus \
          hera \
          janus \
          olympus

MODDIR_BINS = $(addsuffix /build/program.bin, $(MODDIRS))

BOOTDIRS = master_boot \
           slave_boot


BOOT_APP = usbFlasher/Boot.py

SUBDIRS = $(MODDIRS) \
			$(BOOTDIRS)

CLEANDIRS = $(addprefix clean-, $(SUBDIRS))
TAGDIRS = $(addprefix tags-, $(SUBDIRS))

.PHONY: all clean tags $(SUBDIRS) $(CLEANDIRS)

all: $(MODDIRS)

$(SUBDIRS):
	$(MAKE) -C $@
	@echo ""

flash: $(MODDIRS)
	$(BOOT_APP) -m $(MODDIRS) -f $(MODDIR_BINS)

clean: $(CLEANDIRS)
$(CLEANDIRS):
	$(MAKE) -C $(subst clean-,,$@) clean

tags: $(TAGDIRS)
$(TAGDIRS):
	$(MAKE) -C $(subst tags-,,$@) tags
