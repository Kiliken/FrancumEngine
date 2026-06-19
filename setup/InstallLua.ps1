
$luaDownloadUrl = "https://github.com/LuaJIT/LuaJIT/archive/refs/heads/v2.1.zip"

# Download Lua
Write-Output "[CMakeInstall] Downloading LuaJIT-2.1 ..."
curl.exe -L "$luaDownloadUrl" -o "$($PSScriptRoot)\lua.tar.gz" --progress-bar

# Extract Lua
Write-Output "[CMakeInstall] Extracting LuaJIT-2.1 in $($env:LOCALAPPDATA)..."
tar -xzf "$($PSScriptRoot)\lua.tar.gz" -C "$env:LOCALAPPDATA\"

Copy-Item -Path "$($PSScriptRoot)\common\gccbuild.bat" -Destination "$env:LOCALAPPDATA\LuaJIT-2.1\src" -Force

# Remove-Item -Path "$($PSScriptRoot)\lua.tar.gz" -Force

# Build Lua
$env:PATH = "$env:LOCALAPPDATA\w64devkit\bin;$env:PATH"
Set-Location "$env:LOCALAPPDATA\LuaJIT-2.1\src"
cmd.exe /C gccbuild.bat

return 0
