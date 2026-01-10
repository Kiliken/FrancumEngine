@echo off

if exist "%localappdata%/w64cmake/bin"	set PATH=%localappdata%/w64cmake/bin;%PATH%
if exist "%localappdata%/w64devkit/bin" set PATH=%localappdata%/w64devkit/bin;%PATH%

if not exist "%cd%/build" md build


cmake -S %cd% -B %cd%/build -G "MinGW Makefiles" ^
  -DCMAKE_C_COMPILER="%localappdata%/w64devkit/bin/gcc.exe" ^
  -DCMAKE_CXX_COMPILER="%localappdata%/w64devkit/bin/g++.exe"

cmake --build %cd%/build


if %errorlevel% neq 0 (
		color 0c
		echo Build failed!
) else (
		color 0a
		echo Build succeeded. Launching game...
		cd build
		Francum.exe
)
