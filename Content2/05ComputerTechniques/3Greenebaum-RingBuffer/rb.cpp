#include <stdio.h>
//#include <stdlib.h>    // malloc/free
#include "ringbuffer.h"
#include "math.h"

int min(int a, int b)
{
   return((a<b)?a:b);
}


RingBuffer::RingBuffer(unsigned int size, unsigned int highWaterMark) : 
_size(size), _highWaterMark(highWaterMark), _head(0), _tail(0)
{
   _buffer = new short[_size]; // allocate buffer

   // ensure legal size
   _highWaterMark = (_highWaterMark>_size)?_size:_highWaterMark;
}


RingBuffer::~RingBuffer()
{
}


int RingBuffer::push(short value)
{
   int status = 1;

   if(getSpaceAvailable()) {
      // next two operations do NOT have to be atomic!
      // do NOT have to worry about collision with _tail
   
      _buffer[_tail] = value;   // store value
      _tail = ++_tail % _size;  // increment tail
   } else {
      // block if we were theaded
      printf("push(%d) failed, buffer full\n", value); 
      status = 0;
   }

   return(status);
}


int RingBuffer::pop(short *value)
{
   int status = 1;

   if(getSamplesAvailable()) {
      *value = _buffer[_head];
      _head = ++_head % _size;  // increment head
   } else {
      status = 0;
      printf("pop failed, buffer empty\n");
      // block this thread on underflow
   }
   
   return(status);
}


unsigned int RingBuffer::getSamplesAvailable()
{
   int count = (_tail - _head + _size) % _size;

   return(count);
}


unsigned int RingBuffer::getSpaceAvailable()
{
   int free = (_head - _tail + _size - 1)%_size;
   int underMark = _highWaterMark - getSamplesAvailable();
   int spaceAvailable = min(underMark, free);

   return(spaceAvailable);
}


void RingBuffer::setHighWaterMark(unsigned int highWaterMark)
{
   if(highWaterMark > _size)
      throw("highWaterMark > buffer size");

   _highWaterMark = highWaterMark;
}


void RingBuffer::status()
{
   printf("h(%2d), t(%2d), full(%2d), fillable(%2d)\n",
	 _head, _tail, getSamplesAvailable(), getSpaceAvailable());
}


int main() 
{
   RingBuffer rb(5, 3);
   char string[100];
   short value;
   int intValue;
   int done = 0;

   while(!done) {
      scanf("%s", &string);
      switch(string[0]) {
	 case 'p': rb.pop(&value); printf("popped %4d: ", value); break;
	 case 'q': done = 1;       printf("quitting: ");          break;
	 case 's':                 printf("status:     ");        break;
         default:
	    sscanf(string, "%d", &intValue);
	    rb.push((short)intValue);
	    printf("pushed %4d: ", intValue);
	 break;
      }
      rb.status();
   }

#ifdef PO
   rb.status();
   rb.push(1);
   rb.status();
   rb.push(2);
   rb.status();
   rb.push(3);
   rb.status();
   rb.push(4);
   rb.status();
   rb.push(5);
   rb.status();
#endif

   return 0;
}
