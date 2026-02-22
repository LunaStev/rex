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
        m = m * Mat4::rotateX(rotation.x * 0.0174533f);
        m = m * Mat4::rotateY(rotation.y * 0.0174533f);
        m = m * Mat4::rotateZ(rotation.z * 0.0174533f);
        m = m * Mat4::scale(scale);
        return m;
    }
};

struct MeshRenderer {
    Model* model = nullptr;
    Mesh* mesh = nullptr; // Fallback for simple shapes
    Vec3 color{1, 1, 1};
    float metallic = 0.0f;
    float roughness = 0.5f;
    float ao = 1.0f;

    MeshRenderer() = default;
    MeshRenderer(Model* mod, Mesh* m, Vec3 col) : model(mod), mesh(m), color(col) {}
    MeshRenderer(Model* mod, Mesh* m, Vec3 col, float met, float rough, float ambient)
        : model(mod), mesh(m), color(col), metallic(met), roughness(rough), ao(ambient) {}
};

struct RigidBodyComponent {
    BodyType type = BodyType::Dynamic;
    float mass = 1.0f;
    float restitution = 0.5f;
    float staticFriction = 0.6f;
    float dynamicFriction = 0.45f;
    float linearDamping = 0.02f;
    float angularDamping = 0.04f;
    bool enableCCD = true;
    Vec3 velocity{0,0,0};
    Vec3 angularVelocity{0,0,0}; // radians/sec

    RigidBody* internalBody = nullptr;
};

struct Camera {
    float fov = 45.0f;
    float aspect = 16.0f/9.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    bool isPerspective = true;

    Mat4 getProjection() const {
        if (isPerspective) return Mat4::perspectiveLH(fov * 0.0174533f, aspect, nearPlane, farPlane);
        return Mat4::identity();
    }
};

struct Light {
    enum Type { Directional, Point, Spot, Area } type = Point;

    Vec3 color{1.0f, 1.0f, 1.0f};
    float intensity = 1.0f;

    bool castShadows = true;
    bool volumetric = false;

    float range = 20.0f;

    float innerConeDeg = 20.0f;
    float outerConeDeg = 35.0f;

    float attenuationConstant = 1.0f;
    float attenuationLinear = 0.09f;
    float attenuationQuadratic = 0.032f;

    Light() = default;
    Light(Vec3 c, float i, Type t = Point)
        : type(t), color(c), intensity(i) {
    }
};

}
