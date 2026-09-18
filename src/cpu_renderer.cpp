#include "renderit/engine.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>

namespace renderit {

static Vec3 add(const Vec3& a, const Vec3& b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

static Vec3 sub(const Vec3& a, const Vec3& b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

static Vec3 mul(const Vec3& a, float s) {
    return {a.x * s, a.y * s, a.z * s};
}

static Vec3 had(const Vec3& a, const Vec3& b) {
    return {a.x * b.x, a.y * b.y, a.z * b.z};
}

static float dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static Vec3 cross(const Vec3& a, const Vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

static Vec3 norm(const Vec3& v) {
    const float len = std::sqrt(std::max(1e-20f, dot(v, v)));
    return mul(v, 1.0f / len);
}

static Vec3 reflect(const Vec3& d, const Vec3& n) {
    return sub(d, mul(n, 2.0f * dot(d, n)));
}

struct Hit {
    float t = 0.0f;
    int material = 0;
    int object_id = 0;
    Vec3 normal{};
};

static bool intersect_triangle(const Scene& scene, const Vec3& ro, const Vec3& rd, const Triangle& tri, Hit& hit) {
    const Vec3 e1 = sub(tri.b, tri.a);
    const Vec3 e2 = sub(tri.c, tri.a);
    const Vec3 pvec = cross(rd, e2);
    const float det = dot(e1, pvec);
    if (std::fabs(det) < 1e-7f) {
        return false;
    }

    const float inv_det = 1.0f / det;
    const Vec3 tvec = sub(ro, tri.a);
    const float u = dot(tvec, pvec) * inv_det;
    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    const Vec3 qvec = cross(tvec, e1);
    const float v = dot(rd, qvec) * inv_det;
    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }

    const float t = dot(e2, qvec) * inv_det;
    if (t > 1e-4f) {
        hit.t = t;
        hit.material = tri.material;
        hit.object_id = tri.object_id;
        hit.normal = tri.n;
        return true;
    }

    return false;
}

static bool intersect(const Scene& scene, const Vec3& ro, const Vec3& rd, Hit& hit) {
    bool found = false;
    float best_t = std::numeric_limits<float>::infinity();
    Hit best_hit{};

    for (const auto& tri : scene.triangles) {
        Hit candidate{};
        if (intersect_triangle(scene, ro, rd, tri, candidate)) {
            if (candidate.t < best_t) {
                best_t = candidate.t;
                best_hit = candidate;
                found = true;
            }
        }
    }

    if (found) {
        hit = best_hit;
    }
    return found;
}

static Vec3 environment_background(const Scene& scene, const Vec3& d) {
    const float t = 0.5f * (d.y + 1.0f);
    const Vec3 sky_top{0.55f, 0.70f, 1.00f};
    const Vec3 sky_bottom{0.12f, 0.13f, 0.18f};
    return mul(add(mul(sky_bottom, 1.0f - t), mul(sky_top, t)), scene.environment.strength);
}

static Vec3 trace(const Scene& scene, const Vec3& ro, const Vec3& rd, int depth, std::mt19937& rng) {
    if (depth >= scene.camera.fov_deg > 0.0f ? 8 : 8) {
        return environment_background(scene, rd);
    }

    Hit hit{};
    if (!intersect(scene, ro, rd, hit)) {
        return environment_background(scene, rd);
    }

    const int material_index = std::clamp(hit.material, 0, static_cast<int>(scene.materials.size()) - 1);
    const Material& material = scene.materials[material_index];

    Vec3 normal = hit.normal;
    if (dot(normal, rd) > 0.0f) {
        normal = mul(normal, -1.0f);
    }

    const Vec3 p = add(ro, mul(rd, hit.t));
    Vec3 color = mul(material.emission, material.emission_strength);

    for (const auto& light : scene.lights) {
        if (!light.enabled) {
            continue;
        }

        Vec3 ldir = sub(light.position, p);
        const float dist_sq = std::max(1e-4f, dot(ldir, ldir));
        ldir = norm(ldir);

        const float ndotl = std::max(0.0f, dot(normal, ldir));
        if (ndotl <= 0.0f) {
            continue;
        }

        Hit shadow_hit{};
        const bool occluded = intersect(scene, add(p, mul(normal, 1e-4f)), ldir, shadow_hit);
        if (!occluded) {
            const float intensity = light.intensity / dist_sq * 0.001f;
            color = add(color, mul(had(material.base, light.color), ndotl * intensity));
        }
    }

    if (material.metallic > 0.05f || material.roughness < 0.35f) {
        const Vec3 reflected = norm(reflect(rd, normal));
        const Vec3 reflected_color = trace(scene, add(p, mul(normal, 1e-4f)), reflected, depth + 1, rng);
        color = add(color, mul(reflected_color, 0.35f + (1.0f - material.roughness) * 0.5f));
    }

    if (material.transmission > 0.001f) {
        const float eta = 1.0f / std::max(1.001f, material.ior);
        const float cosi = -dot(rd, normal);
        const float k = 1.0f - eta * eta * (1.0f - cosi * cosi);
        if (k >= 0.0f) {
            const Vec3 transmission_dir = norm(add(mul(rd, eta), mul(normal, eta * cosi - std::sqrt(k))));
            const Vec3 transmitted = trace(scene, add(p, mul(transmission_dir, 1e-4f)), transmission_dir, depth + 1, rng);
            color = add(mul(color, 1.0f - material.transmission), mul(transmitted, material.transmission));
        }
    }

    return color;
}

bool render_cpu(const Scene& scene, const Settings& settings, Image& image, std::string* error) {
    image.width = settings.width;
    image.height = settings.height;
    image.rgba.assign(static_cast<size_t>(settings.width) * settings.height * 4, 0.0f);

    if (scene.materials.empty()) {
        if (error) {
            *error = "No materials were defined in the scene.";
        }
        return false;
    }

    const float aspect = static_cast<float>(settings.width) / static_cast<float>(settings.height);
    const float scale = std::tan(scene.camera.fov_deg * 3.14159265f / 360.0f);

    std::mt19937 rng(1337);

    for (int y = 0; y < settings.height; ++y) {
        for (int x = 0; x < settings.width; ++x) {
            Vec3 color{0.0f, 0.0f, 0.0f};

            for (int s = 0; s < std::max(1, settings.spp); ++s) {
                const float jx = (s + 0.5f) / static_cast<float>(std::max(1, settings.spp));
                const float jy = (s + 0.5f) / static_cast<float>(std::max(1, settings.spp));

                const float px = ((static_cast<float>(x) + jx) / static_cast<float>(settings.width) * 2.0f - 1.0f) * aspect * scale;
                const float py = (1.0f - (static_cast<float>(y) + jy) / static_cast<float>(settings.height) * 2.0f) * scale;

                const Vec3 target = add(scene.camera.forward, add(mul(scene.camera.right, px), mul(scene.camera.up, py)));
                const Vec3 direction = norm(target);
                color = add(color, trace(scene, scene.camera.eye, direction, 0, rng));
            }

            color = mul(color, 1.0f / std::max(1, settings.spp));
            color = mul(color, std::pow(2.0f, settings.exposure));

            const size_t idx = static_cast<size_t>(y * settings.width + x) * 4;
            image.rgba[idx + 0] = std::clamp(color.x, 0.0f, 1.0f);
            image.rgba[idx + 1] = std::clamp(color.y, 0.0f, 1.0f);
            image.rgba[idx + 2] = std::clamp(color.z, 0.0f, 1.0f);
            image.rgba[idx + 3] = 1.0f;
        }
    }

    return true;
}

bool denoise(Image& image, std::string* error) {
    if (image.width < 3 || image.height < 3) {
        if (error) {
            *error = "Image is too small to denoise.";
        }
        return false;
    }

    std::vector<float> src = image.rgba;
    for (int y = 1; y < image.height - 1; ++y) {
        for (int x = 1; x < image.width - 1; ++x) {
            float r = 0.0f, g = 0.0f, b = 0.0f;
            for (int yy = -1; yy <= 1; ++yy) {
                for (int xx = -1; xx <= 1; ++xx) {
                    const size_t idx = static_cast<size_t>((y + yy) * image.width + (x + xx)) * 4;
                    r += src[idx + 0];
                    g += src[idx + 1];
                    b += src[idx + 2];
                }
            }
            const size_t idx = static_cast<size_t>(y * image.width + x) * 4;
            image.rgba[idx + 0] = r / 9.0f;
            image.rgba[idx + 1] = g / 9.0f;
            image.rgba[idx + 2] = b / 9.0f;
        }
    }

    return true;
}

bool render_gpu(const Scene&, const Settings&, Image&, std::string* error) {
    if (error) {
        *error = "No GPU backend is linked in this build. Use the CPU renderer or enable the Windows DirectX/CUDA backend in the production configuration.";
    }
    return false;
}

bool load_environment(const std::string& path, Environment& env, std::string* error) {
    if (path.empty()) {
        if (error) {
            *error = "Environment path cannot be empty.";
        }
        return false;
    }
    env.path = path;
    return true;
}

} // namespace renderit
