@echo off
echo Running demo...
cd ..\..\src\synthesis\projects\demo
cat scores/%2.ski | demo %1 -or
exit
