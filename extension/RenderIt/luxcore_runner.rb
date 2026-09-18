module RenderIt
  module LuxCoreRunner
    extend self

    def executable
      configured = Sketchup.read_default('RenderIt', 'luxcore_executable', '')
      return configured unless configured.to_s.empty?
      File.expand_path(File.join(__dir__, '..', 'runtime', 'luxcoreconsole.exe'))
    end

    def render(scene_path, output_path, settings = {})
      exe = executable
      raise "LuxCoreRender executable not found: #{exe}" unless File.file?(exe)
      command = Sketchup.read_default('RenderIt', 'luxcore_command', '"%{exe}" --scene "%{scene}" --output "%{output}"')
      command = command.gsub('%{exe}', exe).gsub('%{scene}', scene_path).gsub('%{output}', output_path)
      ok = system(command)
      raise 'LuxCoreRender returned a failure status' unless ok
      output_path
    end
  end
end
