module RenderIt
  module Native
    extend self

    def path
      File.expand_path(File.join(__dir__, '..', 'native', 'win64', 'RenderItNative.dll'))
    end

    def available?
      Sketchup.platform == :platform_win && File.file?(path)
    end

    def load
      return false unless available?

      require 'fiddle'
      @handle ||= Fiddle::Handle.new(path)
      true
    rescue StandardError => e
      @error = e.message
      false
    end
  end
end
