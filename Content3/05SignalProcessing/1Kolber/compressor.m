% Code used to generate diagrams for 
% Figure 3, "Simulation of a Basic Compressor"
% Authored by KC Lee
% Dan@zip.com.au


%----------------------------
% SIGNAL 'Source'/Input:
%	Create an arbitrary periodic tone-burst signal with varying 
%	step amplitudes (A1 - A4). Signal Is 400 Samples large.

nn=[1:400];
%for nn between certain values, want the amplititude to vary
A1 = 5  * (nn>=1   & nn<=100);
A2 = 50    * (nn>=101 & nn<=200);
A3 = 2.5 * (nn>=201 & nn<=300);
A4 = 25    * (nn>=301 & nn<=400);
An = A1 + A2 + A3 + A4;

initw=0.5;
disp('Default w at 0.5');
w=input('Enter new w  : ');
if (w==[])
	w=initw;
end

periodic=cos(w*nn);
X=An.*periodic;
subplot(2,2,1);plot(X);title('Input signal to compressor')
replot

%----------------------------
% ENVELOPE DETECTOR  C(n):
% 	C(n)=lamda*C(n-1) + (1-lamda)*C(n)
% lamda is proportional to the risetime/transient of the control signal
% hence it is directly proportional to attack/release times
% lamda=0 for instantaneous peak detector


% lamda is initially assumed to be 0.9 but this subsection 
% of code allows you to play around with the value
	initlamda=0.9;
	disp('Default lamda set to 0.9')
	lamda=input('Enter new lamda value : ');
	if (lamda==[])			%to change lamda value
		lamda=initlamda		%press enter to use default
	end


C(1)=(1-lamda)*abs(X(1));
n=2;
    for n=2:400
	C(n)=lamda*C(n-1) + (1-lamda)*abs(X(n));
    end

subplot(2,2,2);plot(C);title('Level detector output')
replot

%----------------------------
% GAIN PROCESSOR  G(n)
%	 G(n) 	= (C(n)/C0).^(p-1)   	,if C(n) >= C0
%		= 1			,if C(n) <  c0
% c0 = desired threshold
% G(n) is the gain (adaptive)


% c0 is initially assumed to be 10 but this subsection 
% of code allows you to play around with the value
	initc0 = 10;
	disp('Default threshold set to 10')
	c0=input('Enter new c0 value : ');
	if (c0==[])
		c0=initc0;
	end

% p is the compression ratio
% p > 1 for expander, p < 1 for compressor, p = 1 for unity
% p is initially set to 0.5 but this subsection 
% of code allows you to play around with the value
	initp  = .5;     	
	disp('Default p set to 0.5');
	p=input('Enter new p value : ');     %press enter to use default
	if (p==[])
		p=initp;
	end

n=1;
   for n=1:400;
	if C(n) >= c0
	    	G(n) = (C(n)/c0).^(p-1);
	else
	    	G(n) = 1;
	end
   end

subplot(2,2,3);plot(G);title('Gain processor output')
replot

%----------------------------
%OUTPUT Y2(n)
Y=G.*X';
%subplot(2,2,3); plot(Y);


%DELAY
%introduce delay, d, to signal X to compensate for for processing time in processed path
% Y(n)=G(n).X(n-d)
% This should reduce overshoots

% d is initially assumed to be 1 to simulate NO delay. However, subsection 
% of code allows you to play around with the value
	initd=1;   		
	disp('Default delay for compressor set to 1')
	d=input('Enter new delay value : ');
	if (d==[])
		d=initd
	end

% I have assumed that the initial output of the compressor will 
% be based on the first sample of the input signal (no good reason)
n=1;
for n=1:d
	Y2(n)=G(n)*X(1);
end
for n=d+1:400
	Y2(n)=G(n)*X(n-d);
end
subplot(2,2,4);plot(Y2);title('Output of compressor');
replot
