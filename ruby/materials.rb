module RenderIt
  module Materials
    DEFAULTS = {
      'base_color' => [0.72, 0.72, 0.72],
      'roughness' => 0.55,
      'metallic' => 0.0,
      'opacity' => 1.0,
      'transmission' => 0.0,
      'ior' => 1.52,
      'clearcoat' => 0.0,
      'glass' => false,
      'mirror' => false,
      'emission' => [0.0, 0.0, 0.0],
      'emission_strength' => 0.0,
      'thin_walled' => false
    }.freeze

    PRESETS = {
      'Glass' => {
        'base_color' => [0.95, 0.98, 1.0],
        'roughness' => 0.03,
        'transmission' => 0.96,
        'ior' => 1.52,
        'opacity' => 0.08,
        'glass' => true,
        'thin_walled' => true
      },
      'Mirror' => {
        'base_color' => [0.95, 0.95, 0.95],
        'roughness' => 0.01,
        'metallic' => 1.0,
        'mirror' => true
      },
      'Metal' => {
        'base_color' => [0.65, 0.67, 0.70],
        'roughness' => 0.22,
        'metallic' => 1.0
      }
    }.freeze

    def self.get(material)
      if material.nil?
        return DEFAULTS.dup
      end

      data = material.get_attribute('RenderIt::PBR', 'data') || {}
      DEFAULTS.merge(data)
    end

    def self.set(material, data)
      return unless material
      material.set_attribute('RenderIt::PBR', 'data', DEFAULTS.merge(data))
    end

    def self.apply_preset(material, name)
      set(material, PRESETS.fetch(name, {}))
    end
  end
end
