TOPDIR=../..

# unfortunately gnu cpp stopped working after 3.2 (can't deal with #comments)
# CPP=cpp -traditional -P -C -fpreprocessed
CPP=../../bin/cpp.pl
SED=/bin/sed

TARGETS=linux-do.cgi win-do.cgi mac-do.cgi \
debuglinux-do.cgi debugwin-do.cgi debugmac-do.cgi \
06win.html 06unix.html head1.html head2.html main1.html main2.html \
build.html.cd1 build.html.cd2

all: $(TARGETS)

dist:
	$(TOPDIR)/bin/tbmaker $(TYPE) $(TOPDIR)/distro/ $(TOPDIR)/dirt/

%.cd1: %
	$(SED) s/VOLUME/Volume1/g < $< > $@

%.cd2: %
	$(SED) s/VOLUME/Volume2/g < $< > $@

debuglinux-do.cgi: do.cgi
	$(CPP) do.cgi -D DEBUG -D LINUX -D UNIX > debuglinux-do.cgi

debugwin-do.cgi: do.cgi
	$(CPP) do.cgi -D DEBUG -D WINDOWS > debugwin-do.cgi

debugmac-do.cgi: do.cgi
	$(CPP) do.cgi -D DEBUG -D MAC > debugmac-do.cgi

linux-do.cgi: do.cgi
	$(CPP) do.cgi -D LINUX -D UNIX > linux-do.cgi

win-do.cgi: do.cgi
	$(CPP) do.cgi -D WINDOWS > win-do.cgi

mac-do.cgi: do.cgi
	$(CPP) do.cgi -D MAC > mac-do.cgi

06win.html: 06ComputerTechniques.html
	$(CPP) 06ComputerTechniques.html -D WINDOWS > 06win.html

06unix.html: 06ComputerTechniques.html
	$(CPP) 06ComputerTechniques.html > 06unix.html

head1.html: head.html
	$(CPP) head.html -D CD1 > head1.html

head2.html: head.html
	$(CPP) head.html -D CD2 > head2.html

main1.html: main.html
	$(CPP) main.html -D CD1 > main1.html

main2.html: main.html
	$(CPP) main.html -D CD2 > main2.html

clean:
	rm -f linux-do.cgi win-do.cgi mac-do.cig

