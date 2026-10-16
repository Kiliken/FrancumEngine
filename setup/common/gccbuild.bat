@rem Script to build LuaJIT with w64devkit (GCC / MinGW).
@rem Adapted from msvcbuild.bat by Mike Pall.
@rem
@rem Options (in order):
@rem    nogc64      disable LJ_GC64 mode for x64
@rem    lua52compat   enable extra Lua 5.2 extensions
@rem    debug         emit debug symbols
@rem    amalg         amalgamated build
@rem    static        create static library (.a)
@rem    mixed         create static library to build a DLL later

@setlocal
@set GCCFLAGS=-O2 -Wall -Wno-unused-result
@set LJDYNBUILD=-DLUA_BUILD_AS_DLL
@set DASMDIR=..\dynasm
@set DASM=%DASMDIR%\dynasm.lua
@set DASC=vm_x64.dasc
@set LJDLLNAME=lua51.dll
@set LJLIBNAME=liblua51.a
@set ALL_LIB=lib_base.c lib_math.c lib_bit.c lib_string.c lib_table.c lib_io.c lib_os.c lib_package.c lib_debug.c lib_jit.c lib_ffi.c lib_buffer.c

@rem --- Pre-parse options to catch debug/flags early ---
@set BUILD_MODE=dynamic
:PARSEOPTS
@if "%1"=="nogc64" (
    @shift
    @set GCCFLAGS=%GCCFLAGS% -DLUAJIT_DISABLE_GC64
    @goto PARSEOPTS
)
@if "%1"=="lua52compat" (
    @shift
    @set GCCFLAGS=%GCCFLAGS% -DLUAJIT_ENABLE_LUA52COMPAT
    @goto PARSEOPTS
)
@if "%1"=="debug" (
    @shift
    @set GCCFLAGS=%GCCFLAGS% -g -DLUA_USE_ASSERT
    @goto PARSEOPTS
)
@if "%1"=="amalg" (
    @shift
    @set BUILD_MODE=amalg
    @goto PARSEOPTS
)
@if "%1"=="static" (
    @shift
    @set BUILD_MODE=static
    @goto PARSEOPTS
)
@if "%1"=="mixed" (
    @shift
    @set BUILD_MODE=mixed
    @goto PARSEOPTS
)

@rem --- Compile minilua host tool ---
@echo === Compiling minilua ===
gcc %GCCFLAGS% -o minilua.exe host\minilua.c
@if errorlevel 1 goto :BAD

@rem --- Setup Architecture and DynASM flags ---
@set DASMFLAGS=-D WIN -D JIT -D FFI -D ENDIAN_LE -D FPU -D P64
@set LJARCH=x64
minilua %DASM% -LN %DASMFLAGS% -o host\buildvm_arch.h %DASC%
@if errorlevel 1 goto :BAD

@rem --- Generate version info ---
if exist ..\.git ( 
    git show -s --format=%%ct >luajit_relver.txt 
) else ( 
    type ..\.relver >luajit_relver.txt 
)
minilua host\genversion.lua

@rem --- Compile buildvm ---
@echo === Compiling buildvm ===
gcc %GCCFLAGS% -I "." -I %DASMDIR% -o buildvm.exe host\buildvm.c host\buildvm_asm.c host\buildvm_peobj.c host\buildvm_lib.c host\buildvm_fold.c
@if errorlevel 1 goto :BAD

@rem --- Generate headers and ASM via buildvm (PE/COFF format for MinGW) ---
@echo === Generating buildvm artifacts ===
buildvm -m peobj -o lj_vm.o
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

@rem --- Build Logic based on Mode ---
@if "%BUILD_MODE%"=="static" goto :BUILD_STATIC
@if "%BUILD_MODE%"=="mixed" goto :BUILD_STATIC
@if "%BUILD_MODE%"=="amalg" goto :BUILD_AMALG

@rem --- Standard Dynamic Library (DLL) ---
@echo === Building Dynamic Library (DLL) ===
gcc %GCCFLAGS% %LJDYNBUILD% -c lj_*.c lib_*.c
@if errorlevel 1 goto :BAD
gcc -shared -o %LJDLLNAME% -Wl,--out-implib,%LJLIBNAME% lj_*.o lib_*.o lj_vm.o
@if errorlevel 1 goto :BAD
@goto :BUILD_EXE

:BUILD_STATIC
@echo === Building Static Library (.a) ===
gcc %GCCFLAGS% -c lj_*.c lib_*.c
@if errorlevel 1 goto :BAD
@rem ar does not expand wildcards, use a batch loop to add them safely
ar rcs %LJLIBNAME% lj_vm.o
@if errorlevel 1 goto :BAD
for %%f in (lj_*.o) do ar r %LJLIBNAME% %%f
for %%f in (lib_*.o) do ar r %LJLIBNAME% %%f
@if errorlevel 1 goto :BAD
@goto :BUILD_EXE

:BUILD_AMALG
@echo === Building Amalgamated Build ===
gcc %GCCFLAGS% %LJDYNBUILD% -c ljamalg.c
@if errorlevel 1 goto :BAD
gcc -shared -o %LJDLLNAME% -Wl,--out-implib,%LJLIBNAME% ljamalg.o lj_vm.o
@if errorlevel 1 goto :BAD

:BUILD_EXE
@rem --- Build Frontend Executable (luajit.exe) ===
@echo === Building luajit.exe frontend ===
gcc %GCCFLAGS% -c luajit.c
@if errorlevel 1 goto :BAD

@if exist %LJDLLNAME% (
    gcc %GCCFLAGS% -o luajit.exe luajit.o %LJLIBNAME%
) else (
    gcc %GCCFLAGS% -o luajit.exe luajit.o %LJLIBNAME% -static
)
@if errorlevel 1 goto :BAD

@rem --- Cleanup intermediate files ---
@del *.o minilua.exe buildvm.exe
@del host\buildvm_arch.h
@del lj_bcdef.h lj_ffdef.h lj_libdef.h lj_recdef.h lj_folddef.h
@echo.
@echo === Successfully built LuaJIT for Windows/%LJARCH% (w64devkit/GCC) ===
@goto :END

:BAD
@echo.
@echo *******************************************************
@echo *** Build FAILED -- Please check the error messages ***
@echo *******************************************************
:END
@endlocal