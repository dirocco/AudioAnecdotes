FIGURES = sample1 sample2 difference error-noisy error-accurate rounding-noisy rounding-accurate

PNGFILES = $(addsuffix .png,$(FIGURES))
PSFILES = $(addsuffix .ps,$(FIGURES))
EPSFILES = $(addsuffix .eps,$(FIGURES))
PDFFILES = $(addsuffix .pdf,$(FIGURES))

.PHONY: all ps eps pdf clean clean_all

all : ps eps pdf

png : $(PNGFILES)
ps : $(PSFILES)
eps : $(EPSFILES)
pdf : $(PDFFILES)

clean:
	rm -f $(PSFILES) $(PDFFILES) $(PNGFILES)

clean_all : clean
	rm -f $(PNGFILES) $(EPSFILES) $(PDFFILES)

$(PSFILES) : makesamples.m
	echo 'makesamples("ps","postscript");' | octave

$(PNGFILES) : makesamples.m
	echo 'makesamples("png","png color small");' | octave

%.pdf : %.ps
	rm -f $@
	ps2pdf $< $@

%.eps : %.ps
	rm -f $@
	ps2epsi $< $@
