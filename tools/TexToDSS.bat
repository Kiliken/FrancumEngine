@echo off

set "texconv=https://github.com/microsoft/DirectXTex/releases/download/may2026/texconv.exe"

if not exist "%~dp0bin\texconv.exe" (
	md bin
	
	echo [DirectXTex] Downloading texconv...
	curl -L "%texconv%" -o "%~dp0bin\texconv.exe" --progress-bar
	echo texconv Installed...
	pause
	exit
)

set PATH=%~dp0bin;%PATH%
set "ASSETS_PATH=..\res"


if "%~1"=="" (
    echo No file or folder dropped. 
    echo Converting file in: %ASSETS_PATH%
    echo -------------------------------------------------------
    
    :: Check if the constant path actually exists
    if not exist "%ASSETS_PATH%\" (
        echo.
        echo Error: The configured path does not exist.
        echo Please check the ASSETS_PATH variable at the top of the script.
        pause
        exit /b
    )

    :: Run the loop using the constant path
    for /r "%ASSETS_PATH%" %%i in (*.jpg *.png) do (
        echo Converting: %%~nxi -> %%~ni.dds
        for %%d in ("%%~dpi.") do (
            texconv -y -f BC3_UNORM -m 0 -o "%%~fld" "%%i"
        )
    )
    
    echo -------------------------------------------------------
    echo Batch conversion complete.
	
    pause
    exit /b
)

if not "%~1"=="" (
    echo Converting %~nx1 to DDS...
	texconv -y -f BC3_UNORM -m 0 -o "%~dp1." "%~1"
    pause
    exit
)

exit
