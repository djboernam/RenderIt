module RenderIt
  module Lights
    DICT = 'RenderIt::Light'

    def self.create(type = 'point')
      model = Sketchup.active_model
      ents = model.active_entities
      point = model.active_view.camera.eye
      group = ents.add_group
      group.name = "RenderIt Light - #{type.capitalize}"
      group.set_attribute(DICT, 'type', type)
      group.set_attribute(DICT, 'power', 1000.0)
      group.set_attribute(DICT, 'temperature', 4000.0)
      group.set_attribute(DICT, 'color', [1.0, 1.0, 1.0])
      group.set_attribute(DICT, 'size', 1.0)
      group.set_attribute(DICT, 'enabled', true)
      group.transform!(Geom::Transformation.translation(point))
      group
    end

    def self.all
      Sketchup.active_model.entities.grep(Sketchup::Group).select do |group|
        group.get_attribute(DICT, 'type')
      end
    end
  end
end
