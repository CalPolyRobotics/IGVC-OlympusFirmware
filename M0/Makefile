SUBDIRS = apollo \
			 hera \
			 hephaestus \
			 janus

.PHONY: all clean

all:
	$(MAKE) -C apollo
	$(MAKE) -C hera
	$(MAKE) -C hephaestus
	$(MAKE) -C janus


clean:
	$(MAKE) -C apollo $@
	$(MAKE) -C hera $@
	$(MAKE) -C hephaestus $@
	$(MAKE) -C janus $@
