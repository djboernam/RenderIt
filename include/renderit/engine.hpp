#pragma once

#include "renderit/scene.hpp"

#include <string>

namespace renderit {

class Engine {
public:
    bool render(const Scene& scene, const Settings& settings, Image& image, std::string* error = nullptr);
    bool write_outputs(const Image& image, const std::string& base, std::string* error = nullptr);
};

bool render_cpu(const Scene&, const Settings&, Image&, std::string* error = nullptr);
bool render_gpu(const Scene&, const Settings&, Image&, std::string* error = nullptr);
bool denoise(Image&, std::string* error = nullptr);
bool load_environment(const std::string&, Environment&, std::string* error = nullptr);
bool write_png(const Image&, const std::string&, std::string* error = nullptr);
bool write_hdr(const Image&, const std::string&, std::string* error = nullptr);
bool write_exr(const Image&, const std::string&, std::string* error = nullptr);

} // namespace renderit
