param([string]$BuildDir = "$PSScriptRoot\..\build\windows\Release", [string]$OutputDir = "$PSScriptRoot\..\dist")
$ErrorActionPreference = 'Stop'
New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null
$stage = Join-Path $OutputDir 'RenderIt'
if (Test-Path $stage) { Remove-Item -Recurse -Force $stage }
New-Item -ItemType Directory -Force -Path "$stage\native\win64", "$stage\runtime" | Out-Null
Copy-Item -Recurse -Force "$PSScriptRoot\..\extension\*" $stage
if (Test-Path "$BuildDir\RenderItNative.dll") { Copy-Item "$BuildDir\RenderItNative.dll" "$stage\native\win64" }
if (Test-Path "$BuildDir\renderit_cli.exe") { Copy-Item "$BuildDir\renderit_cli.exe" "$stage\runtime" }
Write-Host "Staged RenderIt package at $stage"
Write-Host 'Place a licensed LuxCoreRender runtime at dist\RenderIt\runtime before building the installer.'
