#ifndef ringbuffer
#define ringbuffer

class RingBuffer {
  public:
    RingBuffer(unsigned int size, unsigned int highWaterMark);
    ~RingBuffer();

    int push(short value);
    int pop(short *value);

    unsigned int getSamplesAvailable();
    unsigned int getSpaceAvailable();
    void setHighWaterMark(unsigned int highWaterMark);

    void status();

  private:
    short *_buffer;
    int _head;
    int _tail;
    unsigned int _size;
    unsigned int _highWaterMark;
};

#endif /* ringbuffer */
