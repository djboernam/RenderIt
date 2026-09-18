#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace renderit {

struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct Camera {
    Vec3 eye{0.0f, 0.0f, 5.0f};
    Vec3 forward{0.0f, 0.0f, -1.0f};
    Vec3 right{1.0f, 0.0f, 0.0f};
    Vec3 up{0.0f, 1.0f, 0.0f};
    float fov_deg = 45.0f;
    float focus_distance = 10.0f;
    float aperture = 0.0f;
};

struct Material {
    std::string id = "material_0";
    std::string name = "default_material";
    Vec3 base{0.75f, 0.75f, 0.75f};
    Vec3 emission{0.0f, 0.0f, 0.0f};
    float roughness = 0.55f;
    float metallic = 0.0f;
    float specular = 0.5f;
    float opacity = 1.0f;
    float transmission = 0.0f;
    float ior = 1.52f;
    float clearcoat = 0.0f;
    float emission_strength = 0.0f;
    int type = 0;
};

struct Triangle {
    Vec3 a{};
    Vec3 b{};
    Vec3 c{};
    Vec3 n{};
    int material = 0;
    int object_id = 0;
};

struct Light {
    std::string id = "light_0";
    std::string name = "light_0";
    std::string group = "default";
    int type = 0;
    Vec3 position{0.0f, 3.0f, 4.0f};
    Vec3 direction{0.0f, -1.0f, 0.0f};
    Vec3 color{1.0f, 1.0f, 1.0f};
    float intensity = 1500.0f;
    float size = 1.0f;
    float temperature = 6500.0f;
    bool enabled = true;
};

struct Environment {
    std::string path;
    float rotation = 0.0f;
    float exposure = 0.0f;
    float strength = 1.0f;
    bool visible = true;
    bool lighting = true;
    bool reflections = true;
};

struct Scene {
    std::vector<Triangle> triangles;
    std::vector<Material> materials;
    std::vector<Light> lights;
    Camera camera;
    Environment environment;
    Vec3 sun_dir{-0.4f, -0.7f, -0.3f};
    Vec3 sun_color{1.0f, 1.0f, 1.0f};
    float sun_intensity = 2.0f;
};

struct Settings {
    int width = 1280;
    int height = 720;
    int spp = 16;
    int max_depth = 8;
    float exposure = 0.0f;
    bool denoise = true;
    bool gpu = false;
    bool motion_blur = false;
    bool dof = false;
    std::string output;
};

struct Image {
    int width = 0;
    int height = 0;
    std::vector<float> rgba;
};

} // namespace renderit
