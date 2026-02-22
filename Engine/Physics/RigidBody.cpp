#include "RigidBody.h"

#include "RustPhysicsFFI.h"

#include <cstdint>

namespace rex {

namespace {

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

    ffi::RexPhysicsBody raw{};
    raw.id = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(&body));
    raw.bodyType = toBodyType(body.type);
    raw.flags = flags;
    raw.mass = body.mass;
    raw.invMass = body.invMass;

    raw.position = toFfi(body.position);
    raw.scale = toFfi(body.scale);
    raw.orientation = toFfi(body.orientation);

    raw.velocity = toFfi(body.velocity);
    raw.angularVelocity = toFfi(body.angularVelocity);
    raw.force = toFfi(body.force);
    raw.torque = toFfi(body.torque);

    raw.invInertia = toFfi(body.invInertia);
    raw.localBoundsMin = toFfi(body.localBoundsMin);
    raw.localBoundsMax = toFfi(body.localBoundsMax);

    raw.restitution = body.restitution;
    raw.staticFriction = body.staticFriction;
    raw.dynamicFriction = body.dynamicFriction;
    raw.linearDamping = body.linearDamping;
    raw.angularDamping = body.angularDamping;

    raw.isAwake = body.isAwake ? 1u : 0u;
    raw.sleepTimer = body.sleepTimer;
    return raw;
}

inline void unpackBody(const ffi::RexPhysicsBody& raw, RigidBody& body) {
    body.mass = raw.mass;
    body.invMass = raw.invMass;

    body.position = fromFfi(raw.position);
    body.scale = fromFfi(raw.scale);
    body.orientation = normalize(fromFfi(raw.orientation));

    body.velocity = fromFfi(raw.velocity);
    body.angularVelocity = fromFfi(raw.angularVelocity);
    body.force = fromFfi(raw.force);
    body.torque = fromFfi(raw.torque);

    body.invInertia = fromFfi(raw.invInertia);
    body.localBoundsMin = fromFfi(raw.localBoundsMin);
    body.localBoundsMax = fromFfi(raw.localBoundsMax);

    body.restitution = raw.restitution;
    body.staticFriction = raw.staticFriction;
    body.dynamicFriction = raw.dynamicFriction;
    body.linearDamping = raw.linearDamping;
    body.angularDamping = raw.angularDamping;

    body.isAwake = (raw.isAwake != 0u);
    body.sleepTimer = raw.sleepTimer;
}

} // namespace

RigidBody::RigidBody(BodyType t) : type(t) {
    if (type == BodyType::Dynamic) {
        setMass(1.0f);
    } else {
        setMass(0.0f);
    }
}

void RigidBody::setMass(float m) {
    ffi::RexPhysicsBody raw = packBody(*this);
    ffi::rex_rigidbody_set_mass(&raw, m);
    unpackBody(raw, *this);
}

void RigidBody::applyForce(const Vec3& f) {
    ffi::RexPhysicsBody raw = packBody(*this);
    ffi::rex_rigidbody_apply_force(&raw, toFfi(f));
    unpackBody(raw, *this);
}

void RigidBody::applyImpulse(const Vec3& impulse) {
    ffi::RexPhysicsBody raw = packBody(*this);
    ffi::rex_rigidbody_apply_impulse(&raw, toFfi(impulse));
    unpackBody(raw, *this);
}

void RigidBody::applyTorque(const Vec3& t) {
    ffi::RexPhysicsBody raw = packBody(*this);
    ffi::rex_rigidbody_apply_torque(&raw, toFfi(t));
    unpackBody(raw, *this);
}

void RigidBody::applyAngularImpulse(const Vec3& impulse) {
    ffi::RexPhysicsBody raw = packBody(*this);
    ffi::rex_rigidbody_apply_angular_impulse(&raw, toFfi(impulse));
    unpackBody(raw, *this);
}

void RigidBody::applyImpulseAtPoint(const Vec3& impulse, const Vec3& worldPoint) {
    ffi::RexPhysicsBody raw = packBody(*this);
    ffi::rex_rigidbody_apply_impulse_at_point(&raw, toFfi(impulse), toFfi(worldPoint));
    unpackBody(raw, *this);
}

void RigidBody::updateInertiaTensor() {
    ffi::RexPhysicsBody raw = packBody(*this);
    ffi::rex_rigidbody_update_inertia_tensor(&raw);
    unpackBody(raw, *this);
}

void RigidBody::wakeUp() {
    ffi::RexPhysicsBody raw = packBody(*this);
    ffi::rex_rigidbody_wake_up(&raw);
    unpackBody(raw, *this);
}

} // namespace rex
