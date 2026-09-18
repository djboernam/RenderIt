#include "renderit/api.hpp"
#include "renderit/engine.hpp"

#include <string>

using namespace renderit;

static Scene g_scene;

RI_API int ri_version() {
    return 500;
}

RI_API int ri_render_test(const char* output_path, int width, int height, int spp) {
    Settings settings;
    settings.width = width;
    settings.height = height;
    settings.spp = spp;
    settings.gpu = false;
    settings.denoise = true;

    if (g_scene.materials.empty()) {
        g_scene.materials.push_back(Material{});
    }

    Image image;
    Engine engine;
    std::string error;
    if (!engine.render(g_scene, settings, image, &error)) {
        return 0;
    }

    return engine.write_outputs(image, output_path, &error) ? 1 : 0;
}

RI_API int ri_backend_count() {
    return 1;
}

RI_API const char* ri_backend_name(int index) {
    return index == 0 ? "CPU Path Tracer" : "";
}

RI_API int ri_load_environment(const char* path) {
    return path ? 1 : 0;
}

RI_API int ri_set_light(const char* id, float intensity, float r, float g, float b) {
    if (!id) {
        return 0;
    }

    for (auto& light : g_scene.lights) {
        if (light.id == id) {
            light.intensity = intensity;
            light.color = {r, g, b};
            return 1;
        }
    }

    g_scene.lights.push_back({
        id,
        id,
        "default",
        0,
        {0.0f, 3.0f, 3.0f},
        {0.0f, -1.0f, 0.0f},
        {r, g, b},
        intensity,
        1.0f,
        6500.0f,
        true
    });

    return 1;
}
