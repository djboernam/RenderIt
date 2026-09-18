# RenderIt LuxCoreRender integration

RenderIt uses an SDK-independent exporter by default:

```text
SketchUp Ruby API -> scene.json -> LuxCoreRender runtime -> PNG/HDR/EXR
```

Set the LuxCore executable path from SketchUp Ruby or the Ruby console:

```ruby
Sketchup.write_default('RenderIt', 'luxcore_executable', 'C:/RenderIt/runtime/luxcoreconsole.exe')
```

The command template is configurable because LuxCore distributions may expose different console entry points:

```ruby
Sketchup.write_default('RenderIt', 'luxcore_command', '"%{exe}" --scene "%{scene}" --output "%{output}"')
```

The exact command-line arguments must match the LuxCore build being distributed. The installer does not invent or redistribute third-party LuxCore binaries. Place the approved LuxCore runtime in `runtime/` and include its license and dependency notices before packaging.

This integration is intentionally external-process based. It does not require Trimble's native Desktop SDK and is therefore the recommended first Windows implementation.
