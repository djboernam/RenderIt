#ifdef RENDERIT_SKETCHUP_SDK
#include <SketchUpAPI/application/application.h>
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/face.h>
#include <SketchUpAPI/model/vertex.h>
#include <SketchUpAPI/geometry.h>

#include "renderit/scene.hpp"

#include <string>
#include <vector>

namespace renderit {

static Vec3 to_vec3(const SUPoint3D& p) {
    return {static_cast<float>(p.x), static_cast<float>(p.y), static_cast<float>(p.z)};
}

static bool extract_active_model_scene(Scene& out, std::string* error) {
    SUModelRef model = SU_INVALID;
    if (SUApplicationGetActiveModel(&model) != SU_ERROR_NONE || model == SU_INVALID) {
        if (error) {
            *error = "No active SketchUp model";
        }
        return false;
    }

    SUEntitiesRef entities = SU_INVALID;
    if (SUModelGetEntities(model, &entities) != SU_ERROR_NONE) {
        if (error) {
            *error = "Cannot access active SketchUp entities";
        }
        return false;
    }

    size_t face_count = 0;
    if (SUEntitiesGetNumFaces(entities, &face_count) != SU_ERROR_NONE) {
        if (error) {
            *error = "Failed to retrieve face count";
        }
        return false;
    }

    std::vector<SUFaceRef> faces(face_count);
    if (face_count > 0) {
        size_t actual_faces = face_count;
        if (SUEntitiesGetFaces(entities, face_count, faces.data(), &actual_faces) != SU_ERROR_NONE) {
            if (error) {
                *error = "Failed to get face references";
            }
            return false;
        }
    }

    out.triangles.clear();
    for (size_t i = 0; i < face_count; ++i) {
        size_t vertex_count = 0;
        if (SUFaceGetNumVertices(faces[i], &vertex_count) != SU_ERROR_NONE || vertex_count < 3) {
            continue;
        }

        std::vector<SUVertexRef> vertices(vertex_count);
        if (SUFaceGetVertices(faces[i], vertex_count, vertices.data(), &vertex_count) != SU_ERROR_NONE) {
            continue;
        }

        std::vector<Vec3> points(vertex_count);
        for (size_t j = 0; j < vertex_count; ++j) {
            SUPoint3D p{};
            if (SUVertexGetPosition(vertices[j], &p) == SU_ERROR_NONE) {
                points[j] = to_vec3(p);
            }
        }

        SUVector3D normal{};
        if (SUFaceGetNormal(faces[i], &normal) == SU_ERROR_NONE) {
            const Vec3 n{static_cast<float>(normal.x), static_cast<float>(normal.y), static_cast<float>(normal.z)};
            for (size_t j = 1; j + 1 < vertex_count; ++j) {
                out.triangles.push_back({points[0], points[j], points[j + 1], n, 0, static_cast<int>(i)});
            }
        }
    }

    if (out.materials.empty()) {
        out.materials.push_back(Material{});
    }

    return true;
}

} // namespace renderit
#endif
