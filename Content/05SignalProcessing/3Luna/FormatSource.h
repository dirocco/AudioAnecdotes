// -*- C++ -*-	
////////////////////////////////////////////////////////////////////////////////
//
// FormatSource.h
//
// Format data bytes into M-ary symbols suitable for modulation
//
////////////////////////////////////////////////////////////////////////////////

class FormatSource {
public:
    FormatSource():
	m_stream(0L),
	m_streamLen(0),
	m_symbols(0L),
	m_streamSymbolCount(0),
	m_k(1) {}

    ~FormatSource();

    // set the number of bits per symbol.  There are M = 2^^k symbols
    // in the symbol set, aka alphabet. 0 < k <= 8 for simplicity.
    void Setk(unsigned k);

    // set the stream to be formatted using a zero terminated string.
    void SetStream(char *stream);

    // format a short int
    void SetDataShort(short data);

    // format an int
    void SetDataInt(int data);

    // format a float
    void SetDataFloat(float data);

    // return the ith message symbol.
    unsigned char GetMessageSymbol(unsigned i);

    // return the number of  message symbols in the stream
    unsigned GetMessageSymbolCount();

    // Built-in test routine.
    void Test();

private:
    unsigned char *m_stream;
    unsigned       m_streamLen;
    unsigned char *m_symbols;
    unsigned       m_streamSymbolCount;
    unsigned       m_k;

    // populate the symbol array if we have an active stream.
    void UpdateSymbolData();

    // pulls out the [msb,lsb] range of bits from x and returns them right
    // adjusted
    unsigned char GetBits(unsigned x, unsigned msb, unsigned lsb);
    unsigned char GetBits(unsigned char x, unsigned msb, unsigned lsb);
};
