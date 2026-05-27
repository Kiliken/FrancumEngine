@echo off

set "texassemble=https://github.com/microsoft/DirectXTex/releases/download/may2026/texassemble.exe"

if not exist "%~dp0bin\texassemble.exe" (
	md bin
	
	echo [DirectXTex] Downloading texassemble...
	curl -L "%texassemble%" -o "%~dp0bin\texassemble.exe" --progress-bar
	echo texassemble Installed...
	pause
	exit
)

set PATH=%~dp0bin;%PATH%


if "%~3"=="" (
	echo [Error] You must drop EXACTLY 3 files (AO, Roughness, and Metallic).
    echo Current input missing 1 or more files.
	pause
	exit
)

if not "%~3"=="" (
    echo ====================================================
	echo  Processing ORM Texture...
	echo ====================================================
	echo  Red Channel (AO):       %~nx1
	echo  Green Channel (Rough):  %~nx2
	echo  Blue Channel (Metal):   %~nx3
	echo ====================================================
	texassemble merge -o "%~dp0ORM.jpg" -f R8G8B8A8_UNORM -y "%~1" "%~2" "%~3"
    pause
    exit
)

exit
