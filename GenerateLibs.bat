@echo off

if exist "%appdata%/w64devkit/bin" set PATH=%appdata%/w64devkit/bin;%PATH%
if not exist glew32.dll exit


gendef glew32.dll
dlltool -d glew32.def -D glew32.dll -l libglew32.a -k

COPY libglew32.a ..\..\..\..\..\dep\lib
COPY glew32.dll ..\..\..\..\..\build
exit
