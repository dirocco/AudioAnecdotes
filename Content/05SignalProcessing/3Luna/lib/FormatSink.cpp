////////////////////////////////////////////////////////////////////////////////
//
// FormatSink.h
//
// Format detected M-ary symbols into data bytes
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "FormatSink.h"

FormatSink::~FormatSink() {
    // free symbol buffer
    if (0L != m_symbols) {
        delete [] m_symbols;
        m_symbols = 0L;
    }
    // free data byte stream buffer
    if (0L != m_stream) {
        delete [] m_stream;
        m_stream = 0L;
    }

    m_streamSymbolCount = 0;
    m_streamLen = 0;
} // FormatSink::~FormatSink()

// set the symbol stream to be formatted and the number of bits
// per symbol.  There are M = 2^^k symbols in the symbol set, aka
// alphabet. 0 < k <= 8 for simplicity.
void FormatSink::SetSymbols(unsigned char *symbols,
                            unsigned symbolCount,
                            unsigned k)
{
    // validate symbols pointer
    if (0L == symbols) {
        return;
    }
    // validate k
    if (0 == k || k > 8) {
        return;
    }       
    m_k = k;

    // free old string buffer
    if (0L != m_symbols) {
        delete [] m_symbols;
    }

    m_streamSymbolCount = symbolCount;
    m_symbols = new unsigned char [m_streamSymbolCount];
    if (0L == m_symbols) {
        m_streamSymbolCount = 0;
        return;
    }

    for (unsigned i = 0; i < m_streamSymbolCount; i++) {
        m_symbols[i] = symbols[i];
    }

    UpdateDataBytes();
} // FormatSink::SetSymbols()

// return the ith data byte.
unsigned char FormatSink::GetDataByte(unsigned i) {
    if (0L != m_stream &&
        i < m_streamLen)
        {
            return m_stream[i];
        }
    else {
        // error condition: return 0xff.
        return 0xff;
    }
} // FormatSink::GetDataByte()

// return the number of data bytes in the stream
unsigned FormatSink::GetDataByteCount() {
    return m_streamLen;
} // FormatSink::GetDataByteCount()


// return the ith data as a short.
short FormatSink::GetDataShort(unsigned i) {
    assert(sizeof(short) == 2);
    unsigned idx = i*2;
    unsigned result = SHRT_MAX;
    if (0L != m_stream) {
        // if data bytes received don't fill a short, pad with zeros
        if (idx+1 < m_streamLen)
            {
                result  = unsigned(m_stream[idx]);
                result |= unsigned(m_stream[idx+1]) << 8;
            }
        else if (idx < m_streamLen) {
            result  = unsigned(m_stream[idx]);
        }
    }
    // error condition: return 0xffff.
    return short(result);
} // FormatSink::GetDataShort()

// return the number of data shorts in the stream
unsigned FormatSink::GetDataShortCount() {
    unsigned count = m_streamLen / sizeof(short);
    return (m_streamLen % sizeof(short) ? count+1: count);
} // FormatSink::GetDataShortCount()

// return the ith data as a int.
int FormatSink::GetDataInt(unsigned i) {
    assert(sizeof(int) == 4);
    unsigned idx = i*4;
    unsigned result = INT_MAX;
    if (0L != m_stream) {
        // if data bytes received don't fill an int, pad with zeros
        if (idx+3 < m_streamLen)
            {
                result  = unsigned(m_stream[idx]);
                result |= unsigned(m_stream[idx+1]) << 8;
                result |= unsigned(m_stream[idx+2]) << 16;
                result |= unsigned(m_stream[idx+3]) << 24;
            }
        else if (idx+2 < m_streamLen) {
            result  = unsigned(m_stream[idx]);
            result |= unsigned(m_stream[idx+1]) << 8;
            result |= unsigned(m_stream[idx+2]) << 16;
        }
        else if (idx+1 < m_streamLen) {
            result  = unsigned(m_stream[idx]);
            result |= unsigned(m_stream[idx+1]) << 8;
        }
        else if (idx < m_streamLen) {
            result  = unsigned(m_stream[idx]);
        }
    }
    return int(result);
} // FormatSink::GetDataInt()

// return the number of data ints in the stream
unsigned FormatSink::GetDataIntCount() {
    unsigned count = m_streamLen / sizeof(int);
    return (m_streamLen % sizeof(int) ? count+1: count);
} // FormatSink::GetDataIntCount()


// return the ith data as a float.
float FormatSink::GetDataFloat(unsigned i) {
    assert(sizeof(float) == 4);
    unsigned idx = i*4;
    if (0L != m_stream) {
        // make no attempt to fill out incomplete floating point parts,
        // just return 0
        if (idx+3 < m_streamLen)
            {
                unsigned result;
                result  = unsigned(m_stream[idx]);
                result |= unsigned(m_stream[idx+1]) << 8;
                result |= unsigned(m_stream[idx+2]) << 16;
                result |= unsigned(m_stream[idx+3]) << 24;
                return *(float *)&result;
            }
    }
    return 0.0f;
} // FormatSink::GetDataFloat()

// return the number of data floats in the stream
unsigned FormatSink::GetDataFloatCount() {
    unsigned count = m_streamLen / sizeof(float);
    return (m_streamLen % sizeof(float) ? count+1: count);
} // FormatSink::GetDataFloatCount()

// populate the symbol array if we have an active stream.
void FormatSink::UpdateDataBytes() {
    // if no stream, then we are done
    if (0L == m_symbols) {
        return;
    }

    // delete old data byte stream
    if (0L != m_stream) {
        delete [] m_stream;
    }

    // compute the number of symbols
    m_streamLen = m_streamSymbolCount * m_k / 8;
    unsigned mod = m_streamSymbolCount * m_k % 8;
    if (mod) {
        m_streamLen++;
    }

    m_stream = new unsigned char [m_streamLen];
    if (0L == m_stream) {
        m_streamLen = 0;
        return;
    }
    memset(m_stream, 0, m_streamLen);

    unsigned istream, isymbol;

    // bits left in the byte to populate
    int bitsLeft = 0;

    // bits already populated into the byte
    int bitsUsed = 0;

    for (istream = 0, isymbol = 0; isymbol < m_streamSymbolCount; isymbol++) {
        bitsLeft = 8 - (m_k + bitsUsed);

        // there room in the current data byte for all the current
        // symbols bits.
        if (bitsLeft >= 0) {
            m_stream[istream] <<= m_k;
            m_stream[istream] |= m_symbols[isymbol];
            bitsUsed += m_k;

            if (0 == bitsLeft) {
                bitsUsed = 0;
                istream++;
            }
        }

        // more bits in the current symbol than remaining to be
        // populated in the data byte, so bitsLeft is negative.
        // Populate the remains the current data byte and then shove
        // the left over symbol bits into the next data byte;
        else {
            m_stream[istream] <<= m_k + bitsLeft;
            unsigned tst = GetBits(m_symbols[isymbol], m_k-1, 0-bitsLeft);
            m_stream[istream] |= GetBits(m_symbols[isymbol], m_k-1, 0-bitsLeft);
            istream++;
            bitsUsed = 0-bitsLeft;
            m_stream[istream] |= GetBits(m_symbols[isymbol], bitsUsed-1,0);
        }
    }
} // FormatSink::UpdateDataBytes()  

// Built-in test routine.
void FormatSink::Test() {
    unsigned char symbolskEqual1[] = {
        0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0
    };
    unsigned char symbolskEqual2[] = {
        1,    2,    0,    2,    1,    2,    1,    1,    1,    2,    3,    2
    };
    unsigned char symbolskEqual3[] = {
        3,       0,       4,       6,       2,       5,       5,       6
    };
    unsigned char symbolskEqual4[] = {
        6,          2,          6,          5,          6,        0xe
    };
    unsigned char symbolskEqual5[] = {
        0xc,             0x9,            0x12,            0x16,     0x1c,
    };
    unsigned char symbolskEqual6[] = {
        0x18,             0x26,             0x15,             0x2e
    };
    unsigned char symbolskEqual7[] = {
        0x31,            0x19,            0x2d,            0x60
    };
    unsigned char symbolskEqual8[] = {
        0x62,                  0x65,                  0x6e
    };

    unsigned int i;
    SetSymbols(symbolskEqual1, 24, 0);
    printf("\n<-----     Begin Testing FormatSink    ----->\nstream:\n");
    printf("              b               e               n\n"); 
    printf("0 1 1 0 0 0 1 0 0 1 1 0 0 1 0 1 0 1 1 0 1 1 1 0\n"); 
    printf("\nk=0, should map to k=1\n"); 
    for (i = 0; i < GetDataByteCount(); i++) {
        unsigned char b = GetDataByte(i);
        printf("              %c ", (char)b);
    }
    SetSymbols(symbolskEqual1, 24, 1);
    printf("\nk=1\n"); 
    for (i = 0; i < GetDataByteCount(); i++) {
        unsigned char b = GetDataByte(i);
        printf("              %c ", (char)b);
    }
    printf("\nk=2\n"); 
    SetSymbols(symbolskEqual2, 12, 2);
    for (i = 0; i < GetDataByteCount(); i++) {
        unsigned char b = GetDataByte(i);
        printf("              %c ", (char)b);
    }
    printf("\nk=3\n"); 
    SetSymbols(symbolskEqual3, 8, 3);
    for (i = 0; i < GetDataByteCount(); i++) {
        unsigned char b = GetDataByte(i);
        printf("              %c ", (char)b);
    }

    printf("\nk=4\n"); 
    SetSymbols(symbolskEqual4, 6, 4);
    for (i = 0; i < GetDataByteCount(); i++) {
        unsigned char b = GetDataByte(i);
        printf("              %c ", (char)b);
    }

    printf("\nk=5\n"); 
    SetSymbols(symbolskEqual5, 5, 5);
    for (i = 0; i < GetDataByteCount(); i++) {
        unsigned char b = GetDataByte(i);
        printf("              %c ", (char)b);
    }

    printf("\nk=6\n"); 
    SetSymbols(symbolskEqual6, 4, 6);
    for (i = 0; i < GetDataByteCount(); i++) {
        unsigned char b = GetDataByte(i);
        printf("              %c ", (char)b);
    }

    printf("\nk=7\n"); 
    SetSymbols(symbolskEqual7, 4, 7);
    for (i = 0; i < GetDataByteCount(); i++) {
        unsigned char b = GetDataByte(i);
        printf("              %c ", (char)b);
    }

    printf("\nk=8\n"); 
    SetSymbols(symbolskEqual8, 3, 8);
    for (i = 0; i < GetDataByteCount(); i++) {
        unsigned char b = GetDataByte(i);
        printf("              %c ", (char)b);
    }
    printf("\nk=9, should map to k=8\n"); 
    SetSymbols(symbolskEqual8, 3, 9);
    for (i = 0; i < GetDataByteCount(); i++) {
        unsigned char b = GetDataByte(i);
        printf("              %c ", (char)b);
    }
    printf("\n<-----      End Testing FormatSink     ----->\n");
} // FormatSink::Test()

// pulls out [msb,lsb] range of bits from x and returns them right adjusted
unsigned char FormatSink::GetBits(unsigned char x, unsigned msb, unsigned lsb) {
    return (x >> lsb) & ~(~0 << (msb-lsb+1));
} // FormatSink::GetBits()
