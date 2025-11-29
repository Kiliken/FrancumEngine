@echo off

if exist "%cd%/dep" goto ExitProgram
if not exist "%localappdata%/w64devkit" goto InstallGcc


:SetupProject
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0InstallOpenGL.ps1"
echo Project Setup completed...
pause
exit

:InstallGcc
echo Installing Gcc...
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0InstallGcc.ps1"
echo Waiting for Gcc to finish installing...
pause
goto SetupProject

:ExitProgram
echo Project is already installed...
pause
exit
