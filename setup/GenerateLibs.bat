@echo off

if exist "%localappdata%/w64cmake/bin"	set PATH=%localappdata%/w64cmake/bin;%PATH%
if exist "%localappdata%/w64devkit/bin" set PATH=%localappdata%/w64devkit/bin;%PATH%

set toBuild=%cd%\setup\external\%1

echo %toBuild%

cmake -S %toBuild% -B %toBuild% -G "MinGW Makefiles" ^
  -DCMAKE_C_COMPILER="%localappdata%/w64devkit/bin/gcc.exe" ^
  -DCMAKE_CXX_COMPILER="%localappdata%/w64devkit/bin/g++.exe"

cmake --build %toBuild%


