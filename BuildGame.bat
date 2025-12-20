@echo off

if exist "%localappdata%/w64devkit/bin" set PATH=%localappdata%/w64devkit/bin;%PATH%


SET includes=-I%cd%\src -I%cd%\dep\include
SET links=-L%cd%\src ^
-L%cd%\dep\lib ^
-lglfw3dll -lopengl32 -luser32 -lshell32 -lgdi32 -lkernel32 -ladvapi32 -lole32 ^
-loleaut32 -luuid -lodbc32 -lodbccp32 -lglew32
SET defines=-DRELEASE

if not exist "%cd%/build" md build


g++ -o "%cd%\build\App.exe" %includes% %defines% ^
 "%cd%\src\*.cpp" ^
 "%cd%\dep\include\imgui\imgui.cpp" ^
 "%cd%\dep\include\imgui\imgui_draw.cpp" ^
 "%cd%\dep\include\imgui\imgui_tables.cpp" ^
 "%cd%\dep\include\imgui\imgui_widgets.cpp" ^
 "%cd%\dep\include\imgui\imgui_demo.cpp" ^
 "%cd%\dep\include\imgui\imgui_impl_glfw.cpp" ^
 "%cd%\dep\include\imgui\imgui_impl_opengl3.cpp" ^
 %links%


if %errorlevel% neq 0 (
		color 0c
		echo Build failed!
) else (
		color 0a
		echo Build succeeded. Launching game...
		cd build
		start "" App.exe
)

