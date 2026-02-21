#pragma once
#include <string>
#include <vector>
#include "Mesh.h"

namespace rex {

class Model {
public:
    Model() = default;
    ~Model();

    bool loadFromFile(const std::string& path);
    void draw() const;

    const std::vector<Mesh*>& getMeshes() const { return m_meshes; }

private:
    std::vector<Mesh*> m_meshes;
};

}
