#include "renderit/engine.hpp"
#include "renderit/scene.hpp"

#include <iostream>

int main(int argc, char** argv) {
    renderit::Scene scene;
    scene.materials.push_back(renderit::Material{});
    scene.triangles.push_back({
        {-2.0f, -1.0f, 0.0f},
        {2.0f, -1.0f, 0.0f},
        {0.0f, 2.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
        0,
        1
    });
    scene.lights.push_back({
        "key",
        "Key",
        "default",
        0,
        {2.0f, 3.0f, 4.0f},
        {0.0f, -1.0f, 0.0f},
        {1.0f, 1.0f, 1.0f},
        1500.0f,
        1.0f,
        6500.0f,
        true
    });

    renderit::Settings settings;
    settings.width = 320;
    settings.height = 180;
    settings.spp = 4;
    settings.gpu = false;
    settings.output = argc > 1 ? argv[1] : "renderit_test";

    renderit::Image image;
    renderit::Engine engine;
    std::string error;

    if (!engine.render(scene, settings, image, &error)) {
        std::cerr << "render failed: " << error << '\n';
        return 1;
    }

    const std::string base = settings.output;
    if (!engine.write_outputs(image, base, &error)) {
        std::cerr << "write failed: " << error << '\n';
        return 1;
    }

    std::cout << "RenderIt native smoke render complete" << std::endl;
    return 0;
}
