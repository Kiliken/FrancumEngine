@echo off

set "dxcDlUrl=https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.9.2602/dxc_2026_02_20.zip"
set "glsDlUrl=https://github.com/KhronosGroup/glslang/releases/download/main-tot/glslang-master-windows-Release.zip"


:: Please delete shaderTools directory on switching between compilers
set useDx=true


if %useDx%==true goto DirectXShaderCompiler


: GlslangCompiler

if not exist "%cd%\shaderTools" (
	md shaderTools
	
	echo [dxcInstall] Downloading GlslangCompiler...
	curl -L "%glsDlUrl%" -o "%cd%\glslang-win.zip" --progress-bar
	
	echo [dxcInstall] Installing GlslangCompiler...
	tar -xf "%cd%\glslang-win.zip" -C "%cd%\shaderTools"
	
	
	pause
	
)

set PATH=%cd%\shaderTools\bin;%PATH%


if "%~1"=="" (

	glslangValidator --version
	echo Drag a file in this batch file to compile it in SpirV shader
	pause
	exit
)

if not "%~1"=="" (
    echo Compiling Shader...

	
	findstr /R "\<VSMain\>" "%~1" >nul && (
		glslangValidator -D -S vert -e VSMain --target-env opengl -o "%~n1.vert.spv" "%~1"
	)
	
	findstr /R "\<PSMain\>" "%~1" >nul && (
		glslangValidator -D -S frag -e PSMain --target-env opengl -o "%~n1.frag.spv" "%~1"
	)
	
	findstr /R "\<CSMain\>" "%~1" >nul && (
		glslangValidator -D -S comp -e CSMain -G -o "%~n1.comp.spv" "%~1"
	)

    pause
    exit
)


exit

: DirectXShaderCompiler

if not exist "%cd%\shaderTools" (
	md shaderTools
	
	echo [dxcInstall] Downloading DirectXShaderCompiler...
	curl -L "%dxcDlUrl%" -o "%cd%\dxc_2026_02_20.zip" --progress-bar
	
	echo [dxcInstall] Installing DirectXShaderCompiler...
	tar -xf "%cd%\dxc_2026_02_20.zip" -C "%cd%\shaderTools"
	
	
	pause
	
)

set PATH=%cd%\shaderTools\bin\x64;%PATH%


if "%~1"=="" (
	echo Drag a file in this batch file to compile it in SpirV shader
	pause
	exit
)

if not "%~1"=="" (
    echo Compiling Shader...

	findstr /C:"VSMain" "%~1" >nul && (
		dxc -spirv -fspv-target-env=universal1.5 -T vs_6_0 -E VSMain "%~1" -Fo "%~n1.vs.spv"
	)
	
	findstr /C:"PSMain" "%~1" >nul && (
		dxc -spirv -fspv-target-env=universal1.5 -T ps_6_0 -E PSMain "%~1" -Fo "%~n1.ps.spv"
	)
	
	findstr /C:"CSMain" "%~1" >nul && (
		dxc -spirv -fspv-target-env=universal1.5 -T cs_6_0 -E CSMain "%~1" -Fo "%~n1.cs.spv"
	)

    pause
    exit
)

exit