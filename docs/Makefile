TOPDIR=..

dist-src:
	$(TOPDIR)/bin/tbmaker src_v1 $(TOPDIR)/distro/ $(TOPDIR)/dirt
	make -C html dist TOPDIR=$(TOPDIR) TYPE=src_v1

dist-linux:
	$(TOPDIR)/bin/tbmaker bin_v1_linux $(TOPDIR)/distro/ $(TOPDIR)/dirt
	make -C html dist TOPDIR=$(TOPDIR) TYPE=bin_v1_linux

dist-win:
	$(TOPDIR)/bin/tbmaker bin_v1_win $(TOPDIR)/distro/ $(TOPDIR)/dirt
	make -C html dist TOPDIR=$(TOPDIR) TYPE=bin_v1_win

dist-mac:
	$(TOPDIR)/bin/tbmaker bin_v1_mac $(TOPDIR)/distro/ $(TOPDIR)/dirt
	make -C html dist TOPDIR=$(TOPDIR) TYPE=bin_v1_mac
