function  T = thrabs(f)

% THRABS   Absoute hearing threshold (Fletcher-Munson)
%
% H. S. Malvar - Nov'00, (c) H. S. Malvar
%
% Syntax:  T = thrabs(f)
%
%  f  = frequency, in Hz (or a vector of frequencies)
%  T  = corresponding absolute threshold of hearing, in dB SPL

fk = f/1000;
% Approximation to the F&M curves
T = 3.64*(fk.^(-.8))-6.5*exp(-.6*(fk-3.3).^2)+1e-3*fk.^4;

%end of thrabs.m
