#pragma once
#include "GLInternal.h"
#include "../Core/RexMath.h"
#include <vector>

namespace rex {

struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec2 texCoords;
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    ~Mesh();

    void draw() const;

    static Mesh* createCube();

private:
    uint32_t m_vao, m_vbo, m_ebo;
    uint32_t m_indexCount;
};

}
