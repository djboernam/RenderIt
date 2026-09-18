module RenderIt
  module SceneExporter
    extend self
    DICT = 'RenderIt::PBR'

    def export(path, settings = {})
      require 'json'
      model = Sketchup.active_model
      triangles = []
      collect_entities(model.entities, Geom::Transformation.new, triangles)
      scene = {
        'version' => 1,
        'units' => 'inch',
        'camera' => camera_hash(model.active_view.camera),
        'triangles' => triangles,
        'materials' => model.materials.to_a.map { |m| material_hash(m) },
        'lights' => RenderIt::Lights.all.map { |g| light_hash(g) },
        'settings' => settings
      }
      File.binwrite(path, JSON.generate(scene))
      path
    end

    def collect_entities(entities, transform, output)
      entities.each do |entity|
        case entity
        when Sketchup::Face
          mesh = entity.mesh(7)
          points = mesh.points
          mesh.polygons.each do |poly|
            indices = poly.map(&:to_i)
            next if indices.length < 3
            indices[1..-2].each_with_index do |_, i|
              ids = [indices[0], indices[i + 1], indices[i + 2]]
              pts = ids.map { |id| points[id.abs - 1].transform(transform) }
              output << { 'a' => point(pts[0]), 'b' => point(pts[1]), 'c' => point(pts[2]), 'material' => material_name(entity.material || entity.back_material), 'object_id' => entity.object_id }
            end
          end
        when Sketchup::Group
          collect_entities(entity.entities, transform * entity.transformation, output) unless entity.hidden?
        when Sketchup::ComponentInstance
          collect_entities(entity.definition.entities, transform * entity.transformation, output) unless entity.hidden?
        end
      end
    end

    def point(p); [p.x.to_f, p.y.to_f, p.z.to_f]; end
    def material_name(m); m ? m.name : 'Default'; end

    def material_hash(m)
      data = m.get_attribute(DICT, 'data') || {}
      { 'name' => m.name, 'base_color' => data['base_color'] || [m.color.red / 255.0, m.color.green / 255.0, m.color.blue / 255.0], 'roughness' => data['roughness'] || 0.55, 'metallic' => data['metallic'] || 0.0, 'opacity' => data['opacity'] || 1.0, 'transmission' => data['transmission'] || 0.0, 'ior' => data['ior'] || 1.52, 'emission' => data['emission'] || [0, 0, 0], 'emission_strength' => data['emission_strength'] || 0.0 }
    end

    def camera_hash(camera)
      { 'eye' => point(camera.eye), 'target' => point(camera.target), 'up' => point(camera.up), 'fov' => camera.fov.to_f }
    end

    def light_hash(group)
      { 'id' => group.get_attribute(RenderIt::Lights::DICT, 'group_id', group.object_id.to_s), 'type' => group.get_attribute(RenderIt::Lights::DICT, 'type', 'point'), 'power' => group.get_attribute(RenderIt::Lights::DICT, 'power', 1000).to_f, 'color' => group.get_attribute(RenderIt::Lights::DICT, 'color', [1, 1, 1]), 'position' => point(group.transformation.origin) }
    end
  end
end
