#pragma once
#include <cmath>
#include <cstring>

namespace rex {

struct Vec2 { float x, y; };
struct Vec3 {
    float x, y, z;

    Vec3 operator+(const Vec3& rhs) const {
        return { x + rhs.x, y + rhs.y, z + rhs.z };
    }

    Vec3 operator-(const Vec3& rhs) const {
        return { x - rhs.x, y - rhs.y, z - rhs.z };
    }

    Vec3 operator*(float s) const {
        return { x * s, y * s, z * s };
    }

    Vec3& operator+=(const Vec3& rhs) {
        x += rhs.x; y += rhs.y; z += rhs.z;
        return *this;
    }

    Vec3& operator-=(const Vec3& rhs) {
        x -= rhs.x; y -= rhs.y; z -= rhs.z;
        return *this;
    }
};
struct Vec4 { float x, y, z, w; };

inline float dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3 cross(const Vec3& a, const Vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

inline Vec3 normalize(const Vec3& v) {
    float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len == 0.0f) return {0, 0, 0};
    return { v.x / len, v.y / len, v.z / len };
}

struct Mat4 {
    float m[16];
    Mat4() { std::memset(m, 0, sizeof(m)); }
    
    static Mat4 identity() {
        Mat4 mat;
        mat.m[0] = mat.m[5] = mat.m[10] = mat.m[15] = 1.0f;
        return mat;
    }
    
    static Mat4 translate(const Vec3& v) {
        Mat4 mat = identity();
        mat.m[12] = v.x; mat.m[13] = v.y; mat.m[14] = v.z;
        return mat;
    }
    
    static Mat4 scale(const Vec3& v) {
        Mat4 mat = identity();
        mat.m[0] = v.x; mat.m[5] = v.y; mat.m[10] = v.z;
        return mat;
    }

    static Mat4 rotateY(float angle) {
        Mat4 mat = identity();
        float c = std::cos(angle);
        float s = std::sin(angle);
        mat.m[0] = c;  mat.m[2] = -s;
        mat.m[8] = s;  mat.m[10] = c;
        return mat;
    }

    static Mat4 rotateX(float angle) {
        Mat4 mat = identity();
        float c = std::cos(angle);
        float s = std::sin(angle);
        mat.m[5] = c;  mat.m[6] = s;
        mat.m[9] = -s; mat.m[10] = c;
        return mat;
    }

    static Mat4 perspective(float fov, float aspect, float near, float far) {
        float tanHalfFov = std::tan(fov / 2.0f);
        Mat4 mat;
        mat.m[0] = 1.0f / (aspect * tanHalfFov);
        mat.m[5] = 1.0f / (tanHalfFov);
        mat.m[10] = -(far + near) / (far - near);
        mat.m[11] = -1.0f;
        mat.m[14] = -(2.0f * far * near) / (far - near);
        return mat;
    }

    // Left-handed lookAt (+Z forward)
    static Mat4 lookAtLH(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Vec3 f = normalize({ center.x - eye.x, center.y - eye.y, center.z - eye.z });
        Vec3 s = normalize(cross(up, f));
        Vec3 u = cross(f, s);

        Mat4 m = identity();
        m.m[0] = s.x; m.m[4] = s.y; m.m[8] = s.z;
        m.m[1] = u.x; m.m[5] = u.y; m.m[9] = u.z;
        m.m[2] = f.x; m.m[6] = f.y; m.m[10] = f.z;
        m.m[12] = -dot(s, eye);
        m.m[13] = -dot(u, eye);
        m.m[14] = -dot(f, eye);
        return m;
    }

    // Left-handed perspective for OpenGL clip space (-1..1)
    static Mat4 perspectiveLH(float fov, float aspect, float near, float far) {
        float tanHalfFov = std::tan(fov / 2.0f);
        Mat4 mat;
        mat.m[0] = 1.0f / (aspect * tanHalfFov);
        mat.m[5] = 1.0f / (tanHalfFov);
        mat.m[10] = (far + near) / (far - near);
        mat.m[11] = 1.0f;
        mat.m[14] = -(2.0f * far * near) / (far - near);
        return mat;
    }

    Mat4 operator*(const Mat4& other) const {
        Mat4 res;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                for (int k = 0; k < 4; k++) {
                    res.m[j * 4 + i] += m[k * 4 + i] * other.m[j * 4 + k];
                }
            }
        }
        return res;
    }
};

}
