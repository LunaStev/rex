#pragma once

#include <cstdint>
#include <vector>

#include "../Core/RexMath.h"

namespace rex {

class RigidBody;

struct RaycastHit {
    bool hit = false;
    Vec3 point{0, 0, 0};
    Vec3 normal{0, 0, 1};
    float distance = 0.0f;
    RigidBody* body = nullptr;
};

struct DistanceJointDesc {
    RigidBody* bodyA = nullptr;
    RigidBody* bodyB = nullptr;
    Vec3 localAnchorA{0, 0, 0};
    Vec3 localAnchorB{0, 0, 0};
    float restLength = -1.0f; // negative means "use current distance"
    float stiffness = 1.0f;   // 0..1 positional correction strength
    float damping = 0.2f;     // velocity damping on joint axis
};

class PhysicsWorld {
public:
    PhysicsWorld();
    ~PhysicsWorld();

    void step(float dt);

    void addBody(RigidBody* body);
    void removeBody(RigidBody* body);

    int addDistanceJoint(const DistanceJointDesc& desc);
    void removeDistanceJoint(int jointId);
    void clearDistanceJoints();

    RaycastHit raycast(const Vec3& origin, const Vec3& direction, float maxDist);

    void setGravity(const Vec3& g);
    Vec3 getGravity() const { return m_gravity; }
    void setSolverIterations(int velocityIterations, int positionIterations);
    void setMaxSubSteps(int maxSubSteps);

private:
    struct DistanceJointConstraint {
        int id = 0;
        RigidBody* a = nullptr;
        RigidBody* b = nullptr;
        Vec3 localAnchorA{0, 0, 0};
        Vec3 localAnchorB{0, 0, 0};
        float restLength = 0.0f;
        float stiffness = 1.0f;
        float damping = 0.2f;
    };

    void simulate(float dt);

    std::vector<RigidBody*> m_bodies;
    std::vector<DistanceJointConstraint> m_joints;

    Vec3 m_gravity{0, -9.81f, 0};
    int m_solverIterations = 10;
    int m_positionIterations = 4;
    int m_maxSubSteps = 6;

    const float FIXED_STEP = 1.0f / 60.0f;
    float m_accumulator = 0.0f;
    float m_maxFrameStep = 0.1f;
    int m_nextJointId = 1;

    void* m_rustWorld = nullptr;
};

} // namespace rex
