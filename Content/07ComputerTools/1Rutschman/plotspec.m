function plotspec(data,oversample)
samplerate=18000*oversample;

l = columns(data);
xvals = 1:(1+l/2);
freqs = (xvals - 1)/l * samplerate;

gset ytics 6
gset xrange [0:44100]
gset yrange [-96:6]

for k = 1:rows(data);
  plotdata(k,:) = [20*log10(abs(fft([data(k,:)])(xvals)/l*2))];
end
plot (freqs, plotdata);

gset ytics autofreq
gset xtics autofreq
gset autoscale
end

