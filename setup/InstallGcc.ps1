
$GccDownloadUrl = "https://github.com/skeeto/w64devkit/releases/download/v2.6.0/w64devkit-x64-2.6.0.7z.exe"

Add-Type -AssemblyName System.IO.Compression.FileSystem

Write-Output "[GccInstall] Downloading w64devkit..."
curl.exe -L "$GccDownloadUrl" -o "$($PSScriptRoot)\w64devkit.7z.exe" --progress-bar

Write-Output "[GccInstall] Extracting w64devkit in $($env:LOCALAPPDATA)..."
& "$PSScriptRoot\w64devkit.7z.exe" -o"$($env:LOCALAPPDATA)" -y

return 0
