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
 
SRCS = TimeScale.c lowpass.c bestpt.c

OBJS = TimeScale.o lowpass.o bestpt.o

all: TimeScale.exe

TimeScale.exe: $(OBJS)
	$(CC) -o TimeScale.exe -L $(LIBDIR) $(OBJS) -lm -lcu -lcwav -lcdsp

install: TimeScale.exe
	cp TimeScale.exe $(BINDIR)/

clean:
	rm -f $(OBJS)
