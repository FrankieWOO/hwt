SUBDIRS := external $(wildcard */.)
CLEANDIRS = $(SUBDIRS:%=clean-%)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	 $(MAKE) -C $@

clean: $(CLEANDIRS)
$(CLEANDIRS): 
	$(MAKE) -C $(@:clean-%=%) clean

.PHONY: subdirs $(SUBDIRS)
.PHONY: clean $(CLEANDIRS)

