@echo off

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"

SET includes=/Isrc /IDependencies/Include
SET links=/link /LIBPATH:%cd%/src /LIBPATH:%cd%/Dependencies/Lib/glew-2.1.0 /LIBPATH:%cd%/Dependencies/Lib/glfw-vc2022 MSVCRT.lib glew32s.lib glfw3.lib opengl32.lib User32.lib Shell32.lib Gdi32.lib Kernel32.lib Advapi32.lib
SET defines=/D DEBUG

cl /EHsc /Z7 /Fe"Main" %includes% %defines% src/main.cpp %links%

