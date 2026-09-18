module RenderIt
  module Renderer
    extend self

    @history = []

    def default_settings
      {
        'sky' => 'gradient',
        'sun_enabled' => true,
        'sun' => { 'az' => 135.0, 'el' => 45.0, 'power' => 1800.0 },
        'exposure' => 0.0,
        'denoise' => true,
        'passes' => ['Beauty'],
        'motion_blur' => false,
        'dof' => false,
        'focus_distance' => 10.0,
        'aperture' => 2.8
      }
    end

    def render_to_file(path = nil, width = 640, height = 360, samples = 1, settings = nil)
      settings ||= default_settings
      target_path = path || File.join(Dir.tmpdir, 'renderit_render.png')

      # This fallback Ruby renderer is intentionally simple and deterministic.
      # The native renderer replaces it in the production Windows build.
      rgb = Array.new(width * height * 3, 0.0)
      width.times do |x|
        height.times do |y|
          idx = (y * width + x) * 3
          rgb[idx + 0] = ((x.to_f / width) * 255.0)
          rgb[idx + 1] = ((y.to_f / height) * 255.0)
          rgb[idx + 2] = 128.0
        end
      end

      File.binwrite(target_path, bmp(rgb, width, height))
      @history << target_path
      @history.shift while @history.length > 12
      target_path
    end

    def bmp(pixels, width, height)
      row = ((width * 3 + 3) / 4) * 4
      file_size = 54 + row * height
      out = [66, 77].pack('C*') + [file_size, 0, 0, 0, 54, 0, 0, 0, 40, 0, 0, 0, width, 0, 0, 0, height, 0, 0, 0, 1, 0, 24, 0, 0, 0, 0, 0, row * height, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0].pack('V2V2V2v2V6')

      height.times do |yy|
        y = height - 1 - yy
        bytes = ''.b
        width.times do |x|
          i = (y * width + x) * 3
          r = [pixels[i + 0], 0.0].max
          g = [pixels[i + 1], 0.0].max
          b = [pixels[i + 2], 0.0].max
          bytes << [(b * 255.0).round, (g * 255.0).round, (r * 255.0).round].pack('C3')
        end
        bytes << "\0" * (row - width * 3)
        out << bytes
      end

      out
    end

    def clear_history
      @history = []
    end

    def history
      @history.dup
    end

    def start_interactive
      UI.messagebox('Interactive render mode is available in the production Windows build with the native renderer.')
    end
  end
end
