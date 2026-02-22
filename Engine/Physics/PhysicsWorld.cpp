#include "PhysicsWorld.h"

#include "RigidBody.h"
#include "RustPhysicsFFI.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <unordered_map>

namespace rex {

namespace {

inline float lengthSq(const Vec3& v) {
    return dot(v, v);
}

inline ffi::RexVec3 toFfi(const Vec3& v) {
    return {v.x, v.y, v.z};
}

inline Vec3 fromFfi(const ffi::RexVec3& v) {
    return {v.x, v.y, v.z};
}

inline ffi::RexQuat toFfi(const Quat& q) {
    return {q.x, q.y, q.z, q.w};
}

inline Quat fromFfi(const ffi::RexQuat& q) {
    return {q.x, q.y, q.z, q.w};
}

inline uint64_t bodyId(const RigidBody* body) {
    return static_cast<uint64_t>(reinterpret_cast<uintptr_t>(body));
}

inline uint32_t toBodyType(BodyType type) {
    switch (type) {
        case BodyType::Static: return ffi::REX_BODY_STATIC;
        case BodyType::Dynamic: return ffi::REX_BODY_DYNAMIC;
        case BodyType::Kinematic: return ffi::REX_BODY_KINEMATIC;
    }
    return ffi::REX_BODY_DYNAMIC;
}

inline ffi::RexPhysicsBody packBody(const RigidBody& body) {
    uint32_t flags = 0;
    if (body.enableCCD) flags |= ffi::REX_BODY_FLAG_ENABLE_CCD;
    if (body.enableSleep) flags |= ffi::REX_BODY_FLAG_ENABLE_SLEEP;

    ffi::RexPhysicsBody out{};
    out.id = bodyId(&body);
    out.bodyType = toBodyType(body.type);
    out.flags = flags;
    out.mass = body.mass;
    out.invMass = body.invMass;

    out.position = toFfi(body.position);
    out.scale = toFfi(body.scale);
    out.orientation = toFfi(body.orientation);

    out.velocity = toFfi(body.velocity);
    out.angularVelocity = toFfi(body.angularVelocity);
    out.force = toFfi(body.force);
    out.torque = toFfi(body.torque);

    out.invInertia = toFfi(body.invInertia);
    out.localBoundsMin = toFfi(body.localBoundsMin);
    out.localBoundsMax = toFfi(body.localBoundsMax);

    out.restitution = body.restitution;
    out.staticFriction = body.staticFriction;
    out.dynamicFriction = body.dynamicFriction;
    out.linearDamping = body.linearDamping;
    out.angularDamping = body.angularDamping;

    out.isAwake = body.isAwake ? 1u : 0u;
    out.sleepTimer = body.sleepTimer;
    return out;
}

inline void unpackBody(const ffi::RexPhysicsBody& in, RigidBody& body) {
    body.position = fromFfi(in.position);
    body.orientation = normalize(fromFfi(in.orientation));
    body.velocity = fromFfi(in.velocity);
    body.angularVelocity = fromFfi(in.angularVelocity);
    body.force = fromFfi(in.force);
    body.torque = fromFfi(in.torque);
    body.isAwake = (in.isAwake != 0u);
    body.sleepTimer = in.sleepTimer;
}

} // namespace

PhysicsWorld::PhysicsWorld() {
    m_rustWorld = static_cast<void*>(ffi::rex_physics_world_create());
    if (m_rustWorld) {
        ffi::rex_physics_world_set_gravity(
            static_cast<ffi::RexPhysicsWorld*>(m_rustWorld), toFfi(m_gravity));
        ffi::rex_physics_world_set_solver_iterations(
            static_cast<ffi::RexPhysicsWorld*>(m_rustWorld), m_solverIterations, m_positionIterations);
        ffi::rex_physics_world_set_max_substeps(
            static_cast<ffi::RexPhysicsWorld*>(m_rustWorld), m_maxSubSteps);
    }
}

PhysicsWorld::~PhysicsWorld() {
    if (m_rustWorld) {
        ffi::rex_physics_world_destroy(static_cast<ffi::RexPhysicsWorld*>(m_rustWorld));
        m_rustWorld = nullptr;
    }
}

void PhysicsWorld::setGravity(const Vec3& g) {
    m_gravity = g;
    if (m_rustWorld) {
        ffi::rex_physics_world_set_gravity(
            static_cast<ffi::RexPhysicsWorld*>(m_rustWorld), toFfi(m_gravity));
    }
}

void PhysicsWorld::setSolverIterations(int velocityIterations, int positionIterations) {
    m_solverIterations = std::max(1, velocityIterations);
    m_positionIterations = std::max(1, positionIterations);
    if (m_rustWorld) {
        ffi::rex_physics_world_set_solver_iterations(
            static_cast<ffi::RexPhysicsWorld*>(m_rustWorld), m_solverIterations, m_positionIterations);
    }
}

void PhysicsWorld::setMaxSubSteps(int maxSubSteps) {
    m_maxSubSteps = std::clamp(maxSubSteps, 1, 16);
    if (m_rustWorld) {
        ffi::rex_physics_world_set_max_substeps(
            static_cast<ffi::RexPhysicsWorld*>(m_rustWorld), m_maxSubSteps);
    }
}

void PhysicsWorld::addBody(RigidBody* b) {
    if (!b) return;
    if (std::find(m_bodies.begin(), m_bodies.end(), b) == m_bodies.end()) {
        m_bodies.push_back(b);
    }
}

void PhysicsWorld::removeBody(RigidBody* b) {
    if (!b) return;
    m_bodies.erase(std::remove(m_bodies.begin(), m_bodies.end(), b), m_bodies.end());

    m_joints.erase(
        std::remove_if(m_joints.begin(), m_joints.end(),
            [&](const DistanceJointConstraint& j) {
                return j.a == b || j.b == b;
            }),
        m_joints.end());
}

int PhysicsWorld::addDistanceJoint(const DistanceJointDesc& desc) {
    if (!desc.bodyA || !desc.bodyB || desc.bodyA == desc.bodyB) {
        return -1;
    }

    DistanceJointConstraint joint;
    joint.id = m_nextJointId++;
    joint.a = desc.bodyA;
    joint.b = desc.bodyB;
    joint.localAnchorA = desc.localAnchorA;
    joint.localAnchorB = desc.localAnchorB;
    joint.stiffness = std::clamp(desc.stiffness, 0.0f, 1.0f);
    joint.damping = std::max(0.0f, desc.damping);

    const Vec3 worldA = desc.bodyA->position + desc.localAnchorA;
    const Vec3 worldB = desc.bodyB->position + desc.localAnchorB;
    const float currentLength = std::sqrt(lengthSq(worldB - worldA));
    joint.restLength = desc.restLength >= 0.0f ? desc.restLength : currentLength;

    m_joints.push_back(joint);
    return joint.id;
}

void PhysicsWorld::removeDistanceJoint(int jointId) {
    m_joints.erase(
        std::remove_if(m_joints.begin(), m_joints.end(),
            [&](const DistanceJointConstraint& j) { return j.id == jointId; }),
        m_joints.end());
}

void PhysicsWorld::clearDistanceJoints() {
    m_joints.clear();
}

void PhysicsWorld::step(float dt) {
    if (!m_rustWorld || dt <= 0.0f) return;

    dt = std::min(dt, m_maxFrameStep);
    m_accumulator += dt;

    int fixedSteps = 0;
    const int maxFixedStepsPerFrame = 8;
    while (m_accumulator >= FIXED_STEP && fixedSteps < maxFixedStepsPerFrame) {
        simulate(FIXED_STEP);
        m_accumulator -= FIXED_STEP;
        ++fixedSteps;
    }

    if (fixedSteps == maxFixedStepsPerFrame) {
        m_accumulator = 0.0f;
    }
}

void PhysicsWorld::simulate(float dt) {
    if (!m_rustWorld || dt <= 0.0f) return;

    std::vector<RigidBody*> activeBodies;
    activeBodies.reserve(m_bodies.size());
    for (auto* b : m_bodies) {
        if (b) activeBodies.push_back(b);
    }
    if (activeBodies.empty()) return;

    std::vector<ffi::RexPhysicsBody> ffiBodies;
    ffiBodies.reserve(activeBodies.size());
    std::unordered_map<uint64_t, std::size_t> bodyIndices;
    bodyIndices.reserve(activeBodies.size());

    for (std::size_t i = 0; i < activeBodies.size(); ++i) {
        RigidBody* body = activeBodies[i];
        ffiBodies.push_back(packBody(*body));
        bodyIndices.emplace(bodyId(body), i);
    }

    std::vector<ffi::RexDistanceJoint> ffiJoints;
    ffiJoints.reserve(m_joints.size());
    for (const auto& joint : m_joints) {
        if (!joint.a || !joint.b) continue;

        const uint64_t aId = bodyId(joint.a);
        const uint64_t bId = bodyId(joint.b);
        if (bodyIndices.find(aId) == bodyIndices.end()) continue;
        if (bodyIndices.find(bId) == bodyIndices.end()) continue;

        ffi::RexDistanceJoint j{};
        j.id = static_cast<uint64_t>(joint.id);
        j.bodyA = aId;
        j.bodyB = bId;
        j.localAnchorA = toFfi(joint.localAnchorA);
        j.localAnchorB = toFfi(joint.localAnchorB);
        j.restLength = joint.restLength;
        j.stiffness = joint.stiffness;
        j.damping = joint.damping;
        ffiJoints.push_back(j);
    }

    ffi::rex_physics_world_step(
        static_cast<ffi::RexPhysicsWorld*>(m_rustWorld),
        dt,
        ffiBodies.data(),
        ffiBodies.size(),
        ffiJoints.data(),
        ffiJoints.size());

    for (std::size_t i = 0; i < ffiBodies.size(); ++i) {
        unpackBody(ffiBodies[i], *activeBodies[i]);
    }
}

RaycastHit PhysicsWorld::raycast(const Vec3& origin, const Vec3& direction, float maxDist) {
    RaycastHit best;
    if (!m_rustWorld || maxDist <= 0.0f) return best;

    std::vector<RigidBody*> activeBodies;
    activeBodies.reserve(m_bodies.size());
    for (auto* b : m_bodies) {
        if (b) activeBodies.push_back(b);
    }
    if (activeBodies.empty()) return best;

    std::vector<ffi::RexPhysicsBody> ffiBodies;
    ffiBodies.reserve(activeBodies.size());
    std::unordered_map<uint64_t, RigidBody*> bodyLookup;
    bodyLookup.reserve(activeBodies.size());

    for (auto* body : activeBodies) {
        ffiBodies.push_back(packBody(*body));
        bodyLookup.emplace(bodyId(body), body);
    }

    ffi::RexRaycastHit hit{};
    ffi::rex_physics_world_raycast(
        static_cast<ffi::RexPhysicsWorld*>(m_rustWorld),
        toFfi(origin),
        toFfi(direction),
        maxDist,
        ffiBodies.data(),
        ffiBodies.size(),
        &hit);

    if (hit.hit == 0u) {
        return best;
    }

    auto it = bodyLookup.find(hit.body);
    if (it == bodyLookup.end()) {
        return best;
    }

    best.hit = true;
    best.body = it->second;
    best.point = fromFfi(hit.point);
    best.normal = fromFfi(hit.normal);
    best.distance = hit.distance;
    return best;
}

} // namespace rex
