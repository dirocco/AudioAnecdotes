//////////////////////////////////////////////////////////////////////////////// 
//
//   GnuplotDriver.cpp 
//
//   Drive gnuplot from C++.  Currently only supports win32, but can be easily
//   exteneded for other gnuplot platforms.
//
//   Adapted from various postings to comp.graphics.apps.gnuplot.
//
//////////////////////////////////////////////////////////////////////////////// 

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "GnuplotDriver.h"

bool GnuplotDriver::Init(char *gnuplotExecutable) {
    // default length of each data stream is 0
    memset(m_uStreamDataLength, 0, GNUPLOT_MAX_STREAMS*sizeof(unsigned));

    // default plot IDs are ""
    memset(m_sIDList, 0, GNUPLOT_MAX_STREAMS*GNUPLOT_MAX_TITLE_LEN);

    // default plot options are ""
    memset(m_sStreamDataPlotOptionsList, 0, GNUPLOT_MAX_STREAMS*GNUPLOT_MAX_TITLE_LEN);

    // 0 data streams in use.
    m_uNumStreams = 0;
    memset(m_fStreamDataX, 0, GNUPLOT_MAX_STREAMS);
    memset(m_fStreamDataY, 0, GNUPLOT_MAX_STREAMS);

#if 0 // defined(WIN32)
    m_hwndParent = 0;
    m_hwndText = 0;

    // check to see if gnuplot is running and if not start it.
    m_hwndParent = FindWindow((LPSTR)NULL, (LPCTSTR)gnuplotExecutable);
    if (!m_hwndParent) {
	fprintf(stdout, "Can't find gnuplot already running, exec'ing: %s\n", gnuplotExecutable);

	/* load gnuplot
	   To hide gnuplot when starting, try the SW_HIDE flag. */
	if (WinExec(gnuplotExecutable, SW_SHOWNORMAL) <= 32) {
	    fprintf(stderr, "can't load gnuplot\n");
	    return false;
	}

	m_hwndParent = FindWindow((LPSTR)NULL, (LPCTSTR)gnuplotExecutable);
	if (!m_hwndParent) {
	    fprintf(stderr, "can't find the gnuplot window\n");
	    return false;
	}
    }
    /* find the child text window */
    m_hwndText = FindWindowEx(m_hwndParent, (HWND) NULL,
			      (LPCTSTR)"wgnuplot_text",
			      (LPCTSTR) NULL);
    if (!m_hwndText) {
	fprintf(stderr, "can't find wgnuplot_text");
	return false;
    }
    return true;
#else
#ifdef WIN32
    m_pPipe = _popen(gnuplotExecutable, "w") ;
#else
    m_pPipe = popen(gnuplotExecutable, "w") ;
#endif

    if (0L == m_pPipe) {
	fprintf(stderr, "can't find %s in path!\n", gnuplotExecutable);
	return false;
    }
    return true;
#endif
} // GnuplotDriver::Init()

GnuplotDriver::~GnuplotDriver() {
    unsigned i;

    for (i=0; i<GNUPLOT_MAX_STREAMS; i++) {
	if (m_fStreamDataX[i]) {
	    delete [] m_fStreamDataX[i];
	}
	if (m_fStreamDataY[i]) {
	    delete [] m_fStreamDataY[i];
	}
    }

    // siliently fail since we're done with it anyway
#ifdef WIN32
    (void)_pclose(m_pPipe);
#else
    (void)pclose(m_pPipe);
#endif
} // GnuplotDriver::~GnuplotDriver()

//
// Make it easier to send complex gnuplot commands
//
void GnuplotDriver::GnuplotCommand(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    vfprintf(m_pPipe, fmt, args);
    fflush(m_pPipe);
    va_end(args);
} // GnuplotDriver::GnuplotCommand(const char *fmt, ...);

//
// simplify making a single plot with several data streams.
//

// Y vs X plot
void GnuplotDriver::AppendStreamData(const char *id, float x, float y) {
    unsigned uStreamIndex = GetIDIndex(id);
    if (uStreamIndex >= GNUPLOT_MAX_STREAMS) {
	fprintf(stderr,
		"GnuplotDriver error: Reached %d stream limit, cannot plot: '%s'\n",
		GNUPLOT_MAX_STREAMS, id);
	return;
    }

    // check to see if other plot type used for this id
    if (0L != m_fStreamDataY[uStreamIndex] &&
	0L == m_fStreamDataX[uStreamIndex])
    {
	fprintf(stderr,
		"GnuplotDriver warn: automatic X scale plot for '%s' in progress,\n"
		"GnuplotDriver warn: ignoring X value!!",
		id);
	if (++m_uStreamDataLength[uStreamIndex] < GNUPLOT_MAX_DATA_LEN) {
	    m_fStreamDataY[uStreamIndex][m_uStreamDataLength[uStreamIndex]] = y;
	}
	else {
	    --m_uStreamDataLength[uStreamIndex];
	    fprintf(stderr,
		    "GnuplotDriver warn: data length limit(%d) reached for plot:\n"
		    "GnuplotDriver warn: '%s,' ignoring future values.\n",
		    GNUPLOT_MAX_DATA_LEN, id);
	}
	return;
    }
    // new data stream
    if (0L == m_fStreamDataY[uStreamIndex]) {
	m_fStreamDataY[uStreamIndex] = new float[GNUPLOT_MAX_DATA_LEN];
	if (0L == m_fStreamDataY[uStreamIndex]) {
	    fprintf(stderr,
		    "GnuplotDriver error: memory alloc for plot '%s' failed!\n",
		    id);
	    return;
	}
    }
    if (0L == m_fStreamDataX[uStreamIndex]) {
	m_fStreamDataX[uStreamIndex] = new float[GNUPLOT_MAX_DATA_LEN];
	if (0L == m_fStreamDataX[uStreamIndex]) {
	    fprintf(stderr,
		    "GnuplotDriver error: memory alloc for plot '%s' failed!\n",
		    id);
	    return;
	}
    }
    if (++m_uStreamDataLength[uStreamIndex] <= GNUPLOT_MAX_DATA_LEN) {
	m_fStreamDataX[uStreamIndex][m_uStreamDataLength[uStreamIndex]-1] = x;
	m_fStreamDataY[uStreamIndex][m_uStreamDataLength[uStreamIndex]-1] = y;
    }
    else {
	--m_uStreamDataLength[uStreamIndex];
	fprintf(stderr,
		"GnuplotDriver warn: data length limit(%d) reached for plot:\n"
		"GnuplotDriver warn: '%s,' ignoring future values.\n",
		GNUPLOT_MAX_DATA_LEN, id);
    }
} // GnuplotDriver::AppendStreamData()

// automatic X scale determined by gnuplot
void GnuplotDriver::AppendStreamData(const char *id, float y) {
    unsigned uStreamIndex = GetIDIndex(id);
    if (uStreamIndex >= GNUPLOT_MAX_STREAMS) {
	fprintf(stderr,
		"GnuplotDriver error: Reached %d stream limit, cannot plot: '%s'\n",
		GNUPLOT_MAX_STREAMS, id);
	return;
    }

    // check to see if other plot type used for this id
    if (0L != m_fStreamDataX[uStreamIndex]) {
	fprintf(stderr,
		"GnuplotDriver warn: Y vs X plot for '%s' in progress,\n"
		"GnuplotDriver warn: no data being updated!!",
		id);
	return;
    }

    // new data stream
    if (0L == m_fStreamDataY[uStreamIndex]) {
	m_fStreamDataY[uStreamIndex] = new float[GNUPLOT_MAX_DATA_LEN];
	if (0L == m_fStreamDataY[uStreamIndex]) {
	    fprintf(stderr,
		    "GnuplotDriver error: memory alloc for plot '%s' failed!\n",
		    id);
	    return;
	}
    }
    if (++m_uStreamDataLength[uStreamIndex] <= GNUPLOT_MAX_DATA_LEN) {
	m_fStreamDataY[uStreamIndex][m_uStreamDataLength[uStreamIndex]-1] = y;
    }
    else {
	--m_uStreamDataLength[uStreamIndex];
	fprintf(stderr,
		"GnuplotDriver warn: data length limit(%d) reached for plot:\n"
		"GnuplotDriver warn: '%s,' ignoring future values.\n",
		GNUPLOT_MAX_DATA_LEN, id);
    }
} // GnuplotDriver::AppendStreamData()

// additional options for this id's data plot.  For example:
// SetStreamDataPlotOptions("plot 1", "with impulses");
// cause plot 1's data to be drawn as impulses.
void GnuplotDriver::SetStreamDataPlotOptions(const char *id, const char *options) {
    unsigned uStreamIndex = GetIDIndex(id);
    if (uStreamIndex >= GNUPLOT_MAX_STREAMS) {
	fprintf(stderr,
		"GnuplotDriver error: Reached %d stream limit, cannot plot: '%s'\n",
		GNUPLOT_MAX_STREAMS, id);
	return;
    }
    strcpy(m_sStreamDataPlotOptionsList[uStreamIndex], options);
} // GnuplotDriver::SetStreamDataPlotOptions()

// plot gathered data
void GnuplotDriver::PlotStreams() {
    unsigned i, j;

    // if we don't have any data, we're already done.
    if (0 == m_uNumStreams) {
	fprintf(stderr, "GnuplotDriver: No data to plot!\n");
	return;
    }
	
#if 1
    //
    // use data files for plot printing.  Sometime sending data via events
    // to a running gnuplot process does not work
    //

    // print the data files...
    for (i = 0; i < m_uNumStreams; i++) {
	char filename[2048];
	sprintf(filename, "data/%d.dat", i);
	FILE *fp = fopen(filename, "w");

	if (0L != fp) {
	    if (0L == m_fStreamDataX[i]) {
		for (j = 0; j < m_uStreamDataLength[i]; j++) {
		    fprintf(fp, "%f\r\n", m_fStreamDataY[i][j]);
		}
	    }
	    // Y vs X plot
	    else {
		for (j = 0; j < m_uStreamDataLength[i]; j++) {
		    fprintf(fp, "%f %f\r\n", m_fStreamDataX[i][j], m_fStreamDataY[i][j]);
		}
	    }
	    (void)fclose(fp);
	}
    }
    // ... and send the plot command 
    GnuplotCommand("plot \"data/%d.dat\" title \"%s\" %s", 0, m_sIDList[0],
		   m_sStreamDataPlotOptionsList[0]);
    for (i = 1; i < m_uNumStreams; i++) {
	GnuplotCommand(", \"data/%d.dat\" title \"%s\" %s", i, m_sIDList[i],
		       m_sStreamDataPlotOptionsList[i]);
    }
    GnuplotCommand("\r\npause -1 \"waiting...\"\r\n");
#else
    //
    // use IPC to send plot data to running gnuplot process
    //
	
    // formulate the plot command ...
    GnuplotCommand("plot '-' title \"%s\" %s", m_sIDList[0],
		   m_sStreamDataPlotOptionsList[0]);
    for (i = 1; i < m_uNumStreams; i++) {
	GnuplotCommand(", '-' title \"%s\" %s", m_sIDList[i],
		       m_sStreamDataPlotOptionsList[i]);
    }
    GnuplotCommand("\n");

    // ...and then print the data files
    for (i = 0; i < m_uNumStreams; i++) {
	char filename[MAX_NAME_LEN];
	sprintf(filename, "%s.dat", m_sIDList[i]);
	FILE *fp = fopen(filename, "w");
		
	if (0L == m_fStreamDataX[i]) {
	    for (j = 0; j < m_uStreamDataLength[i]; j++) {
		GnuplotCommand("%f\n", m_fStreamDataY[i][j]);
		Sleep(10);
	    }
	}
	// Y vs X plot
	else {
	    for (j = 0; j < m_uStreamDataLength[i]; j++) {
		GnuplotCommand("%f %f\n", m_fStreamDataX[i][j], m_fStreamDataY[i][j]);
		Sleep(10);
	    }
	}

	// end of data for ith stream.
	GnuplotCommand("e\n");
    }
#endif
} // GnuplotDriver::PlotStreams()

unsigned GnuplotDriver::GetIDIndex(const char *id) {
    unsigned uStreamIndex;

    for (uStreamIndex = 0; uStreamIndex < m_uNumStreams; uStreamIndex++) {
	if (0 == strcmp(id, m_sIDList[uStreamIndex])) {
	    break;
	}
    }

    // stream id not found
    if (uStreamIndex == m_uNumStreams) {
	// add stream id if we are under the limit
	if (uStreamIndex < GNUPLOT_MAX_STREAMS) {
	    strcpy(m_sIDList[uStreamIndex], id);
	    m_uNumStreams++;
	}
    }
    return uStreamIndex;
} // GnuplotDriver::GetIDIndex()
