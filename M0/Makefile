SUBDIRS = apollo \
			 hera \
			 hephaestus \
			 janus

.PHONY: all clean tags

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

tags:
	cd apollo; ctags -R; cd ..
	cd hera; ctags -R; cd ..
	cd hephaestus; ctags -R; cd ..
	cd janus; ctags -R; cd ..
