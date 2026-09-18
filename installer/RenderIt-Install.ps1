param(
  [string]$PackageRoot = $PSScriptRoot
)

$ErrorActionPreference = 'Stop'

$versions = @('2025', '2026')

foreach ($v in $versions) {
  $dst = Join-Path $env:APPDATA "SketchUp\SketchUp $v\SketchUp\Plugins\RenderIt"
  New-Item -ItemType Directory -Force -Path $dst | Out-Null

  $source = Join-Path $PackageRoot '..\ruby'
  Copy-Item -Recurse -Force $source $dst

  if (Test-Path (Join-Path $PackageRoot '..\native\win64\RenderItNative.dll')) {
    $nativeDir = Join-Path $dst 'native\win64'
    New-Item -ItemType Directory -Force -Path $nativeDir | Out-Null
    Copy-Item -Force (Join-Path $PackageRoot '..\native\win64\RenderItNative.dll') (Join-Path $nativeDir 'RenderItNative.dll')
  }
}

Write-Host 'RenderIt installed for SketchUp 2025 and 2026.'
