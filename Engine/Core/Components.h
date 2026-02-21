#pragma once
#include "RexMath.h"
#include "../Graphics/Mesh.h"
#include "../Physics/RigidBody.h"

namespace rex {

class Model;

struct Transform {
    Vec3 position{0, 0, 0};
    Vec3 rotation{0, 0, 0};
    Vec3 scale{1, 1, 1};

    Mat4 getMatrix() const {
        Mat4 m = Mat4::translate(position);
        m = m * Mat4::rotateY(rotation.y * 0.0174533f);
        m = m * Mat4::scale(scale);
        return m;
    }
};

struct MeshRenderer {
    Model* model = nullptr;
    Mesh* mesh = nullptr; // Fallback for simple shapes
    Vec3 color{1, 1, 1};

    MeshRenderer() = default;
    MeshRenderer(Model* mod, Mesh* m, Vec3 col) : model(mod), mesh(m), color(col) {}
};

struct RigidBodyComponent {
    BodyType type = BodyType::Dynamic;
    float mass = 1.0f;
    float restitution = 0.5f;
    Vec3 velocity{0,0,0};
    
    RigidBody* internalBody = nullptr;
};

struct Camera {
    float fov = 45.0f;
    float aspect = 16.0f/9.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    bool isPerspective = true;

    Mat4 getProjection() const {
        if (isPerspective) return Mat4::perspective(fov * 0.0174533f, aspect, nearPlane, farPlane);
        return Mat4::identity();
    }
};

struct Light {
    Vec3 color{1, 1, 1};
    float intensity = 1.0f;
    enum Type { Directional, Point } type = Point;
};

}
