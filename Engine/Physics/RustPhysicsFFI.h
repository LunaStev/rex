#pragma once

#include <cstddef>
#include <cstdint>

namespace rex::ffi {

struct RexPhysicsWorld;

struct RexVec3 {
    float x;
    float y;
    float z;
};

struct RexQuat {
    float x;
    float y;
    float z;
    float w;
};

enum RexBodyType : uint32_t {
    REX_BODY_STATIC = 0,
    REX_BODY_DYNAMIC = 1,
    REX_BODY_KINEMATIC = 2,
};

enum RexBodyFlags : uint32_t {
    REX_BODY_FLAG_ENABLE_CCD = 1u << 0,
    REX_BODY_FLAG_ENABLE_SLEEP = 1u << 1,
};

struct RexPhysicsBody {
    uint64_t id;
    uint32_t bodyType;
    uint32_t flags;
    float mass;
    float invMass;

    RexVec3 position;
    RexVec3 scale;
    RexQuat orientation;

    RexVec3 velocity;
    RexVec3 angularVelocity;
    RexVec3 force;
    RexVec3 torque;

    RexVec3 invInertia;
    RexVec3 localBoundsMin;
    RexVec3 localBoundsMax;

    float restitution;
    float staticFriction;
    float dynamicFriction;
    float linearDamping;
    float angularDamping;

    uint32_t isAwake;
    float sleepTimer;
};

struct RexDistanceJoint {
    uint64_t id;
    uint64_t bodyA;
    uint64_t bodyB;
    RexVec3 localAnchorA;
    RexVec3 localAnchorB;
    float restLength;
    float stiffness;
    float damping;
};

struct RexRaycastHit {
    uint32_t hit;
    uint64_t body;
    RexVec3 point;
    RexVec3 normal;
    float distance;
};

extern "C" {
RexPhysicsWorld* rex_physics_world_create();
void rex_physics_world_destroy(RexPhysicsWorld* world);

void rex_physics_world_set_gravity(RexPhysicsWorld* world, RexVec3 gravity);
void rex_physics_world_set_solver_iterations(RexPhysicsWorld* world, int velocityIterations, int positionIterations);
void rex_physics_world_set_max_substeps(RexPhysicsWorld* world, int maxSubSteps);

void rex_physics_world_step(
    RexPhysicsWorld* world,
    float dt,
    RexPhysicsBody* bodies,
    std::size_t bodyCount,
    const RexDistanceJoint* joints,
    std::size_t jointCount);

void rex_physics_world_raycast(
    RexPhysicsWorld* world,
    RexVec3 origin,
    RexVec3 direction,
    float maxDist,
    const RexPhysicsBody* bodies,
    std::size_t bodyCount,
    RexRaycastHit* outHit);
}

} // namespace rex::ffi
