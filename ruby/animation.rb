module RenderIt
  module Animation
    def self.scenes
      Sketchup.active_model.pages.to_a.map(&:name)
    end

    def self.export_frames(folder, fps = 30, frames = 60)
      return if folder.nil? || folder.empty?

      Dir.mkdir(folder) unless Dir.exist?(folder)
      pages = Sketchup.active_model.pages.to_a
      return if pages.empty?

      frames.times do |i|
        page = pages[[i, pages.length - 1].min]
        Sketchup.active_model.pages.selected_page = page
        path = File.join(folder, format('frame_%05d.png', i))
        RenderIt::Renderer.render_to_file(path, 640, 360, 1)
      end
    end
  end
end
