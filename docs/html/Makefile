TOPDIR=../..

CPP=cpp -traditional -P -C -fpreprocessed

TARGETS=linux-do.cgi win-do.cgi mac-do.cgi \
debuglinux-do.cgi debugwin-do.cgi debugmac-do.cgi

all: $(TARGETS)


debuglinux-do.cgi: do.cgi
	$(CPP) do.cgi -D DEBUG -D LINUX -D UNIX > debuglinux-do.cgi

debugwin-do.cgi: do.cgi
	$(CPP) do.cgi -D DEBUG -D WINDOWS > debugwin-do.cgi

debugmac-do.cgi: do.cgi
	$(CPP) do.cgi -D DEBUG -D MAC -D UNIX > debugmac-do.cgi

linux-do.cgi: do.cgi
	$(CPP) do.cgi -D LINUX -D UNIX > linux-do.cgi

win-do.cgi: do.cgi
	$(CPP) do.cgi -D WINDOWS > win-do.cgi

mac-do.cgi: do.cgi
	$(CPP) do.cgi -D MAC -D UNIX > mac-do.cgi

dist:
	$(TOPDIR)/bin/tbmaker $(TYPE) $(TOPDIR)/distro/ $(TOPDIR)/dirt/

clean:
	rm -f linux-do.cgi win-do.cgi mac-do.cig
