#include "Mesh.h"

namespace rex {

Mesh::Mesh(const std::vector<Vertex>& v, const std::vector<uint32_t>& i) : m_indexCount(i.size()) {
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(Vertex), v.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, i.size() * sizeof(uint32_t), i.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
}

void Mesh::draw() const {
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Mesh* Mesh::createCube() {
    std::vector<Vertex> v = {
        {{-0.5, -0.5,  0.5}, {0,0,1}, {0,0}}, {{ 0.5, -0.5,  0.5}, {0,0,1}, {1,0}}, {{ 0.5,  0.5,  0.5}, {0,0,1}, {1,1}}, {{-0.5,  0.5,  0.5}, {0,0,1}, {0,1}},
        {{-0.5, -0.5, -0.5}, {0,0,-1}, {1,0}}, {{ 0.5, -0.5, -0.5}, {0,0,-1}, {0,0}}, {{ 0.5,  0.5, -0.5}, {0,0,-1}, {0,1}}, {{-0.5,  0.5, -0.5}, {0,0,-1}, {1,1}}
    };
    std::vector<uint32_t> i = { 0,1,2, 2,3,0, 4,5,6, 6,7,4, 0,4,7, 7,3,0, 1,5,6, 6,2,1, 3,7,6, 6,2,3, 0,4,5, 5,1,0 };
    return new Mesh(v, i);
}

}
