cmake_minimum_required(VERSION 3.15)
project(FrancumEngine)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add Defines
add_compile_definitions(
    RELEASE
	SOL_ALL_SAFETIES_ON=1 
    SOL_LUAJIT=1
)



# Check File to Embed
file(GLOB RESOURCE_OBJECTS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/res/embed/*.o"
)

# Check for Main Files
file(GLOB MAIN_FILES
    src/*.cpp
	src/utils/*.cpp
)

# Executable Files
add_executable(Francum

	# GLAD
	src/gl.c
	
	# Main
	${MAIN_FILES}

	# ImGui core
	dep/include/imgui/imgui.cpp
	dep/include/imgui/imgui_draw.cpp
	dep/include/imgui/imgui_tables.cpp
	dep/include/imgui/imgui_widgets.cpp
	dep/include/imgui/imgui_demo.cpp
	
	# ImGui backends
	dep/include/imgui/imgui_impl_sdl3.cpp
    dep/include/imgui/imgui_impl_opengl3.cpp
	
	build/resources.rc
	
)

# Include Directories
target_include_directories(Francum PRIVATE 
    src 
    dep/include
	"$ENV{LOCALAPPDATA}/LuaJIT-2.1/src"
)

# Link Directories
target_link_directories(Francum PRIVATE 
    src 
    dep/lib
	"$ENV{LOCALAPPDATA}/LuaJIT-2.1/src"
)

# Link Libraries
target_link_libraries(Francum PRIVATE 
    opengl32
	user32
	shell32
	gdi32
	kernel32
	advapi32
	ole32
	oleaut32
	uuid
	odbc32
	odbccp32
	SDL3.dll
	lua51
)


# Embedding objects
target_sources(Francum PRIVATE
    ${RESOURCE_OBJECTS}
)


