@echo off


:FileCheck
if not exist "%localappdata%/w64devkit" goto InstallGcc
if not exist "%localappdata%/LuaJIT-2.1" goto InstallLua
if exist "%cd%/dep" goto ExitProgram


:SetupProject
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0setup/InstallEngine.ps1"
echo Project Setup completed...
pause
exit

:InstallGcc
echo Installing Gcc...
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0setup/InstallGcc.ps1"
echo Waiting for Gcc to finish installing...
pause
goto FileCheck

:InstallLua
echo Installing Lua...
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0setup/InstallLua.ps1"
echo Waiting for Lua to finish installing...
pause
goto FileCheck

:ExitProgram
echo Project is already installed...
pause
exit
