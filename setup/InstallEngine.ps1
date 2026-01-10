
$ProjectRoot = "$($PSScriptRoot)\.."

New-Item -Path "$($ProjectRoot)\dep" -ItemType Directory
New-Item -Path "$($ProjectRoot)\build" -ItemType Directory
New-Item -Path "$($ProjectRoot)\dep\include" -ItemType Directory
New-Item -Path "$($ProjectRoot)\dep\include\sol" -ItemType Directory
New-Item -Path "$($ProjectRoot)\dep\lib" -ItemType Directory

$glfwDownloadUrl = "https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip"
$glewDownloadUrl = "https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip"
$glmDownloadUrl = "https://github.com/g-truc/glm/releases/download/1.0.2/glm-1.0.2.zip"
$imguiDownloadUrl = "https://github.com/ocornut/imgui/archive/refs/tags/v1.92.5.zip"



# Downloads
Write-Output "[openGL-portable-setup] Downloading GLFW..."
curl.exe -L "$glfwDownloadUrl" -o "$($PSScriptRoot)\glfw.zip" --progress-bar
Write-Output "[openGL-portable-setup] Downloading GLEW..."
curl.exe -L "$glewDownloadUrl" -o "$($PSScriptRoot)\glew.zip" --progress-bar
Write-Output "[openGL-portable-setup] Downloading GLM..."
curl.exe -L "$glmDownloadUrl" -o "$($PSScriptRoot)\glm.zip" --progress-bar
Write-Output "[openGL-portable-setup] Downloading ImGUI..."
curl.exe -L "$imguiDownloadUrl" -o "$($PSScriptRoot)\imgui.zip" --progress-bar
Write-Output "[openGL-portable-setup] Downloading Sol3..."
curl.exe -L "https://github.com/ThePhD/sol2/releases/download/v3.3.0/sol.hpp" -o "$($ProjectRoot)\dep\include\sol\sol.hpp"
curl.exe -L "https://github.com/ThePhD/sol2/releases/download/v3.3.0/config.hpp" -o "$($ProjectRoot)\dep\include\sol\config.hpp"
curl.exe -L "https://github.com/ThePhD/sol2/releases/download/v3.3.0/forward.hpp" -o "$($ProjectRoot)\dep\include\sol\forward.hpp"


Add-Type -AssemblyName System.IO.Compression.FileSystem

# Extract
Write-Output "[openGL-portable-setup] Extracting GLFW..."
[System.IO.Compression.ZipFile]::ExtractToDirectory("$($PSScriptRoot)\glfw.zip", "$($PSScriptRoot)")
Write-Output "[openGL-portable-setup] Extracting GLEW..."
[System.IO.Compression.ZipFile]::ExtractToDirectory("$($PSScriptRoot)\glew.zip", "$($PSScriptRoot)")
Write-Output "[openGL-portable-setup] Extracting GLM..."
[System.IO.Compression.ZipFile]::ExtractToDirectory("$($PSScriptRoot)\glm.zip", "$($PSScriptRoot)")
Write-Output "[openGL-portable-setup] Extracting ImGUI..."
[System.IO.Compression.ZipFile]::ExtractToDirectory("$($PSScriptRoot)\imgui.zip", "$($ProjectRoot)\dep\include")
# Write-Output "[openGL-portable-setup] Extracting Sol3..."
# [System.IO.Compression.ZipFile]::ExtractToDirectory("$($PSScriptRoot)\sol3.zip", "$($PSScriptRoot)")



# Dependencies Setup
Copy-Item -Path "$($PSScriptRoot)\glfw-3.4.bin.WIN64\include\GLFW" -Destination "$($ProjectRoot)\dep\include" -Recurse
Copy-Item -Path "$($PSScriptRoot)\glfw-3.4.bin.WIN64\lib-mingw-w64\*" -Destination "$($ProjectRoot)\dep\lib" -Recurse
Copy-Item -Path "$($ProjectRoot)\dep\lib\glfw3.dll" -Destination "$($ProjectRoot)\build" -Force
Copy-Item -Path "$($PSScriptRoot)\glew-2.2.0\include\GL" -Destination "$($ProjectRoot)\dep\include" -Recurse
Copy-Item -Path "$($PSScriptRoot)\glm\glm" -Destination "$($ProjectRoot)\dep\include" -Recurse

Rename-Item -Path "$($ProjectRoot)\dep\include\imgui-1.92.5" -NewName "imgui"
Copy-Item -Path "$($ProjectRoot)\dep\include\imgui\backends\imgui_impl_opengl3.cpp" -Destination "$($ProjectRoot)\dep\include\imgui"
Copy-Item -Path "$($ProjectRoot)\dep\include\imgui\backends\imgui_impl_opengl3.h" -Destination "$($ProjectRoot)\dep\include\imgui"
Copy-Item -Path "$($ProjectRoot)\dep\include\imgui\backends\imgui_impl_opengl3_loader.h" -Destination "$($ProjectRoot)\dep\include\imgui"
Copy-Item -Path "$($ProjectRoot)\dep\include\imgui\backends\imgui_impl_glfw.cpp" -Destination "$($ProjectRoot)\dep\include\imgui"
Copy-Item -Path "$($ProjectRoot)\dep\include\imgui\backends\imgui_impl_glfw.h" -Destination "$($ProjectRoot)\dep\include\imgui"

# Lua Dependencies Fix
(Get-Content "$ProjectRoot\dep\include\sol\sol.hpp") `
    -replace 'this->construct\(std::forward<Args>\(args\)\.\.\.\);', 'this->emplace(std::forward<Args>(args)...);' |
    Set-Content "$ProjectRoot\dep\include\sol\sol.hpp"


Copy-Item -Path "$($PSScriptRoot)\GenerateLibs.bat" -Destination "$($PSScriptRoot)\glew-2.2.0\bin\Release\x64" -Force
Push-Location "$($PSScriptRoot)\glew-2.2.0\bin\Release\x64"
& ".\GenerateLibs.bat"
Pop-Location


# Setup Resources
Copy-Item -Path "$($PSScriptRoot)\resources.rc" -Destination "$($ProjectRoot)\build" -Force
Copy-Item -Path "$($PSScriptRoot)\icon.ico" -Destination "$($ProjectRoot)\build" -Force


# Clean Dependencies
Remove-Item -Path "$($PSScriptRoot)\glew-2.2.0" -Recurse -Force
Remove-Item -Path "$($PSScriptRoot)\glfw-3.4.bin.WIN64" -Recurse -Force
Remove-Item -Path "$($PSScriptRoot)\glm" -Recurse -Force

return 0
