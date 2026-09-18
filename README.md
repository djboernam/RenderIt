# RenderIt

RenderIt is a SketchUp 2025/2026 rendering extension project for Windows. It is designed around a pragmatic two-layer architecture:

- Ruby UI layer for SketchUp integration, menu hooks and HtmlDialog controls
- native C++ renderer core for CPU/GPU rendering, HDR/PNG/EXR output, and optional SketchUp Live C API bridge

This repository deliberately supports a safe fallback mode that works without the official Trimble SketchUp Desktop SDK, and an optional native bridge mode when the SDK is available on a Windows installation.

## Project structure

```text
RenderIt/
├── CMakeLists.txt
├── CMakePresets.json
├── vcpkg.json
├── .gitignore
├── README.md
├── include/
│   └── renderit/
│       ├── api.hpp
│       ├── scene.hpp
│       └── engine.hpp
├── src/
│   ├── main.cpp
│   ├── engine.cpp
│   ├── cpu_renderer.cpp
│   ├── image_io.cpp
│   ├── gpu_stub.cpp
│   └── bridge/
│       ├── host_bridge.cpp
│       └── sketchup_live_bridge.cpp
├── ruby/
│   ├── renderit.rb
│   ├── main.rb
│   ├── renderer.rb
│   ├── materials.rb
│   ├── lights.rb
│   ├── animation.rb
│   ├── native.rb
│   └── ui/
│       └── index.html
├── installer/
│   ├── RenderIt-Install.ps1
│   └── BUILD_INSTALLER.ps1
└── docs/
    └── ARCHITECTURE.md
```

## Runtime modes

### 1. SDK-independent mode (works without SketchUp Desktop SDK)

The Ruby layer exports the active SketchUp scene to a scene package, then launches the native renderer as a standalone process. This makes the extension workable even on a machine where the official native SDK is not installed.

### 2. Optional native bridge mode (requires Trimble SketchUp SDK)

When the SketchUp Desktop SDK is available and `RENDERIT_ENABLE_SKETCHUP` is enabled, the native renderer can use the Live C API to access the active model more directly.

## Required Windows toolchain for real production build

A complete final `RenderItNative.dll` / production `.exe` still needs:

- Visual Studio 2022 with MSVC v143
- Windows 10/11 SDK
- Trimble SketchUp Desktop SDK for SketchUp 2025 / 2026
- CMake
- vcpkg
- DirectX 11/12 headers and libraries
- OpenEXR / zlib / libpng dependencies
- optional GPU backend SDKs (OpenCL, CUDA, Vulkan, OptiX)

## Build on Windows

```powershell
# From a Windows shell with vcpkg and Visual Studio 2022 installed
vcpkg install --triplet x64-windows
cmake --preset windows-release
cmake --build build/windows --config Release
```

## Standalone smoke render

```powershell
./build/windows/Release/renderit_cli.exe ./renderit_test
```

This creates PNG/HDR output from the CPU reference renderer.

## Notes

This is a production-oriented scaffold and source tree, not a claim that a full commercial native GPU render engine has already been completed. The project is intentionally structured so the SDK-independent path is viable and the official Trimble SDK path is optional and version-aware.

For SketchUp 2025/2026 compatibility, the Ruby layer and the native bridge are separated so that the extension can remain installable even when the native SDK is not available.
