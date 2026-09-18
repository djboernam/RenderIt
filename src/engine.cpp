#include "renderit/engine.hpp"

#include <cmath>
#include <string>

namespace renderit {

bool Engine::render(const Scene& scene, const Settings& settings, Image& image, std::string* error) {
    if (settings.gpu && render_gpu(scene, settings, image, error)) {
        return true;
    }

    if (!render_cpu(scene, settings, image, error)) {
        return false;
    }

    if (settings.denoise) {
        denoise(image, error);
    }

    return true;
}

bool Engine::write_outputs(const Image& image, const std::string& base, std::string* error) {
    bool ok = true;
    ok = write_png(image, base + ".png", error) && ok;
    ok = write_hdr(image, base + ".hdr", error) && ok;

    std::string exr_error;
    if (!write_exr(image, base + ".exr", &exr_error)) {
        if (error && error->empty()) {
            *error = exr_error;
        }
        ok = false;
    }

    return ok;
}

} // namespace renderit
