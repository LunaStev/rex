#include "GLInternal.h"
#include "../Core/RexMath.h"

namespace rex {

class SpriteRenderer {
public:
    SpriteRenderer();
    ~SpriteRenderer();

    void draw(uint32_t textureId, Vec2 position, Vec2 size = {1.0f, 1.0f}, float rotate = 0.0f, Vec3 color = {1.0f, 1.0f, 1.0f});

private:
    uint32_t m_quadVAO;
    void initRenderData();
};

}
