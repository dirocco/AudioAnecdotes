const double period = 1.0; // play tone every N seconds
double bufferTime = SizeOfBufferInSeconds();
double nextFrameTime = getFrameTime(outStream) + period;

// output the first chirp
WriteAudioStream(outStream, chirpBuffer, chirpBufferSamples);

while(1) {  // synthesize and output samples forever
   // sleep for most of time + leave margin for jitter
   sleepSeconds(nextFrameTime - getFrameTime() - 0.5*bufferTime); 

   deltaFrameTime = nextFrameTime - getFrameTime(outStream);
   if(deltaTime < 0) { // oops, the os woke us up late, we missed our click 
      exit(-1);
   }
   else if(deltaTime < bufferTime) { // need to chirp within the buffer
	 nextFrameTime = getFrameTime(outStream) + period;
      writeSilence(outStream, deltaTime);
	 writeTone(outStream);
   }
   else { // the OS woke us up too early and the can't write to the buffer 
       // just sleep again
   }
}
