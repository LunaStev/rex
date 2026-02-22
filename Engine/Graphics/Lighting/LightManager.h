#pragma once

#include "Light.h"
#include "../../Core/Scene.h"

#include <vector>

namespace rex::gfx {

class LightManager {
public:
    void gatherFromScene(Scene& scene);

    const std::vector<RuntimeLight>& lights() const { return m_lights; }
    const RuntimeLight* mainDirectionalLight() const;

private:
    std::vector<RuntimeLight> m_lights;
};

} // namespace rex::gfx
