#pragma once

#ifdef _WIN32
#define RI_API extern "C" __declspec(dllexport)
#else
#define RI_API extern "C" __attribute__((visibility("default")))
#endif

struct RI_Vec3 {
    float x, y, z;
};

struct RI_Color4 {
    float r, g, b, a;
};

struct RI_RenderSettings {
    int width;
    int height;
    int spp;
    int max_depth;
    float exposure;
    int denoise;
    int gpu;
};

struct RI_RenderStats {
    unsigned long long rays;
    double seconds;
    int used_gpu;
};

RI_API int ri_version();
RI_API int ri_render_test(const char* output_path, int width, int height, int spp);
RI_API int ri_backend_count();
RI_API const char* ri_backend_name(int index);
RI_API int ri_load_environment(const char* path);
RI_API int ri_set_light(const char* id, float intensity, float r, float g, float b);
