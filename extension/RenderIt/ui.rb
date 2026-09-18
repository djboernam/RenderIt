module RenderIt
  module UI
    @dialog = nil
    def self.show
      unless @dialog
        @dialog = ::UI::HtmlDialog.new(dialog_title: 'RenderIt', preferences_key: 'RenderIt', scrollable: true, resizable: true, width: 480, height: 820)
        @dialog.set_file(File.join(__dir__, 'ui', 'index.html'))
        bind(@dialog)
        @dialog.set_on_closed { @dialog = nil }
      end
      @dialog.show
    end
    def self.show_materials; show; end
    def self.show_lights; show; end
    def self.show_animation; show; end
    def self.bind(dialog)
      dialog.add_action_callback('render') do |_ctx, payload|
        require 'json'; require 'tmpdir'
        options = JSON.parse(payload.to_s)
        dir = File.join(Dir.tmpdir, 'RenderIt')
        Dir.mkdir(dir) unless Dir.exist?(dir)
        scene = File.join(dir, 'scene.json')
        output = File.join(dir, 'render.png')
        RenderIt::SceneExporter.export(scene, options)
        begin
          RenderIt::LuxCoreRunner.render(scene, output, options)
          dialog.execute_script("showImage(#{output.to_json})")
        rescue StandardError => e
          UI.messagebox("RenderIt: #{e.message}")
        end
      end
      dialog.add_action_callback('choose_environment') { |_ctx, _| UI.openpanel('Choose HDRI / EXR environment', nil, 'HDRI/EXR|*.hdr;*.exr') }
      dialog.add_action_callback('interactive') { |_ctx, _| UI.messagebox('Interactive LuxCoreRender mode requires a configured LuxCore runtime.') }
    end
  end
end
