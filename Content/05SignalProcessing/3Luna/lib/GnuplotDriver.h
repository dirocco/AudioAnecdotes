//////////////////////////////////////////////////////////////////////////////// 
//
//   GnuplotDriver.h
//
//   Drive gnuplot from C++.  Currently only supports win32, but can be easily
//   exteneded for other gnuplot platforms.
//
//   Adapted from various postings to comp.graphics.apps.gnuplot.
//
//////////////////////////////////////////////////////////////////////////////// 

#ifndef _GnuplotDriver_h_
#define _GnuplotDriver_h_

#include <stdio.h>

#define GNUPLOT_PROGRAM "pgnuplot"

// #if defined(WIN32)
// #define GNUPLOT_PROGRAM "pgnuplot.exe"
// #else
// //#define GNUPLOT_PROGRAM "gnuplot"
// #define GNUPLOT_PROGRAM "pgnuplot.exe"
// #endif

#define GNUPLOT_MAX_STREAMS     15
#define GNUPLOT_MAX_TITLE_LEN   80

// can fit this to a particular application's needs
#define GNUPLOT_MAX_DATA_LEN  8196 //1024

class GnuplotDriver {
public:
    bool Init(char *gnuplotExecutable = GNUPLOT_PROGRAM);
    ~GnuplotDriver();

    //
    //  Note: single gnuplot commands spanning multiple
    //  GnuplotCommand() calls is supported.
    //

    // send complex gnuplot commands ala printf
    void GnuplotCommand(const char *fmt, ...);

    //
    // simplify making a single plot with several data streams.
    //

    // Y vs X plots
    void AppendStreamData(const char *id, float x, float y);

    // automatic X scale determined by gnuplot
    void AppendStreamData(const char *id, float y);

    // additional options for this id's data plot.  For example:
    // SetStreamDataPlotOptions("plot 1", "with impulses");
    // cause plot 1's data to be drawn as impulses.
    void SetStreamDataPlotOptions(const char *id, const char *options);

    // plot gathered data
    void PlotStreams();

 private:
    // file pointer to pipe for gnuplot
    FILE *m_pPipe;

    // current length of each data stream.
    unsigned m_uStreamDataLength[GNUPLOT_MAX_STREAMS];

    // list of plot IDs
    char m_sIDList[GNUPLOT_MAX_STREAMS][GNUPLOT_MAX_TITLE_LEN];

    // list of plot options
    char m_sStreamDataPlotOptionsList[GNUPLOT_MAX_STREAMS][GNUPLOT_MAX_TITLE_LEN];

    // number of data streams in use.
    unsigned m_uNumStreams;

    // allocate data storage as streams are added.
    float *m_fStreamDataX[GNUPLOT_MAX_STREAMS];
    float *m_fStreamDataY[GNUPLOT_MAX_STREAMS];

    unsigned GetIDIndex(const char *id);
};

#endif _GnuplotDriver_h_
