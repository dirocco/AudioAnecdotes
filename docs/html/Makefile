TOPDIR=../..

# unfortunately gnu cpp stopped working after 3.2 (can't deal with #comments)
# CPP=cpp -traditional -P -C -fpreprocessed
CPP=../../bin/cpp.pl

TARGETS=linux-do.cgi win-do.cgi mac-do.cgi \
debuglinux-do.cgi debugwin-do.cgi debugmac-do.cgi \
06win.html 06unix.html

dist:
	$(TOPDIR)/bin/tbmaker $(TYPE) $(TOPDIR)/distro/ $(TOPDIR)/dirt/

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

06win.html: 06ComputerTechniques.html
	$(CPP) 06ComputerTechniques.html -D WINDOWS > 06win.html

06unix.html: 06ComputerTechniques.html
	$(CPP) 06ComputerTechniques.html > 06unix.html

clean:
	rm -f linux-do.cgi win-do.cgi mac-do.cig

