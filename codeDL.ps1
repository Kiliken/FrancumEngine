

$wc = New-Object Net.WebClient
Add-Type -AssemblyName System.IO.Compression.FileSystem

$wc.DownloadFile("https://github.com/Kiliken/openGL-portable-setup/archive/refs/heads/main.zip", "$($PSScriptRoot)\main.zip")

[System.IO.Compression.ZipFile]::ExtractToDirectory("$($PSScriptRoot)\main.zip", $PSScriptRoot)

Move-Item "$($PSScriptRoot)\openGL-portable-setup-main\*" $PSScriptRoot -Force
Remove-Item "$($PSScriptRoot)\openGL-portable-setup-main" -Recurse -Force
Remove-Item "$($PSScriptRoot)\main.zip" -Force
