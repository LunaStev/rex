#pragma once

#include "../Core/Components.h"
#include "../Core/Scene.h"
#include "RigidBody.h"
#include "RustPhysicsFFI.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace rex {

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
    float restLength = -1.0f;
    float stiffness = 1.0f;
    float damping = 0.2f;
};

class PhysicsSystem {
public:
    PhysicsSystem();
    ~PhysicsSystem();

    PhysicsSystem(const PhysicsSystem&) = delete;
    PhysicsSystem& operator=(const PhysicsSystem&) = delete;

    void update(Scene& scene, float dt);

    void setGravity(const Vec3& g);
    Vec3 getGravity() const { return m_gravity; }
    void setSolverIterations(int velocityIterations, int positionIterations);
    void setMaxSubSteps(int maxSubSteps);

    int addDistanceJoint(const DistanceJointDesc& desc);
    void removeDistanceJoint(int jointId);
    void clearDistanceJoints();

    RaycastHit raycast(const Vec3& origin, const Vec3& direction, float maxDist);

private:
    struct DistanceJointState {
        int id = 0;
        RigidBody* a = nullptr;
        RigidBody* b = nullptr;
        Vec3 localAnchorA{0, 0, 0};
        Vec3 localAnchorB{0, 0, 0};
        float restLength = 0.0f;
        float stiffness = 1.0f;
        float damping = 0.2f;
    };

    void step(float dt);
    void simulate(float dt);

    std::unordered_map<EntityId, std::unique_ptr<RigidBody>> m_bodyPool;
    std::vector<DistanceJointState> m_joints;

    ffi::RexPhysicsWorld* m_rustWorld = nullptr;

    Vec3 m_gravity{0, -9.81f, 0};
    int m_solverIterations = 10;
    int m_positionIterations = 4;
    int m_maxSubSteps = 6;

    const float FIXED_STEP = 1.0f / 60.0f;
    float m_accumulator = 0.0f;
    float m_maxFrameStep = 0.1f;
    int m_nextJointId = 1;
};

} // namespace rex
