% AMPROC - Demonstration of Auditory Masking
%
% H. S. Malvar - Nov'00, (c) H. S. Malvar
%
% Input parameters:
%
% fin  = 'test.wav';    % input .wav audio file
% fout = 'out.wav';     % output .wav audio file
% plt  = 0;             % set to 1 to plot spectra of each block
%
% Example of calling this script:
%
% fin='test.wav'; fout='out.wav'; plt = 0; amproc

% Define FFT length = 2*M; no. of distinct bands = M+1
M = 2048;
h = sin((0.5+[0:2*M-1]')*pi/(2*M)); % 2*M-sample long FFT window

% Read input file into vector x
[x, fs] = wavread(fin);
xlen = length(x);
disp(sprintf(...
   'Input file "%s" read; length = %d samples, fs = %g Hz', ...
   fin, xlen, fs));

% Allocate space for output vector
y = zeros(xlen,1);

% Compute # of blocks
Nblocks = floor((xlen - M) / M);

% Block-by-block processing
ib = 0;
for block = 1:Nblocks
   
   % Read time-domain block from vector x,
   % with M/2-sample overlap
	ibrange = ib+(1:2*M);
   xb = x(ibrange, 1); % use only channel 1

	% Compute windowed FFT
   X = fft(xb .* h);
   
   % Compute magnitude. because of the FFT symmetries,
   % there are M + 1 distinct frequencies
   Xm = abs(X(1:M+1));
   
   % Example processing: zero out coefficients that are not
   % relevant (i.e., that are masked by the auditory system)
   Ht = hthres(Xm, fs);
   irel = find(Xm > Ht); % index of relevant coefficients
   prel = 100 * length(irel) / M;
   txt = sprintf('Block %3d/%3d, %5.1f%% relevant', block, ...
         Nblocks, prel);
   if plt == 1
      Nbands = length(Xm);
      f = [0:Nbands-1]'*fs / (2*(Nbands-1));
      semilogx(f,20*log10([Xm Ht]+eps)); title(txt);
      axis([20 20e3 -60 60])
      xlabel('frequency, Hz');
      ylabel('amplitude, dB');
      pause
   end
   disp(txt);
   Y = 0 * X;
   Y(irel) = X(irel);
   
   % Reconstruct time-domain signal
   Y(M+2:2*M) = conj(Y(M:-1:2)); % ensure symmetry
	yb = h .* real(ifft(Y));
   
   % Overlap block into output vector
   y(ibrange) = y(ibrange) + yb;

   % Move time index to next block & report progress
   ib = ib + M;
   %if mod(block, 50) == 0
   %   disp(sprintf('Processed block %4d of %4d', block, Nblocks));
   %end

end % block loop

% Write processed audio y to output file
wavwrite(y, fs, fout);
disp(sprintf('Output file "%s" written', fout));

% end of amproc.m
