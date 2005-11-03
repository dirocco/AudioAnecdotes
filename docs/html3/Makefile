TOPDIR=../..

# unfortunately gnu cpp stopped working after 3.2 (can't deal with #comments)
# CPP=cpp -traditional -P -C -fpreprocessed
CPP=../../bin/cpp.pl

all: $(TARGETS)

dist:
	$(TOPDIR)/bin/tbmaker $(TYPE) $(TOPDIR)/distro/ $(TOPDIR)/dirt/

clean:

