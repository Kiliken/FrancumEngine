
$ProjectRoot = "$($PSScriptRoot)\.."

New-Item -Path "$($ProjectRoot)\dep" -ItemType Directory
New-Item -Path "$($ProjectRoot)\build" -ItemType Directory
New-Item -Path "$($ProjectRoot)\dep\include" -ItemType Directory
New-Item -Path "$($ProjectRoot)\dep\include\sol" -ItemType Directory
New-Item -Path "$($ProjectRoot)\dep\lib" -ItemType Directory
New-Item -Path "$($PSScriptRoot)\external" -ItemType Directory

# $glfwDownloadUrl = "https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip"
$sdlDownloadUrl = "https://github.com/libsdl-org/SDL/releases/download/release-3.4.10/SDL3-devel-3.4.10-mingw.zip"
# $glewDownloadUrl = "https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip"
$glmDownloadUrl = "https://github.com/g-truc/glm/releases/download/1.0.2/glm-1.0.2.zip"
$imguiDownloadUrl = "https://github.com/ocornut/imgui/archive/refs/tags/v1.92.5.zip"
# $fastgltfDownloadUrl = "https://github.com/spnda/fastgltf/archive/refs/tags/v0.9.0.tar.gz"



# Downloads
if ( -not (Test-Path "$($PSScriptRoot)\sdl.zip")){
	Write-Output "[openGL-portable-setup] Downloading SDL3..."
	curl.exe -L "$sdlDownloadUrl" -o "$($PSScriptRoot)\sdl.zip" --progress-bar
}
# Write-Output "[openGL-portable-setup] Downloading GLEW..."
# curl.exe -L "$glewDownloadUrl" -o "$($PSScriptRoot)\glew.zip" --progress-bar
if ( -not (Test-Path "$($PSScriptRoot)\glm.zip")){
	Write-Output "[openGL-portable-setup] Downloading GLM..."
	curl.exe -L "$glmDownloadUrl" -o "$($PSScriptRoot)\glm.zip" --progress-bar
}
if ( -not (Test-Path "$($PSScriptRoot)\imgui.zip")){
	Write-Output "[openGL-portable-setup] Downloading ImGUI..."
	curl.exe -L "$imguiDownloadUrl" -o "$($PSScriptRoot)\imgui.zip" --progress-bar
}
# if ( -not (Test-Path "$($PSScriptRoot)\fastgltf-0.9.0.tar.gz")){
# 	Write-Output "[openGL-portable-setup] Downloading fastgltf..."
# 	curl.exe -L "$fastgltfDownloadUrl" -o "$($PSScriptRoot)\fastgltf-0.9.0.tar.gz" --progress-bar
# }


Add-Type -AssemblyName System.IO.Compression.FileSystem

# Extract
Write-Output "[openGL-portable-setup] Extracting SDL3..."
[System.IO.Compression.ZipFile]::ExtractToDirectory("$($PSScriptRoot)\sdl.zip", "$($PSScriptRoot)")
# Write-Output "[openGL-portable-setup] Extracting GLEW..."
# [System.IO.Compression.ZipFile]::ExtractToDirectory("$($PSScriptRoot)\glew.zip", "$($PSScriptRoot)")
Write-Output "[openGL-portable-setup] Extracting GLM..."
[System.IO.Compression.ZipFile]::ExtractToDirectory("$($PSScriptRoot)\glm.zip", "$($PSScriptRoot)")
Write-Output "[openGL-portable-setup] Extracting ImGUI..."
[System.IO.Compression.ZipFile]::ExtractToDirectory("$($PSScriptRoot)\imgui.zip", "$($ProjectRoot)\dep\include")
# Write-Output "[openGL-portable-setup] Extracting fastgltf..."
# tar -xzf "$($PSScriptRoot)\fastgltf-0.9.0.tar.gz" -C "$($PSScriptRoot)\external"


# Dependencies Setup

# Glfw
Copy-Item -Path "$($PSScriptRoot)\SDL3-3.4.10\x86_64-w64-mingw32\include\SDL3" -Destination "$($ProjectRoot)\dep\include" -Recurse
Copy-Item -Path "$($PSScriptRoot)\SDL3-3.4.10\x86_64-w64-mingw32\bin\SDL3.dll" -Destination "$($ProjectRoot)\build" -Force
Copy-Item -Path "$($PSScriptRoot)\SDL3-3.4.10\x86_64-w64-mingw32\lib\libSDL3.dll.a" -Destination "$($ProjectRoot)\dep\lib" -Force



# Copy-Item -Path "$($PSScriptRoot)\glew-2.2.0\include\GL" -Destination "$($ProjectRoot)\dep\include" -Recurse

# Glm
Copy-Item -Path "$($PSScriptRoot)\glm\glm" -Destination "$($ProjectRoot)\dep\include" -Recurse

# Glad
New-Item -Path "$($ProjectRoot)\dep\include\glad" -ItemType Directory
Copy-Item -Path "$($PSScriptRoot)\common\gl.h" -Destination "$($ProjectRoot)\dep\include\glad" -Force
New-Item -Path "$($ProjectRoot)\dep\include\KHR" -ItemType Directory
Copy-Item -Path "$($PSScriptRoot)\common\khrplatform.h" -Destination "$($ProjectRoot)\dep\include\KHR" -Force

# Sol3
Copy-Item -Path "$($PSScriptRoot)\common\sol.hpp" -Destination "$($ProjectRoot)\dep\include\sol" -Force
Copy-Item -Path "$($PSScriptRoot)\common\config.hpp" -Destination "$($ProjectRoot)\dep\include\sol" -Force
Copy-Item -Path "$($PSScriptRoot)\common\forward.hpp" -Destination "$($ProjectRoot)\dep\include\sol" -Force

# ImGui
Rename-Item -Path "$($ProjectRoot)\dep\include\imgui-1.92.5" -NewName "imgui"
Copy-Item -Path "$($ProjectRoot)\dep\include\imgui\backends\imgui_impl_opengl3.cpp" -Destination "$($ProjectRoot)\dep\include\imgui"
Copy-Item -Path "$($ProjectRoot)\dep\include\imgui\backends\imgui_impl_opengl3.h" -Destination "$($ProjectRoot)\dep\include\imgui"
Copy-Item -Path "$($ProjectRoot)\dep\include\imgui\backends\imgui_impl_opengl3_loader.h" -Destination "$($ProjectRoot)\dep\include\imgui"
Copy-Item -Path "$($ProjectRoot)\dep\include\imgui\backends\imgui_impl_sdl3.cpp" -Destination "$($ProjectRoot)\dep\include\imgui"
Copy-Item -Path "$($ProjectRoot)\dep\include\imgui\backends\imgui_impl_sdl3.h" -Destination "$($ProjectRoot)\dep\include\imgui"

# fastgltf
# & "$($PSScriptRoot)\GenerateLibs.bat" "fastgltf-0.9.0"
# Copy-Item -Path "$($PSScriptRoot)\external\fastgltf-0.9.0\include\fastgltf" -Destination "$($ProjectRoot)\dep\include" -Recurse
# Copy-Item -Path "$($PSScriptRoot)\external\fastgltf-0.9.0\libfastgltf.a" -Destination "$($ProjectRoot)\dep\lib" -Force

# cgltf
Copy-Item -Path "$($PSScriptRoot)\common\cgltf.h" -Destination "$($ProjectRoot)\dep\include" -Force

# Setup Resources
Copy-Item -Path "$($PSScriptRoot)\resources.rc" -Destination "$($ProjectRoot)\build" -Force
Copy-Item -Path "$($PSScriptRoot)\icon.ico" -Destination "$($ProjectRoot)\build" -Force


# Clean Dependencies
Remove-Item -Path "$($PSScriptRoot)\SDL3-3.4.10" -Recurse -Force
Remove-Item -Path "$($PSScriptRoot)\glm" -Recurse -Force

return 0
