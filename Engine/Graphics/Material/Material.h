#pragma once

#include "../../Core/RexMath.h"

#include <cstdint>

namespace rex::gfx {

enum class ShadingModel {
    Unlit,
    PBR,
};

class Material {
public:
    virtual ~Material() = default;

    ShadingModel shadingModel() const { return m_model; }

protected:
    explicit Material(ShadingModel model)
        : m_model(model) {
    }

private:
    ShadingModel m_model = ShadingModel::Unlit;
};

} // namespace rex::gfx
