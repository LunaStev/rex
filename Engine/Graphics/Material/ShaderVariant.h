#pragma once

#include <cstdint>

namespace rex::gfx {

struct ShaderVariantKey {
    bool useNormalMap = false;
    bool useAoMap = false;
    bool receiveShadows = true;
    bool alphaMask = false;

    uint32_t hash() const {
        uint32_t h = 0;
        h |= (useNormalMap ? 1u : 0u) << 0;
        h |= (useAoMap ? 1u : 0u) << 1;
        h |= (receiveShadows ? 1u : 0u) << 2;
        h |= (alphaMask ? 1u : 0u) << 3;
        return h;
    }
};

} // namespace rex::gfx
