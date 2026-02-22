#include "LightCuller.h"

#include <algorithm>
#include <cmath>

namespace rex::gfx {

std::vector<RuntimeLight> LightCuller::cullForView(const std::vector<RuntimeLight>& input,
                                                   const Vec3& viewPos,
                                                   int maxLights) const {
    if (maxLights <= 0) return {};

    struct ScoredLight {
        RuntimeLight light;
        float score = 0.0f;
    };

    std::vector<ScoredLight> scored;
    scored.reserve(input.size());

    for (const auto& light : input) {
        float score = light.intensity;
        if (light.kind != LightKind::Directional) {
            const Vec3 d = light.position - viewPos;
            const float distSq = dot(d, d);
            const float inv = 1.0f / std::max(1.0f, distSq);
            score *= inv * (light.range * light.range);
        } else {
            score += 10000.0f;
        }

        scored.push_back({light, score});
    }

    std::sort(scored.begin(), scored.end(), [](const ScoredLight& a, const ScoredLight& b) {
        return a.score > b.score;
    });

    if (static_cast<int>(scored.size()) > maxLights) {
        scored.resize(static_cast<size_t>(maxLights));
    }

    std::vector<RuntimeLight> out;
    out.reserve(scored.size());
    for (const auto& s : scored) {
        out.push_back(s.light);
    }
    return out;
}

} // namespace rex::gfx
