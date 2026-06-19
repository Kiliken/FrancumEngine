@rem Script to build LuaJIT with GCC / MinGW.
@rem Adapted from the original msvcbuild.bat by Mike Pall.
@rem

@setlocal
@set GCCFLAGS=-O2 -Wall
@set LJDYNBUILD=-DLUA_BUILD_AS_DLL
@set DASMDIR=..\dynasm
@set DASM=%DASMDIR%\dynasm.lua
@set DASC=vm_x64.dasc
@set LJDLLNAME=lua51.dll
@set LJLIBNAME=lua51.a
@set ALL_LIB=lib_base.c lib_math.c lib_bit.c lib_string.c lib_table.c lib_io.c lib_os.c lib_package.c lib_debug.c lib_jit.c lib_ffi.c lib_buffer.c

@rem Compile minilua
gcc %GCCFLAGS% -o minilua.exe host\minilua.c
@if errorlevel 1 goto :BAD

@set DASMFLAGS=-D WIN -D JIT -D FFI -D ENDIAN_LE -D FPU -D P64
@set LJARCH=x64
@minilua
@if errorlevel 8 goto :NO32
@set DASC=vm_x86.dasc
@set DASMFLAGS=-D WIN -D JIT -D FFI -D ENDIAN_LE -D FPU
@set LJARCH=x86
@set GCCFLAGS=%GCCFLAGS% -msse2
@goto :DA
:NO32
@rem Fallback basic x64 parsing 
:X64
@if "%1" neq "nogc64" goto :DA
@shift
@set DASC=vm_x86.dasc
@set GCCFLAGS=%GCCFLAGS% -DLUAJIT_DISABLE_GC64
:DA
@if "%1" neq "lua52compat" goto :NOLUA52COMPAT
@shift
@set GCCFLAGS=%GCCFLAGS% -DLUAJIT_ENABLE_LUA52COMPAT
:NOLUA52COMPAT
minilua %DASM% -LN %DASMFLAGS% -o host\buildvm_arch.h %DASC%
@if errorlevel 1 goto :BAD

if exist ..\.git ( git show -s --format=%%ct >luajit_relver.txt ) else ( type ..\.relver >luajit_relver.txt )
minilua host\genversion.lua

@rem Compile buildvm
gcc %GCCFLAGS% -I "." -I %DASMDIR% %DASMTARGET% -o buildvm.exe host\buildvm.c host\buildvm_asm.c host\buildvm_peobj.c host\buildvm_lib.c host\buildvm_fold.c
@if errorlevel 1 goto :BAD

@rem Generate headers and ASM via buildvm
buildvm -m coffasm -o lj_vm.S
@if errorlevel 1 goto :BAD
buildvm -m bcdef -o lj_bcdef.h %ALL_LIB%
@if errorlevel 1 goto :BAD
buildvm -m ffdef -o lj_ffdef.h %ALL_LIB%
@if errorlevel 1 goto :BAD
buildvm -m libdef -o lj_libdef.h %ALL_LIB%
@if errorlevel 1 goto :BAD
buildvm -m recdef -o lj_recdef.h %ALL_LIB%
@if errorlevel 1 goto :BAD
buildvm -m vmdef -o jit\vmdef.lua %ALL_LIB%
@if errorlevel 1 goto :BAD
buildvm -m folddef -o lj_folddef.h lj_opt_fold.c
@if errorlevel 1 goto :BAD

@if "%1" neq "debug" goto :NODEBUG
@shift
@set GCCFLAGS=%GCCFLAGS% -g
:NODEBUG

@if "%1"=="amalg" goto :AMALGDLL
@if "%1"=="static" goto :STATIC

@rem --- Build Dynamic Link Library (DLL) ---
gcc %GCCFLAGS% -c lj_*.c lib_*.c
@if errorlevel 1 goto :BAD
gcc %GCCFLAGS% -c lj_vm.S
@if errorlevel 1 goto :BAD
gcc -shared -o %LJDLLNAME% -Wl,--out-implib,%LJLIBNAME% *.o
@if errorlevel 1 goto :BAD
@goto :EXE

:STATIC
@rem --- Build Static Library (.a) ---
gcc %GCCFLAGS% -c lj_*.c lib_*.c
@if errorlevel 1 goto :BAD
gcc %GCCFLAGS% -c lj_vm.S
@if errorlevel 1 goto :BAD
ar rcs %LJLIBNAME% *.o
@if errorlevel 1 goto :BAD
@goto :EXE

:AMALGDLL
@if "%2"=="static" goto :AMALGSTATIC
@rem --- Amalgamated DLL ---
gcc %GCCFLAGS% %LJDYNBUILD% -c ljamalg.c
@if errorlevel 1 goto :BAD
gcc %GCCFLAGS% -c lj_vm.S
@if errorlevel 1 goto :BAD
gcc -shared -o %LJDLLNAME% -Wl,--out-implib,%LJLIBNAME% ljamalg.o lj_vm.o
@if errorlevel 1 goto :BAD
@goto :EXE

:AMALGSTATIC
@rem --- Amalgamated Static ---
gcc %GCCFLAGS% -c ljamalg.c
@if errorlevel 1 goto :BAD
gcc %GCCFLAGS% -c lj_vm.S
@if errorlevel 1 goto :BAD
ar rcs %LJLIBNAME% ljamalg.o lj_vm.o
@if errorlevel 1 goto :BAD

:EXE
@rem --- Build Frontend Executable (luajit.exe) ---
gcc %GCCFLAGS% -c luajit.c
@if errorlevel 1 goto :BAD

@if exist %LJDLLNAME% (
    gcc %GCCFLAGS% -o luajit.exe luajit.o %LJLIBNAME%
) else (
    gcc %GCCFLAGS% -o luajit.exe luajit.o %LJLIBNAME% -static
)
@if errorlevel 1 goto :BAD

@del *.o minilua.exe buildvm.exe lj_vm.S
@del host\buildvm_arch.h
@del lj_bcdef.h lj_ffdef.h lj_libdef.h lj_recdef.h lj_folddef.h
@echo.
@echo === Successfully built LuaJIT for Windows/%LJARCH% (GCC) ===
@goto :END

:BAD
@echo.
@echo *******************************************************
@echo *** Build FAILED -- Please check the error messages ***
@echo *******************************************************
:END
@endlocal