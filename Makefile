SUBDIRS = olympus \
			 apollo \
			 hera \
			 hephaestus \
			 janus

.PHONY: all clean tags olympus apollo hera hephaestus janus

all: olympus apollo hera hephaestus janus

olympus:
	$(MAKE) -C olympus

apollo:
	$(MAKE) -C apollo

hera:
	$(MAKE) -C hera

hephaestus:
	$(MAKE) -C hephaestus

janus:
	$(MAKE) -C janus

clean:
	$(MAKE) -C olympus $@
	$(MAKE) -C apollo $@
	$(MAKE) -C hera $@
	$(MAKE) -C hephaestus $@
	$(MAKE) -C janus $@

tags:
	cd olympus; ctags -R; cd ..
	cd apollo; ctags -R; cd ..
	cd hera; ctags -R; cd ..
	cd hephaestus; ctags -R; cd ..
	cd janus; ctags -R; cd ..
