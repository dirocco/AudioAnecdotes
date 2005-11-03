TOPDIR=..

all:
	$(MAKE) -C html  $(COMD) TOPDIR=$(TOPDIR)
	$(MAKE) -C html2 $(COMD) TOPDIR=$(TOPDIR)
	$(MAKE) -C html3 $(COMD) TOPDIR=$(TOPDIR)

dist:
	$(TOPDIR)/bin/tbmaker $(TYPE) $(TOPDIR)/distro/ $(TOPDIR)/dirt/
	$(MAKE) -C html  dist  TOPDIR=$(TOPDIR)
	$(MAKE) -C html2 dist  TOPDIR=$(TOPDIR)
	$(MAKE) -C html3 dist  TOPDIR=$(TOPDIR)

clean:
	$(MAKE) -C html  clean TOPDIR=$(TOPDIR)
	$(MAKE) -C html2 clean TOPDIR=$(TOPDIR)
	$(MAKE) -C html3 clean TOPDIR=$(TOPDIR)
