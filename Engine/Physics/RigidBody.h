#pragma once
#include "../Core/RexMath.h"
#include <string>

namespace rex {

enum class BodyType { Static, Dynamic, Kinematic };

class RigidBody {
public:
    RigidBody(BodyType type = BodyType::Dynamic);
    
    void applyForce(const Vec3& force);
    void applyImpulse(const Vec3& impulse);

    void setMass(float m) { 
        mass = m; 
        invMass = (m > 0) ? 1.0f / m : 0.0f; 
    }

    // State
    Vec3 position{0,0,0};
    Vec3 scale{1,1,1};
    Vec3 velocity{0,0,0};
    Vec3 force{0,0,0};
    float mass = 1.0f;
    float invMass = 1.0f;
    float restitution = 0.5f;
    float friction = 0.3f;
    BodyType type;

    // AABB bounds (unscaled)
    Vec3 minBound{-0.5f, -0.5f, -0.5f};
    Vec3 maxBound{0.5f, 0.5f, 0.5f};

    bool isColliding(const RigidBody& other) const {
        Vec3 aMin = { position.x + minBound.x * scale.x, position.y + minBound.y * scale.y, position.z + minBound.z * scale.z };
        Vec3 aMax = { position.x + maxBound.x * scale.x, position.y + maxBound.y * scale.y, position.z + maxBound.z * scale.z };
        Vec3 bMin = { other.position.x + other.minBound.x * other.scale.x, other.position.y + other.minBound.y * other.scale.y, other.position.z + other.minBound.z * other.scale.z };
        Vec3 bMax = { other.position.x + other.maxBound.x * other.scale.x, other.position.y + other.maxBound.y * other.scale.y, other.position.z + other.maxBound.z * other.scale.z };

        return (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
               (aMin.y <= bMax.y && aMax.y >= bMin.y) &&
               (aMin.z <= bMax.z && aMax.z >= bMin.z);
    }
};

}
