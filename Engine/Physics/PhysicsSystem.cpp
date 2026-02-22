#include "PhysicsSystem.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <unordered_set>

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

PhysicsSystem::PhysicsSystem() {
    m_rustWorld = ffi::rex_physics_world_create();
    if (m_rustWorld) {
        ffi::rex_physics_world_set_gravity(m_rustWorld, toFfi(m_gravity));
        ffi::rex_physics_world_set_solver_iterations(m_rustWorld, m_solverIterations, m_positionIterations);
        ffi::rex_physics_world_set_max_substeps(m_rustWorld, m_maxSubSteps);
    }
}

PhysicsSystem::~PhysicsSystem() {
    if (m_rustWorld) {
        ffi::rex_physics_world_destroy(m_rustWorld);
        m_rustWorld = nullptr;
    }
}

void PhysicsSystem::setGravity(const Vec3& g) {
    m_gravity = g;
    if (m_rustWorld) {
        ffi::rex_physics_world_set_gravity(m_rustWorld, toFfi(m_gravity));
    }
}

void PhysicsSystem::setSolverIterations(int velocityIterations, int positionIterations) {
    m_solverIterations = std::max(1, velocityIterations);
    m_positionIterations = std::max(1, positionIterations);
    if (m_rustWorld) {
        ffi::rex_physics_world_set_solver_iterations(m_rustWorld, m_solverIterations, m_positionIterations);
    }
}

void PhysicsSystem::setMaxSubSteps(int maxSubSteps) {
    m_maxSubSteps = std::clamp(maxSubSteps, 1, 16);
    if (m_rustWorld) {
        ffi::rex_physics_world_set_max_substeps(m_rustWorld, m_maxSubSteps);
    }
}

int PhysicsSystem::addDistanceJoint(const DistanceJointDesc& desc) {
    if (!desc.bodyA || !desc.bodyB || desc.bodyA == desc.bodyB) {
        return -1;
    }

    DistanceJointState joint;
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

void PhysicsSystem::removeDistanceJoint(int jointId) {
    m_joints.erase(
        std::remove_if(m_joints.begin(), m_joints.end(),
            [&](const DistanceJointState& j) { return j.id == jointId; }),
        m_joints.end());
}

void PhysicsSystem::clearDistanceJoints() {
    m_joints.clear();
}

void PhysicsSystem::step(float dt) {
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

void PhysicsSystem::simulate(float dt) {
    if (!m_rustWorld || dt <= 0.0f) return;

    std::vector<RigidBody*> activeBodies;
    activeBodies.reserve(m_bodyPool.size());
    for (auto& [_, body] : m_bodyPool) {
        if (body) activeBodies.push_back(body.get());
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
        m_rustWorld,
        dt,
        ffiBodies.data(),
        ffiBodies.size(),
        ffiJoints.data(),
        ffiJoints.size());

    for (std::size_t i = 0; i < ffiBodies.size(); ++i) {
        unpackBody(ffiBodies[i], *activeBodies[i]);
    }
}

RaycastHit PhysicsSystem::raycast(const Vec3& origin, const Vec3& direction, float maxDist) {
    RaycastHit best;
    if (!m_rustWorld || maxDist <= 0.0f) return best;

    std::vector<RigidBody*> activeBodies;
    activeBodies.reserve(m_bodyPool.size());
    for (auto& [_, body] : m_bodyPool) {
        if (body) activeBodies.push_back(body.get());
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
        m_rustWorld,
        toFfi(origin),
        toFfi(direction),
        maxDist,
        ffiBodies.data(),
        ffiBodies.size(),
        &hit);

    if (hit.hit == 0u) return best;

    auto it = bodyLookup.find(hit.body);
    if (it == bodyLookup.end()) return best;

    best.hit = true;
    best.body = it->second;
    best.point = fromFfi(hit.point);
    best.normal = fromFfi(hit.normal);
    best.distance = hit.distance;
    return best;
}

void PhysicsSystem::update(Scene& scene, float dt) {
    constexpr float DEG2RAD = 0.01745329251994329577f;
    constexpr float RAD2DEG = 57.295779513082320876f;

    std::unordered_set<EntityId> activeEntities;

    scene.each<RigidBodyComponent>([&](EntityId id, RigidBodyComponent& rb) {
        activeEntities.insert(id);
        auto* transform = scene.getComponent<Transform>(id);
        if (!transform) return;

        auto it = m_bodyPool.find(id);
        if (it == m_bodyPool.end()) {
            auto body = std::make_unique<RigidBody>(rb.type);
            body->position = transform->position;
            body->scale = transform->scale;
            body->orientation = Quat::fromEulerXYZ({
                transform->rotation.x * DEG2RAD,
                transform->rotation.y * DEG2RAD,
                transform->rotation.z * DEG2RAD
            });
            body->velocity = rb.velocity;
            body->angularVelocity = rb.angularVelocity;
            body->setMass(rb.mass);
            body->restitution = rb.restitution;
            body->staticFriction = rb.staticFriction;
            body->dynamicFriction = rb.dynamicFriction;
            body->linearDamping = rb.linearDamping;
            body->angularDamping = rb.angularDamping;
            body->enableCCD = rb.enableCCD;
            body->updateInertiaTensor();

            rb.internalBody = body.get();
            it = m_bodyPool.emplace(id, std::move(body)).first;
        }

        RigidBody* body = it->second.get();
        if (!body) return;

        body->type = rb.type;
        body->setMass(rb.mass);
        body->restitution = rb.restitution;
        body->staticFriction = rb.staticFriction;
        body->dynamicFriction = rb.dynamicFriction;
        body->linearDamping = rb.linearDamping;
        body->angularDamping = rb.angularDamping;
        body->enableCCD = rb.enableCCD;
        body->scale = transform->scale;
        body->updateInertiaTensor();

        if (rb.type != BodyType::Dynamic) {
            body->position = transform->position;
            body->orientation = Quat::fromEulerXYZ({
                transform->rotation.x * DEG2RAD,
                transform->rotation.y * DEG2RAD,
                transform->rotation.z * DEG2RAD
            });
            body->velocity = rb.velocity;
            body->angularVelocity = rb.angularVelocity;
            body->wakeUp();
        }
        rb.internalBody = body;
    });

    for (auto it = m_bodyPool.begin(); it != m_bodyPool.end();) {
        if (activeEntities.find(it->first) == activeEntities.end()) {
            RigidBody* removed = it->second.get();
            m_joints.erase(
                std::remove_if(m_joints.begin(), m_joints.end(),
                    [&](const DistanceJointState& j) {
                        return j.a == removed || j.b == removed;
                    }),
                m_joints.end());
            it = m_bodyPool.erase(it);
        } else {
            ++it;
        }
    }

    step(dt);

    scene.each<RigidBodyComponent>([&](EntityId id, RigidBodyComponent& rb) {
        auto it = m_bodyPool.find(id);
        if (it == m_bodyPool.end() || !it->second) {
            rb.internalBody = nullptr;
            return;
        }

        rb.internalBody = it->second.get();

        auto* transform = scene.getComponent<Transform>(id);
        if (transform && rb.type == BodyType::Dynamic) {
            transform->position = rb.internalBody->position;
            const Vec3 euler = rb.internalBody->orientation.toEulerXYZ();
            transform->rotation = {
                euler.x * RAD2DEG,
                euler.y * RAD2DEG,
                euler.z * RAD2DEG,
            };
        }
        rb.velocity = rb.internalBody->velocity;
        rb.angularVelocity = rb.internalBody->angularVelocity;
    });
}

} // namespace rex
