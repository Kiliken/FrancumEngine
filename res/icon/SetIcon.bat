@echo off

if exist "%localappdata%/w64devkit/bin" set PATH=%localappdata%/w64devkit/bin;%PATH%

echo Creating Custom Icon...
windres appicon.rc -O coff -o appicon.res
move appicon.res ../../build
pause
exit

