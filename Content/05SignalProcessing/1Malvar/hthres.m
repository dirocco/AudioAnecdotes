function  Ht = hthres(X,fs)

% HTHRES   Hearing threshold 
%
% H. S. Malvar - Nov'00, (c) H. S. Malvar
%
% Syntax:  Ht = hthres(X,fs)
%
%    X = vector of frequency magnitude components
%   fs = sampling frequency, in Hz
%   Ht = vector of thresholds, in rms levels

Dabs  = 60;  % in dB, how much to bring down Fletcher-Munson curves;
             % it depends on the assumed playback level
Thmin = -60; % in dB, how far down can any threshold go
Thmax = 60;  % in dB, how far up can any threshold go
Rfac  = 8;   % Power reduction factor in dB for masking within the
             % same bark frequency band

% Bark subbands upper limits
Bh = [100 200 300 400 510 630 770 920 1080 1270 1480 1720 2000 ...
      2320 2700 3150 3700 4400 5300 6400 7700 9500 12000 15500 ...
      22050];

P = X.*X; % power spectrum
TdB = 0*P; % Threshold in dB
Nbands = length(X);
f=[0:Nbands-1]'*fs/(2*(Nbands-1)); % subband center freqs., in Hz

% Loop to generate Bark power spectrum and Bark center thresholds
dindx = Nbands*2/fs; % # of coefficients per Hz
stop = 0;
i1 = 1;
il=[]; iu=[];
for i = 1:25   % scan all 25 Bark bands
   % i-th Bark subband covers original subbands i1 to i2
   i2 = round(Bh(i)*dindx);
   if i2 > Nbands
      i2 = Nbands;
      stop = 1; % stop loop if signal bandwidth is reached
   end
   il = [il;i1];
   iu = [iu;i2];
   % Average RMS signal amplitude over Bark band [i1,i2]
   Arms = sqrt(mean(P(i1:i2)));
   % average signal amplitude over Bark band [i1,i2], in dB
   Adb = 20*log10(Arms+eps);
   % Tr = relative threshold given power level within i-th band
   Tr = Adb - Rfac; % center threshold
   Sbu(i) = Tr;
   i1 = i2+1;
   if stop break; end;
end
Lsb = i; % number of Barks covered for the given fs

% Cross-Bark spreading
Sbu = Sbu';
Sb = Sbu;
pl = -100;
pr = -100;
pli = Sbu(1);
for i = 1:Lsb
   if i < Lsb; pr = Sbu(i+1)-25; else pr = -100; end
   if pli > pl; pl = pli; end
   pl = pl - 10;
   if pl > Sb(i); Sb(i) = pl; end
   if pr > Sb(i); Sb(i) = pr; end
   pli = Sbu(i);
end

% Adjust thresholds considering absolute masking
for i = 1:Lsb
   i1 = il(i);
   i2 = iu(i);
   Tr = Sb(i);
   % Ta = Fletcher-Munson absolute threshold
   Ta = mean(thrabs(f(i1:i2)))-Dabs;
   if Tr < Ta; Tr = Ta; end
   % Clip at minimum and maximum levels
   if Tr < -Thmin; Tr = -Thmin; end
   if Tr > Thmax; Tr = Thmax; end
   Sb(i) = Tr; % save dB thresholds in vector Sb
   TdB(i1:i2) = Tr; % same threshold for all bands within Bark band
end
Ht = 10.^(TdB/20); % convert from power to rms levels

% end of hthres.m
