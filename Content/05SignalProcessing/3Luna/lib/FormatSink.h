////////////////////////////////////////////////////////////////////////////////
//
// FormatSink.h
//
// Format detected M-ary symbols into data bytes
//
////////////////////////////////////////////////////////////////////////////////

class FormatSink {
 public:
    FormatSink():
	m_stream(0L),
	m_streamLen(0),
	m_symbols(0L),
	m_streamSymbolCount(0),
	m_k(1) {}

    ~FormatSink();

    // set the symbol stream to be formatted and the number of bits
    // per symbol.  There are M = 2^^k symbols in the symbol set, aka
    // alphabet. 0 < k <= 8 for simplicity.
    void SetSymbols(unsigned char *symbols, unsigned symbolCount, unsigned k);

    // return the ith data byte.
    unsigned char GetDataByte(unsigned i);

    // return the number of data bytes in the stream
    unsigned GetDataByteCount();

    // return the ith data as a short.
    short GetDataShort(unsigned i);

    // return the number of data shorts in the stream
    unsigned GetDataShortCount();

    // return the ith data as a int.
    int GetDataInt(unsigned i);

    // return the number of data ints in the stream
    unsigned GetDataIntCount();

    // return the ith data as a float.
    float GetDataFloat(unsigned i);

    // return the number of data floats in the stream
    unsigned GetDataFloatCount();

    // Built-in test routine.
    void Test();

 private:
    unsigned char *m_stream;
    unsigned char *m_symbols;
    unsigned       m_streamLen;
    unsigned       m_streamSymbolCount;
    unsigned  m_k;

    // populate the data byte array if we have an active message
    // symbol stream.
    void UpdateDataBytes();

    // pulls out the [msb,lsb] range of bits from x and returns them right
    // adjusted
    unsigned char GetBits(unsigned char x, unsigned msb, unsigned lsb);
};
