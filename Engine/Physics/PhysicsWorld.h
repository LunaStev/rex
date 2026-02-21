#pragma once
#include <vector>
#include <memory>
#include "../Core/RexMath.h"

namespace rex {

class RigidBody;

struct RaycastHit {
    bool hit = false;
    Vec3 point{0,0,0};
    Vec3 normal{0,0,1};
    float distance = 0.0f;
    RigidBody* body = nullptr;
};

class PhysicsWorld {
public:
    PhysicsWorld();
    ~PhysicsWorld();

    void step(float dt);
    
    void addBody(RigidBody* body);
    void removeBody(RigidBody* body);

    RaycastHit raycast(const Vec3& origin, const Vec3& direction, float maxDist);

    void setGravity(const Vec3& g) { m_gravity = g; }
    Vec3 getGravity() const { return m_gravity; }

private:
    void resolveCollisions();
    void integrate(float dt);

    std::vector<RigidBody*> m_bodies;
    Vec3 m_gravity{0, -9.81f, 0};
    
    const float FIXED_STEP = 1.0f / 60.0f;
    float m_accumulator = 0.0f;
};

}
