SUBDIRS = olympus \
			 apollo \
			 hera \
			 hephaestus \
			 janus

.PHONY: all clean tags olympus apollo hera hephaestus janus master_boot slave_boot

all: olympus apollo hera hephaestus janus

olympus:
	$(MAKE) -C $@

apollo:
	$(MAKE) -C $@

hera:
	$(MAKE) -C $@

hephaestus:
	$(MAKE) -C $@

janus:
	$(MAKE) -C $@

master_boot:
	$(MAKE) -C $@

slave_boot:
	$(MAKE) -C $@

clean:
	$(MAKE) -C olympus $@
	$(MAKE) -C apollo $@
	$(MAKE) -C hera $@
	$(MAKE) -C hephaestus $@
	$(MAKE) -C janus $@
	$(MAKE) -C master_boot $@
	$(MAKE) -C slave_boot $@

tags:
	cd olympus; ctags -R; cd ..
	cd apollo; ctags -R; cd ..
	cd hera; ctags -R; cd ..
	cd hephaestus; ctags -R; cd ..
	cd janus; ctags -R; cd ..
