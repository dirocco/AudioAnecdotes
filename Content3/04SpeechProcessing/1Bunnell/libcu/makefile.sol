#------------------------------------------------------------------------
# Sun / Sparc Defines: Uncomment these for compilation on Sparcs
#------------------------------------------------------------------------
CC = gcc
FFLAGS = -O4 -I/vol/spl/include
# need -fpic to compile shared libs -- Will Lowe <lowe@asel.udel.edu>
CFLAGS = -O6 -I/vol/spl/include -fpic -D_BE_MACHINE
LIBDIR = /vol/spl/solaris/lib
DYN_LIB = libcu.so
DYN_FLAGS = -shared
RL = ranlib
# set the permissions right
INSTALL = /usr/local/gnu/bin/install

#------------------------------------------------------------------------
# SGI Defines: Uncomment these for compilation on Silicon Graphics
#    Note:      "so_locations" is made at linking time.  It doesn't seem to
#               be needed, so it's not copied during "install"
#------------------------------------------------------------------------
#CC = cc
#CFLAGS = -O2 -I/vol/spl/include
#LIBDIR = /usr/local/lib
#DYN_LIB = libcu.so
#DYN_FLAGS = -shared
#RL = ls -las

#------------------------------------------------------------------------
# Remaining definitions apply to all machines
#------------------------------------------------------------------------
 
SRCDSP = allocsp.c dcs.c defext.c jscat.c log2.c opnsig.c pearsn.c \
	prompt.c rflio.c stddev.c strpad.c uatola.c unpack.c \
	net_subs.c setext.c


OBJDSP = allocsp.o dcs.o defext.o jscat.o log2.o opnsig.o pearsn.o \
	prompt.o rflio.o stddev.o strpad.o uatola.o unpack.o \
	net_subs.o  setext.o

all: libcu.a $(DYN_LIB)

libcu.a: $(OBJDSP)
	ar ruv libcu.a $(OBJDSP)
	$(RL) libcu.a

$(DYN_LIB): $(OBJDSP)
	gcc $(DYN_FLAGS) -o $(DYN_LIB) $(OBJDSP)

install: libcu.a $(DYN_LIB)
	$(INSTALL) -m 664 -g speech libcu.a $(LIBDIR)/ 
	$(INSTALL) -m 775 -g speech $(DYN_LIB) $(LIBDIR)/

clean:
	/bin/rm -f $(OBJDSP) libcu.a $(DYN_LIB)

