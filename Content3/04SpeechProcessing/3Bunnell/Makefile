#------------------------------------------------------------------------
# Makefile for PitchShift under cygwin
#------------------------------------------------------------------------
CC = gcc
INCL = ../4Bunnell
CFLAGS = -O3 -D_LE_MACHINE -I $(INCL)
#------------------------------------------------------------------------
# Remaining definitions apply to all machines
#------------------------------------------------------------------------
 
SRCS = PitchShift.c psola.c ppsfns.c voicingVerify.c spenvel.c ptracks.c \
	fp_fft.c unpack.c

OBJS = PitchShift.o psola.o ppsfns.o voicingVerify.o spenvel.o ptracks.o \
	fp_fft.o unpack.o

all: PitchShift

PitchShift: $(OBJS) ../4Bunnell/libcwav.a
	$(CC) -o PitchShift -L ../4Bunnell/ $(OBJS) -lm -lcwav

../4Bunnell/libcwav.a:
	make -C ../4Bunnell -f makefile.lnx

clean:
	rm -f $(OBJS)
