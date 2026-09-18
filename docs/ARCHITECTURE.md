# RenderIt 0.5 — architecture notes

## Core runtime architecture

The project is intentionally split into two execution layers:

1. Ruby extension layer for SketchUp integration
2. Native C++ renderer layer for CPU/GPU rendering and output generation

## SDK-independent operation

The Ruby layer can export the scene and then call a standalone native executable, so the extension is still useful even without the official SketchUp C++ SDK.

## Optional native bridge

When the Trimble SketchUp Desktop SDK is present on a Windows machine, the native bridge can query the active model directly using the Live C API.

## Production requirement

A production `RenderItNative.dll` and final `RenderIt_Setup.exe` still require:

- Windows machine
- Visual Studio 2022 / v143
- Windows SDK
- Trimble SketchUp Desktop SDK
- DirectX headers / libs
- CMake / vcpkg / OpenEXR / libpng / zlib

## Planned extensions

- PBR material system and presets
- Light Mixer and pass-based rendering
- denoising and HDR/EXR output
- animation export and temporal processing
- optional GPU DirectX backend
