
$luaDownloadUrl = "https://www.lua.org/ftp/lua-5.3.4.tar.gz"

# Download Lua
Write-Output "[CMakeInstall] Downloading lua-5.3.4 ..."
curl.exe -L "$luaDownloadUrl" -o "$($PSScriptRoot)\lua.tar.gz" --progress-bar

# Extract Lua
Write-Output "[CMakeInstall] Extracting lua-5.3.4 in $($env:LOCALAPPDATA)..."
tar -xzf "$($PSScriptRoot)\lua.tar.gz" -C "$($env:LOCALAPPDATA)"

Remove-Item -Path "$($PSScriptRoot)\lua.zip" -Force

# Build Lua
$env:PATH = "$env:LOCALAPPDATA\w64devkit\bin;$env:PATH"
Set-Location "$env:LOCALAPPDATA\lua-5.3.4\src"
make generic


return 0
