#include "renderit/engine.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

#ifdef RENDERIT_HAS_PNG
#include <png.h>
#endif

#ifdef RENDERIT_HAS_OPENEXR
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfArray.h>
#endif

namespace renderit {

static unsigned char to_srgb(float x) {
    x = std::clamp(x, 0.0f, 1.0f);
    x = x <= 0.0031308f ? 12.92f * x : 1.055f * std::pow(x, 1.0f / 2.4f) - 0.055f;
    return static_cast<unsigned char>(std::clamp(x, 0.0f, 1.0f) * 255.0f + 0.5f);
}

bool write_png(const Image& image, const std::string& path, std::string* error) {
#ifdef RENDERIT_HAS_PNG
    FILE* fp = fopen(path.c_str(), "wb");
    if (!fp) {
        if (error) {
            *error = "Failed to open PNG file for writing.";
        }
        return false;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        fclose(fp);
        if (error) {
            *error = "Failed to allocate PNG write struct.";
        }
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        fclose(fp);
        if (error) {
            *error = "Failed to allocate PNG info struct.";
        }
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        if (error) {
            *error = "PNG write failed.";
        }
        return false;
    }

    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, image.width, image.height, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png_ptr, info_ptr);

    std::vector<unsigned char> row(static_cast<size_t>(image.width) * 4);
    for (int y = 0; y < image.height; ++y) {
        for (int x = 0; x < image.width; ++x) {
            const size_t idx = static_cast<size_t>(y * image.width + x) * 4;
            row[x * 4 + 0] = to_srgb(image.rgba[idx + 0]);
            row[x * 4 + 1] = to_srgb(image.rgba[idx + 1]);
            row[x * 4 + 2] = to_srgb(image.rgba[idx + 2]);
            row[x * 4 + 3] = 255;
        }
        png_write_row(png_ptr, row.data());
    }

    png_write_end(png_ptr, nullptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return true;
#else
    if (error) {
        *error = "PNG support is not enabled in this build.";
    }
    return false;
#endif
}

bool write_hdr(const Image& image, const std::string& path, std::string* error) {
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        if (error) {
            *error = "Failed to open HDR file for writing.";
        }
        return false;
    }

    out << "#?RADIANCE\nFORMAT=32-bit_rle_rgbe\n\n-Y " << image.height << " +X " << image.width << "\n";

    for (int y = 0; y < image.height; ++y) {
        for (int x = 0; x < image.width; ++x) {
            const size_t idx = static_cast<size_t>(y * image.width + x) * 4;
            const float r = image.rgba[idx + 0];
            const float g = image.rgba[idx + 1];
            const float b = image.rgba[idx + 2];
            const float max_value = std::max({r, g, b, 0.0f});

            unsigned char rgbe[4] = {0, 0, 0, 0};
            if (max_value > 1e-32f) {
                int exponent = 0;
                const float mantissa = std::frexp(max_value, &exponent) * 256.0f / max_value;
                rgbe[0] = static_cast<unsigned char>(std::clamp(r * mantissa, 0.0f, 255.0f));
                rgbe[1] = static_cast<unsigned char>(std::clamp(g * mantissa, 0.0f, 255.0f));
                rgbe[2] = static_cast<unsigned char>(std::clamp(b * mantissa, 0.0f, 255.0f));
                rgbe[3] = static_cast<unsigned char>(exponent + 128);
            }
            out.write(reinterpret_cast<const char*>(rgbe), 4);
        }
    }

    return true;
}

bool write_exr(const Image& image, const std::string& path, std::string* error) {
#ifdef RENDERIT_HAS_OPENEXR
    try {
        Imf::Array2D<Imf::Rgba> pixels;
        pixels.resizeErase(image.height, image.width);

        for (int y = 0; y < image.height; ++y) {
            for (int x = 0; x < image.width; ++x) {
                const size_t idx = static_cast<size_t>(y * image.width + x) * 4;
                pixels[y][x] = Imf::Rgba(
                    image.rgba[idx + 0],
                    image.rgba[idx + 1],
                    image.rgba[idx + 2],
                    image.rgba[idx + 3]);
            }
        }

        Imf::RgbaOutputFile file(path.c_str(), image.width, image.height, Imf::WRITE_RGBA);
        file.setFrameBuffer(&pixels[0][0], 1, image.width);
        file.writePixels(image.height);
        return true;
    } catch (const std::exception& e) {
        if (error) {
            *error = e.what();
        }
        return false;
    }
#else
    if (error) {
        *error = "OpenEXR support is not enabled in this build.";
    }
    return false;
#endif
}

} // namespace renderit
