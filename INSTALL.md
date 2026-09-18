# RenderIt 0.6 package

The installable extension payload is under `extension/`.

## Development test

Copy `extension/RenderIt.rb` and the `extension/RenderIt/` directory to:

```text
%APPDATA%\SketchUp\SketchUp 2025\SketchUp\Plugins\
%APPDATA%\SketchUp\SketchUp 2026\SketchUp\Plugins\
```

## Build a package

```powershell
powershell -ExecutionPolicy Bypass -File .\installer\BUILD_INSTALLER.ps1
```

Then build `installer/RenderIt.iss` with Inno Setup 6. The LuxCoreRender runtime must be supplied separately and placed under `dist\RenderIt\runtime` after reviewing its license and dependencies.

The GitHub ZIP remains source code. A finished `.rbz` or `.exe` must be generated from the staged extension payload on Windows.
