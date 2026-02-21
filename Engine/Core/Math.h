#pragma once
#include <cmath>
#include <array>

namespace rex {

struct Vec3 {
    float x, y, z;
    static Vec3 cross(Vec3 a, Vec3 b) { return { a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x }; }
};

struct Mat4 {
    std::array<float, 16> data{};
    static Mat4 identity() { Mat4 m; m.data[0]=m.data[5]=m.data[10]=m.data[15]=1.0f; return m; }
    // Very basic math if GLM is missing
};

}
