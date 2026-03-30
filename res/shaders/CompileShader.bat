@echo off

set "dxcDlUrl=https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.9.2602/dxc_2026_02_20.zip"
set "glsDlUrl=https://github.com/KhronosGroup/glslang/releases/download/main-tot/glslang-master-windows-Release.zip"


:: Please delete shaderTools directory on switching between compilers
set useDx=false


if %useDx%==true goto DirectXShaderCompiler


: GlslangCompiler

if not exist "%cd%\shaderTools\glslang" (
	md shaderTools
	md shaderTools\glslang
	
	echo [dxcInstall] Downloading GlslangCompiler...
	curl -L "%glsDlUrl%" -o "%cd%\glslang-win.zip" --progress-bar
	
	echo [dxcInstall] Installing GlslangCompiler...
	tar -xf "%cd%\glslang-win.zip" -C "%cd%\shaderTools\glslang"
	
	
	pause
	
)

set PATH=%cd%\shaderTools\glslang\bin;%PATH%


if "%~1"=="" (

	glslangValidator --version
	echo Drag a file in this batch file to compile it in SpirV shader
	pause
	exit
)

if not "%~1"=="" (
    echo Compiling Shader...

    if /I "%~x1"==".hlsl" (
        echo Detected HLSL shader

        findstr /R "\<VSMain\>" "%~1" >nul && (
            glslangValidator -D -S vert -e VSMain -o "%~n1.vert.spv" "%~1"
        )

        findstr /R "\<PSMain\>" "%~1" >nul && (
            glslangValidator -D -S frag -e PSMain -o "%~n1.frag.spv" "%~1"
        )

        findstr /R "\<CSMain\>" "%~1" >nul && (
            glslangValidator -D -S comp -e CSMain -G -o "%~n1.comp.spv" "%~1"
        )
    ) else if /I "%~x1"==".glsl" (
        echo Detected GLSL unique shader

        findstr /R "\<VSMain\>" "%~1" >nul && (
            glslangValidator -G -S vert --D VERT -e VSMain --source-entrypoint main -o "%~n1.vert.spv" "%~1"
        )

        findstr /R "\<PSMain\>" "%~1" >nul && (
            glslangValidator -G -S frag --D FRAG -e PSMain --source-entrypoint main -o "%~n1.frag.spv" "%~1"
        )

        findstr /R "\<CSMain\>" "%~1" >nul && (
            glslangValidator -G -S comp --D COMP -e CSMain --source-entrypoint main -o "%~n1.comp.spv" "%~1"
        )
    ) else (
		echo Detected GLSL "%~x1" shader
		
		:: use " --auto-map-locations " in case of location error
		glslangValidator -G "%~1" -o "%~1.spv"
    )

    pause
    exit
)



exit

: DirectXShaderCompiler

if not exist "%cd%\shaderTools\dxc" (
	md shaderTools
	md shaderTools\dxc
	
	echo [dxcInstall] Downloading DirectXShaderCompiler...
	curl -L "%dxcDlUrl%" -o "%cd%\dxc_2026_02_20.zip" --progress-bar
	
	echo [dxcInstall] Installing DirectXShaderCompiler...
	tar -xf "%cd%\dxc_2026_02_20.zip" -C "%cd%\shaderTools\dxc"
	
	
	pause
	
)

set PATH=%cd%\shaderTools\dxc\bin\x64;%PATH%


if "%~1"=="" (
	echo Drag a file in this batch file to compile it in SpirV shader
	pause
	exit
)

if not "%~1"=="" (
    echo Compiling Shader...

	findstr /C:"VSMain" "%~1" >nul && (
		dxc -spirv -fspv-target-env=vulkan1.0 -fvk-use-gl-layout -O3 -T vs_5_0 -D VERT -E VSMain "%~1" -Fo "%~n1.vs.spv"
	)
	
	findstr /C:"PSMain" "%~1" >nul && (
		dxc -spirv -fspv-target-env=vulkan1.0 -fvk-use-gl-layout -O3 -T ps_5_0 -D FRAG -E PSMain "%~1" -Fo "%~n1.ps.spv"
	)
	
	findstr /C:"CSMain" "%~1" >nul && (
		dxc -spirv -fspv-target-env=vulkan1.0 -fvk-use-gl-layout -O3 -T cs_5_0 -D COMP -E CSMain "%~1" -Fo "%~n1.cs.spv"
	)

    pause
    exit
)

exit