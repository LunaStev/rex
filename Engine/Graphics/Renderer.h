#pragma once
#include "../Core/Scene.h"
#include "../Core/Components.h"
#include "../Graphics/Shader.h"

namespace rex {

class Renderer {
public:
    Renderer();
    ~Renderer();

    void render(Scene& scene, const Camera& camera, const Mat4& viewMatrix);

private:
    std::unique_ptr<Shader> m_defaultShader;
};

}
