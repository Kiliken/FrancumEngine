cmake_minimum_required(VERSION 3.15)
project(AppProject)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add Defines
add_compile_definitions(
    RELEASE
)



# Check File to Embed
file(GLOB RESOURCE_OBJECTS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/res/embed/*.o"
)

# Executable Files
add_executable(App

	# Main
    src/main.cpp

	# ImGui core
	dep/include/imgui/imgui.cpp
	dep/include/imgui/imgui_draw.cpp
	dep/include/imgui/imgui_tables.cpp
	dep/include/imgui/imgui_widgets.cpp
	dep/include/imgui/imgui_demo.cpp
	
	# ImGui backends
	dep/include/imgui/imgui_impl_glfw.cpp
    dep/include/imgui/imgui_impl_opengl3.cpp
	
)

# Include Directories
target_include_directories(App PRIVATE 
    src 
    dep/include
	"$ENV{LOCALAPPDATA}/lua-5.3.4/src"
)

# Link Directories
target_link_directories(App PRIVATE 
    src 
    dep/lib
	"$ENV{LOCALAPPDATA}/lua-5.3.4/src"
)

# Link Libraries
target_link_libraries(App PRIVATE 
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
	glfw3dll
	glew32
	lua
)


# Embedding objects
target_sources(App PRIVATE
    ${RESOURCE_OBJECTS}
)


