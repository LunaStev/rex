#include "Model.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "../Core/Logger.h"

namespace rex {

Model::~Model() {
    for (auto* m : m_meshes) delete m;
}

bool Model::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        Logger::error("Failed to open model file: {}", path);
        return false;
    }

    std::vector<Vec3> positions;
    std::vector<Vec3> normals;
    std::vector<Vec2> texCoords;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            Vec3 v;
            ss >> v.x >> v.y >> v.z;
            positions.push_back(v);
        } else if (prefix == "vn") {
            Vec3 v;
            ss >> v.x >> v.y >> v.z;
            normals.push_back(v);
        } else if (prefix == "vt") {
            Vec2 v;
            ss >> v.x >> v.y;
            texCoords.push_back(v);
        } else if (prefix == "f") {
            for (int i = 0; i < 3; i++) {
                std::string vertexData;
                ss >> vertexData;
                
                // Parse face format: v/vt/vn
                std::replace(vertexData.begin(), vertexData.end(), '/', ' ');
                std::stringstream vss(vertexData);
                uint32_t vi, vti = 0, vni = 0;
                vss >> vi;
                if (vertexData.find(' ') != std::string::npos) {
                    vss >> vti >> vni;
                }

                Vertex v{};
                v.position = positions[vi - 1];
                if (vti > 0) v.texCoords = texCoords[vti - 1];
                if (vni > 0) v.normal = normals[vni - 1];
                
                vertices.push_back(v);
                indices.push_back(indices.size());
            }
        }
    }

    m_meshes.push_back(new Mesh(vertices, indices));
    Logger::info("Loaded model: {} ({} vertices)", path, vertices.size());
    return true;
}

void Model::draw() const {
    for (auto* m : m_meshes) m->draw();
}

}
