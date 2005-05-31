#------------------------------------------------------------------------
# Makefile for PitchShift under cygwin
#------------------------------------------------------------------------
CC = gcc
INCL = ../4Bunnell
CFLAGS = -O3 -I $(INCL) -D_LE_MACHINE

#------------------------------------------------------------------------
# Remaining definitions apply to all machines
#------------------------------------------------------------------------
 
SRCS = TimeScale.c lowpass.c bestpt.c unpack.c fp_fft.c

OBJS = TimeScale.o lowpass.o bestpt.o unpack.o fp_fft.o

all: TimeScale

TimeScale: $(OBJS) ../4Bunnell/libcwav.a
	$(CC) -o TimeScale -L../4Bunnell $(OBJS) -lcwav -lm

../4Bunnell/libcwav.a:
	make -C ../4Bunnell -f makefile.lnx

clean:
	rm -f $(OBJS)
