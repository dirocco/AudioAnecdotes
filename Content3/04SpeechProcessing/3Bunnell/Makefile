#------------------------------------------------------------------------
# Makefile for PitchShift under cygwin
#------------------------------------------------------------------------
CC = gcc
CFLAGS = -O3 -DDLL_PREFIX="" -D_LE_MACHINE
LIBDIR = /usr/local/lib
BINDIR = /usr/local/bin

#------------------------------------------------------------------------
# Remaining definitions apply to all machines
#------------------------------------------------------------------------
 
SRCS = PitchShift.c psola.c ppsfns.c voicingVerify.c spenvel.c ptracks.c

OBJS = PitchShift.o psola.o ppsfns.o voicingVerify.o spenvel.o ptracks.o

all: PitchShift.exe

PitchShift.exe: $(OBJS)
	$(CC) -o PitchShift.exe -L $(LIBDIR) $(OBJS) -lm -lcu -lcwav -lcdsp

install: PitchShift.exe
	cp PitchShift.exe $(BINDIR)/

clean:
	rm -f $(OBJS)
