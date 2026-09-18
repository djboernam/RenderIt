module RenderIt
  VERSION = '0.6.0'

  unless file_loaded?(__FILE__)
    require 'sketchup.rb'
    require 'json'
    require File.join(__dir__, 'native')
    require File.join(__dir__, 'scene_exporter')
    require File.join(__dir__, 'luxcore_runner')
    require File.join(__dir__, 'renderer')
    require File.join(__dir__, 'materials')
    require File.join(__dir__, 'lights')
    require File.join(__dir__, 'animation')
    require File.join(__dir__, 'ui')

    menu = UI.menu('Extensions').add_submenu('RenderIt')
    menu.add_item('Render') { RenderIt::UI.show }
    menu.add_item('Interactive Render') { RenderIt::Renderer.start_interactive }
    menu.add_item('Material Editor') { RenderIt::UI.show_materials }
    menu.add_item('Lights / Light Mixer') { RenderIt::UI.show_lights }
    menu.add_item('Animation') { RenderIt::UI.show_animation }
    menu.add_separator
    menu.add_item('About RenderIt') { UI.messagebox("RenderIt #{VERSION}\nLuxCoreRender integration for SketchUp 2025/2026") }

    toolbar = UI::Toolbar.new('RenderIt')
    command = UI::Command.new('RenderIt') { RenderIt::UI.show }
    command.tooltip = 'RenderIt'
    toolbar.add_item(command)
    toolbar.show
    file_loaded(__FILE__)
  end
end
