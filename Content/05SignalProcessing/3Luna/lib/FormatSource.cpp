////////////////////////////////////////////////////////////////////////////////
//
// FormatSource.h
//
// Format M-ary symbols suitable for modulation
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "FormatSource.h"

FormatSource::~FormatSource() {
    // free symbol buffer
    if (0L != m_symbols) {
        delete [] m_symbols;
        m_symbols = 0L;
    }
    // free string buffer
    if (0L != m_stream) {
        delete [] m_stream;
        m_stream = 0L;
    }

    m_streamSymbolCount = 0;
    m_streamLen = 0;
} // FormatSource::~FormatSource()

// set the number of bits per symbol.  There are M = 2^^k symbols
// in the symbol set, aka alphabet.  0 < k <= 8 for simplicity.
void FormatSource::Setk(unsigned k) {
    // validate k
    if (0 == k || k > 8) {
        return;
    }           

    m_k = k;

    UpdateSymbolData();
} // FormatSource::Setk()

// set the stream to be formatted
void FormatSource::SetStream(char *stream) {
    if (0L == stream) {
        return;
    }

    // free old string buffer
    if (0L != m_stream) {
        delete [] m_stream;
    }

    m_streamLen = strlen(stream);
    m_stream = new unsigned char [m_streamLen];

    for (unsigned i = 0; i < m_streamLen; i++) {
        m_stream[i] = (unsigned char)stream[i];
    }

    UpdateSymbolData();
} // FormatSource::SetStream()

// format a short int.  Convert to a string of 2 unsigned chars
void FormatSource::SetDataShort(short data) {
    // free old string buffer
    if (0L != m_stream) {
        delete [] m_stream;
    }

    m_streamLen = sizeof(short);
    m_stream = new unsigned char [m_streamLen];

    for (unsigned i = 0; i < m_streamLen; i++) {
        m_stream[i] = GetBits(unsigned(data), 7+8*i, 0+8*i);
    }

    UpdateSymbolData();
} // FormatSource::SetDataShort()

// format an int.  Convert to a string of 4 unsigned chars
void FormatSource::SetDataInt(int data) {
    // free old string buffer
    if (0L != m_stream) {
        delete [] m_stream;
    }

    m_streamLen = sizeof(int);
    m_stream = new unsigned char [m_streamLen];

    for (unsigned i = 0; i < m_streamLen; i++) {
        m_stream[i] = GetBits(unsigned(data), 7+8*i, 0+8*i);
    }

    UpdateSymbolData();
} // FormatSource::SetDataInt()

// format a float.  Convert to a string of 4 unsigned chars
void FormatSource::SetDataFloat(float data) {
    // free old string buffer
    if (0L != m_stream) {
        delete [] m_stream;
    }

    // ensure we can cast data to unsigned for bit ops
    assert(sizeof(unsigned) >= sizeof(float));

    m_streamLen = sizeof(float);
    m_stream = new unsigned char [m_streamLen];

    for (unsigned i = 0; i < m_streamLen; i++) {
        m_stream[i] = GetBits(*(unsigned *)&data, 7+8*i, 0+8*i);
    }

    UpdateSymbolData();
} // FormatSource::SetDataFloat()


// return the ith message symbol.
unsigned char FormatSource::GetMessageSymbol(unsigned i) {
    if (0L != m_symbols &&
        i < m_streamSymbolCount)
        {
            return m_symbols[i];
        }
    else {
        // error condition: return 0xff.
        return 0xff;
    }
} // FormatSource::GetMessageSymbol()

// return the number of  message symbols in the stream
unsigned FormatSource::GetMessageSymbolCount() {
    return m_streamSymbolCount;
} // FormatSource::GetMessageSymbolCount()

// populate the symbol array if we have an active stream.
void FormatSource::UpdateSymbolData() {
    // if no stream, then we are done
    if (0L == m_stream) {
        return;
    }

    // delete old symbols
    if (0L != m_symbols) {
        delete [] m_symbols;
    }

    // compute the number of symbols
    m_streamSymbolCount = m_streamLen*8 / m_k;
    unsigned mod = m_streamLen*8 % m_k;
    if (mod) {
        m_streamSymbolCount++;
    }

    m_symbols = new unsigned char [m_streamSymbolCount];

    if (0L == m_symbols) {
        m_streamSymbolCount = 0;
        return;
    }

    memset(m_symbols, 0, m_streamSymbolCount);

    unsigned istream; // current stream index
    unsigned isymbol; // current symbol index

    // particular byte of the stream from which we are gathering bits
    unsigned firstStreamByte;

    // when we need bits from two consecutive bytes to make up a symbol
    unsigned nextStreamByte;

    // the range of bits in the bytes from which we are forming the
    // symbol.  If nextStreamByte != firstStreamByte, then the usage
    // for lastStreamBit changes slightly.
    unsigned firstStreamBit;
    unsigned lastStreamBit;

    for (istream = 0, isymbol = 0; istream < m_streamLen; isymbol++) {
        firstStreamByte =  m_k*isymbol/8;
        nextStreamByte  = (m_k*(isymbol+1)-1)/8;

        firstStreamBit  =  7 - (m_k*isymbol % 8);
        lastStreamBit   =  7 - (m_k*(isymbol+1)-1) % 8;

        if (firstStreamByte == nextStreamByte) {
            
            // GetBits() pulls out the [msb,lsb] range of bits from a
            // byte and returns them right adjusted
            m_symbols[isymbol] = GetBits(m_stream[istream],
                                         firstStreamBit,
                                         lastStreamBit);

            // move onto next stream byte
            if (0 == lastStreamBit) { istream++; }
        }
        else {
            m_symbols[isymbol] = GetBits(m_stream[istream],
                                         firstStreamBit, 0);

            // For the last symbol, this can take us off the end of
            // the stream, so we check for that here.
            if ((istream+1) < m_streamLen) {
                m_symbols[isymbol] <<= 8-lastStreamBit;
                m_symbols[isymbol] |=  GetBits(m_stream[istream+1],
                                               7, lastStreamBit);
            }

            // move onto next stream byte
            istream++;
        }
    }
} // FormatSource::UpdateSymbolData()   


// Built-in test routine.
void FormatSource::Test() {
    unsigned int i;
    Setk(0);
    SetStream("ben");
    printf("\n<-----     Begin Testing FormatSource    ----->\nstream:\n");
    printf("              b               e               n\n"); 
    printf("             %2x              %2x              %2x\n", 'b', 'e', 'n'); 
    printf("\nk=0, should map to k=1\n"); 
    for (i = 0; i < GetMessageSymbolCount(); i++) {
        unsigned m = GetMessageSymbol(i);
        printf("%d ", m);
    }
    Setk(1);
    printf("\nk=1\n"); 
    for (i = 0; i < GetMessageSymbolCount(); i++) {
        unsigned m = GetMessageSymbol(i);
        printf("%d ", m);
    }
    printf("\nk=2\n"); 
    Setk(2);
    for (i = 0; i < GetMessageSymbolCount(); i++) {
        unsigned m = GetMessageSymbol(i);
        printf("  %d ", m);
    }
    printf("\nk=3\n"); 
    Setk(3);
    for (i = 0; i < GetMessageSymbolCount(); i++) {
        unsigned m = GetMessageSymbol(i);
        printf("    %d ", m);
    }

    printf("\nk=4\n"); 
    Setk(4);
    for (i = 0; i < GetMessageSymbolCount(); i++) {
        unsigned m = GetMessageSymbol(i);
        printf("     %2x ", m);
    }

    printf("\nk=5\n"); 
    Setk(5);
    for (i = 0; i < GetMessageSymbolCount(); i++) {
        unsigned m = GetMessageSymbol(i);
        printf("       %2x ", m);
    }

    printf("\nk=6\n"); 
    Setk(6);
    for (i = 0; i < GetMessageSymbolCount(); i++) {
        unsigned m = GetMessageSymbol(i);
        printf("         %2x ", m);
    }

    printf("\nk=7\n"); 
    Setk(7);
    for (i = 0; i < GetMessageSymbolCount(); i++) {
        unsigned m = GetMessageSymbol(i);
        printf("           %2x ", m);
    }

    printf("\nk=8\n"); 
    Setk(8);
    for (i = 0; i < GetMessageSymbolCount(); i++) {
        unsigned m = GetMessageSymbol(i);
        printf("             %2x ", m);
    }
    printf("\nk=9, should map to k=8\n"); 
    Setk(9);
    for (i = 0; i < GetMessageSymbolCount(); i++) {
        unsigned m = GetMessageSymbol(i);
        printf("             %2x ", m);
    }
    printf("\n<-----      End Testing FormatSource     ----->\n");
} // FormatSource::Test()

// pulls out the [msb,lsb] range of bits from x and returns them right
// adjusted
unsigned char FormatSource::GetBits(unsigned x, unsigned msb, unsigned lsb) {
    return (x >> lsb) & ~(~0 << (msb-lsb+1));
} // FormatSource::GetBits()

// pulls out the [msb,lsb] range of bits from x and returns them right
// adjusted
unsigned char FormatSource::GetBits(unsigned char x, unsigned msb, unsigned lsb)
{
    return (x >> lsb) & ~(~0 << (msb-lsb+1));
} // FormatSource::GetBits()
