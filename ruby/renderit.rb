module RenderIt
  module UI
    @dialog = nil

    def self.show
      unless @dialog
        @dialog = ::UI::HtmlDialog.new(
          dialog_title: 'RenderIt',
          preferences_key: 'RenderIt',
          scrollable: true,
          resizable: true,
          width: 480,
          height: 820
        )
        @dialog.set_file(File.join(__dir__, 'ui', 'index.html'))
        @dialog.set_on_closed { @dialog = nil }
      end
      @dialog.show
    end

    def self.show_materials
      show
    end

    def self.show_lights
      show
    end

    def self.show_animation
      show
    end
  end
end
