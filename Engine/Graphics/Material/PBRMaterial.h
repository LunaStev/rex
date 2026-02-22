#pragma once

#include "Material.h"

namespace rex::gfx {

struct TextureHandles {
    uint32_t albedo = 0;
    uint32_t normal = 0;
    uint32_t roughness = 0;
    uint32_t metallic = 0;
    uint32_t ao = 0;
};

class PBRMaterial : public Material {
public:
    PBRMaterial()
        : Material(ShadingModel::PBR) {
    }

    Vec3 albedo{1.0f, 1.0f, 1.0f};
    float roughness = 0.5f;
    float metallic = 0.0f;
    float ao = 1.0f;

    TextureHandles textures;
};

} // namespace rex::gfx
