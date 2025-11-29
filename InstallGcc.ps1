
$GccDownloadUrl = "https://github.com/skeeto/w64devkit/releases/download/v2.4.0/w64devkit-x64-2.4.0.7z.exe"

$wc = New-Object Net.WebClient
Add-Type -AssemblyName System.IO.Compression.FileSystem

Write-Output "[GccInstall] Downloading w64devkit..."
$wc.DownloadFile($GccDownloadUrl, "$($PSScriptRoot)\w64devkit.7z.exe")

Write-Output "[GccInstall] Extracting w64devkit in $($env:LOCALAPPDATA)..."
& "$PSScriptRoot\w64devkit.7z.exe" -o"$($env:LOCALAPPDATA)" -y

return 0
