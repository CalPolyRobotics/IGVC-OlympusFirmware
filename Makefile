MODDIRS = olympus \
			 apollo \
			 hera \
			 hephaestus \
			 janus

BOOTDIRS = master_boot \
           slave_boot

SUBDIRS = $(MODDIRS) \
			$(BOOTDIRS)

CLEANDIRS = $(addprefix clean-, $(SUBDIRS))
TAGDIRS = $(addprefix tags-, $(SUBDIRS))

.PHONY: all clean tags $(SUBDIRS) $(CLEANDIRS)

all: $(MODDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

clean: $(CLEANDIRS)
$(CLEANDIRS):
	$(MAKE) -C $(subst clean-,,$@) clean

tags: $(TAGDIRS)
$(TAGDIRS):
	$(MAKE) -C $(subst tags-,,$@) tags
