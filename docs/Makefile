TOPDIR=..
CHILDDIRS = html

makechildren: $(CHILDDIRS)
        if [ "$(COMD)" = "dist" ]; then  $(TOPDIR)/bin/tbmaker $(TYPE) $(TOPDIR)/distro/ $(TOPDIR)/dirt/;fi

FORCE:
        #Subdirectories must always be built

$(CHILDDIRS): FORCE
        $(MAKE) -C $@ $(COMD) TOPDIR=$(TOPDIR)

%:
        $(MAKE) makechildren COMD=$* TOPDIR=$(TOPDIR)
