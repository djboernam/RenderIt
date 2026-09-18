# RenderIt extension loader
require 'sketchup.rb'
require 'extensions.rb'

module RenderIt
  unless const_defined?(:EXTENSION)
    EXTENSION = SketchupExtension.new('RenderIt', 'RenderIt/main')
    EXTENSION.creator = 'RenderIt'
    EXTENSION.description = 'LuxCoreRender-based architectural rendering for SketchUp 2025/2026.'
    EXTENSION.version = '0.6.0'
    EXTENSION.copyright = 'Copyright 2026 RenderIt'
    Sketchup.register_extension(EXTENSION, true)
  end
end
