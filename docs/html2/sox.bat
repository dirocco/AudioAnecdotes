@echo off
echo Running sox...
if %1==n ..\..\bin\sox -r 44100 -c 1 -w -s ..\..\Content2\media\thought.raw thought.wav
if %1==r ..\..\bin\sox -r 44100 -c 1 -w -s ..\..\Content2\media\thought.raw thought.wav reverb 1 800 400 200
chmod -x thought.wav
exit
