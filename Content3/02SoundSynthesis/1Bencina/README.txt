Ross Bencina's Granular Synthesis Toolkit (RB-GST)
Copyright (C) 2001 Ross Bencina
email: rossb@audiomulch.com
web: http://www.audiomulch.com/~rossb/

RB-GST is distributed under the terms of the GNU General Public License.
See the file COPYING for details.
------------------------------------------------------------------------------

Version of September 7 2001.

This package contains an implementation of the Granular Synthesis Architecture
described in the artice "Implementing Real-Time Granular Synthesis" by
Ross Bencina which is to be included in a new book entitled "Audio Anecdotes",
ed. Ken Greenbaum, published by A.K. Peters.


WHAT'S INCLUDED

This distribution includes a set of portable C++ header files which constitue  
the reusable portion of the toolkit, additionally the file Random.cpp defines  
static state needed by the pseudo random number generators. Also included is a
demonstration program (Demo.cpp) and a set of script files (*.data) which are
used by the demonstration program. A version of the demonstration program
compiled for the Win32 environment named Demo.exe is also included.


WHAT'S NOT INCLUDED

The real-time demonstration program contained in Demo.cpp uses the PABLIO
blocking audio library which is part of the PortAudio portable real-time audio
library, available as open source software from http://www.portaudio.com/


SHORTCOMINGS

This release has the following shortcomings:

- Although a precompiled Win32 executable (Demo.exe) and a project file for
  Borland C++ Builder 4.0 are included (BcbProject.bpr, BcbProject.cpp), 
  a portable Makefile has not been provided with this release. 

- Sample.h which is used for Stored Sample Granular Synthesis is not endian-portable. 
  It requires sample files that have the same byte order as the target machine.

- A better sound file should be used for test.pcm which is used as the source 
  sound for the StochasticStoredSampleGranulator demonstration.

- The delay tap code in DelayLine.h is not as efficient as it could be, it
  should take advantage of the boundary check hoisting technique described
  in the article.


BUILDING THE DEMONSTRATION PROGRAM

To build the demonstration program you will need to obtain and compile
the PortAudio and PABLIO libraries for your platform. These are available at
http://www.portaudio.com/ This package was tested with PortAudio release 15.

The demonstration program requires the following C++ files to be compiled
and linked: Random.cpp, Demo.cpp, in addition to the PortAudio library
(for example the Win32 executable was built with ringbuffer.c, pablio.c,
pa_lib.c and pa_win_wmme.c from the PortAudio distribution.)

For efficiency you will need to use maximum compiler optimization and 
inlining. The code has been written in a style which assumes a good inlining
compiler is available. Define the NDEBUG preprocessor variable to disable 
asserts, as they are used extensively.


USING THE DEMONSTRATION PROGRAM

The demonstration program can perform three forms of granular synthesis
(FM (fm), StoredSample (sample), and DelayLine (dl) granular synthesis) and
FOF (fof) formant synthesis. To use the default demonstration scripts,
simply run the program specifying the desired type of synthesis,
for example:

>demo fm

>demo sample

>demo dl

>demo fof

These commands will use the supplied demonstration scripts fm.data, 
sample.data, dl.data and fof.data respectively. The demonstration scripts 
are editable plain text. The format of these script files is described
in HackedUpParameterSequencer.h. The parameters available for each type of 
synthesis are indicated in Demo.cpp. The demonstration program can also be
used with user supplied script files, for example:

>demo fof myfofscript.data


#EOF
