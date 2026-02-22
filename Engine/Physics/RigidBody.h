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
    void applyTorque(const Vec3& torque);
    void applyAngularImpulse(const Vec3& impulse);
    void applyImpulseAtPoint(const Vec3& impulse, const Vec3& worldPoint);
    void wakeUp();
    void updateInertiaTensor();
    void setMass(float m);

    // State
    Vec3 position{0,0,0};
    Vec3 scale{1,1,1};
    Quat orientation = Quat::identity();
    Vec3 velocity{0,0,0};
    Vec3 angularVelocity{0,0,0};
    Vec3 force{0,0,0};
    Vec3 torque{0,0,0};
    float mass = 1.0f;
    float invMass = 1.0f;
    Vec3 invInertia{0,0,0}; // Diagonal local inertia inverse (box approximation)
    float restitution = 0.5f;
    float staticFriction = 0.6f;
    float dynamicFriction = 0.45f;
    float linearDamping = 0.02f;
    float angularDamping = 0.04f;
    bool enableCCD = true;
    bool enableSleep = true;
    bool isAwake = true;
    float sleepTimer = 0.0f;
    BodyType type;

    // Local oriented box bounds before world transform.
    Vec3 localBoundsMin{-0.5f, -0.5f, -0.5f};
    Vec3 localBoundsMax{0.5f, 0.5f, 0.5f};
};

}
