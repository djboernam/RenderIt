module RenderIt
  VERSION = '0.5.0'

  unless file_loaded?(__FILE__)
    require 'sketchup.rb'
    require 'json'

    require File.join(__dir__, 'native')
    require File.join(__dir__, 'renderer')
    require File.join(__dir__, 'materials')
    require File.join(__dir__, 'lights')
    require File.join(__dir__, 'animation')

    menu = UI.menu('Extensions').add_submenu('RenderIt')
    menu.add_item('Render') { RenderIt::UI.show }
    menu.add_item('Interactive Render') { RenderIt::Renderer.start_interactive }
    menu.add_separator
    menu.add_item('Material Editor') { RenderIt::UI.show_materials }
    menu.add_item('Lights / Light Mixer') { RenderIt::UI.show_lights }
    menu.add_item('Animation') { RenderIt::UI.show_animation }
    menu.add_separator
    menu.add_item('About RenderIt') do
      UI.messagebox("RenderIt #{VERSION}\nSketchUp 2025/2026 rendering extension prototype")
    end

    toolbar = UI::Toolbar.new('RenderIt')
    cmd = UI::Command.new('RenderIt') { RenderIt::UI.show }
    cmd.tooltip = 'RenderIt'
    cmd.status_bar_text = 'Open RenderIt'
    toolbar.add_item(cmd)
    toolbar.show

    file_loaded(__FILE__)
  end
end
