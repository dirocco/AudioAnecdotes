function y = tone(hz,duration)
samplerate = 18000;
y = sin((0:duration*samplerate-1)/samplerate*2*pi*hz);
end

