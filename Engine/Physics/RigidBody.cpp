#include "RigidBody.h"
#include <cmath>

namespace rex {

namespace {
inline Vec3 mulWorldInvInertia(const RigidBody& body, const Vec3& v) {
    const Quat q = normalize(body.orientation);
    const Vec3 local = q.conjugate().rotate(v);
    const Vec3 localApplied{
        body.invInertia.x * local.x,
        body.invInertia.y * local.y,
        body.invInertia.z * local.z
    };
    return q.rotate(localApplied);
}
}

RigidBody::RigidBody(BodyType t) : type(t) {
    if (type == BodyType::Dynamic) {
        setMass(1.0f);
    } else {
        setMass(0.0f);
    }
}

void RigidBody::applyForce(const Vec3& f) {
    if (type != BodyType::Dynamic) return;
    wakeUp();
    force.x += f.x;
    force.y += f.y;
    force.z += f.z;
}

void RigidBody::applyImpulse(const Vec3& impulse) {
    if (type != BodyType::Dynamic) return;
    wakeUp();
    velocity.x += impulse.x * invMass;
    velocity.y += impulse.y * invMass;
    velocity.z += impulse.z * invMass;
}

void RigidBody::applyTorque(const Vec3& t) {
    if (type != BodyType::Dynamic) return;
    wakeUp();
    torque.x += t.x;
    torque.y += t.y;
    torque.z += t.z;
}

void RigidBody::applyAngularImpulse(const Vec3& impulse) {
    if (type != BodyType::Dynamic) return;
    wakeUp();
    angularVelocity += mulWorldInvInertia(*this, impulse);
}

void RigidBody::applyImpulseAtPoint(const Vec3& impulse, const Vec3& worldPoint) {
    if (type != BodyType::Dynamic) return;
    wakeUp();

    velocity.x += impulse.x * invMass;
    velocity.y += impulse.y * invMass;
    velocity.z += impulse.z * invMass;

    const Vec3 r{
        worldPoint.x - position.x,
        worldPoint.y - position.y,
        worldPoint.z - position.z
    };
    const Vec3 angularImpulse = cross(r, impulse);
    angularVelocity += mulWorldInvInertia(*this, angularImpulse);
}

void RigidBody::updateInertiaTensor() {
    if (type != BodyType::Dynamic || mass <= 0.0f) {
        invInertia = {0,0,0};
        return;
    }

    // Box inertia approximation from local OBB extents in world scale.
    const float width  = std::max(0.001f, std::fabs((localBoundsMax.x - localBoundsMin.x) * scale.x));
    const float height = std::max(0.001f, std::fabs((localBoundsMax.y - localBoundsMin.y) * scale.y));
    const float depth  = std::max(0.001f, std::fabs((localBoundsMax.z - localBoundsMin.z) * scale.z));

    const float ixx = (mass / 12.0f) * (height * height + depth * depth);
    const float iyy = (mass / 12.0f) * (width * width + depth * depth);
    const float izz = (mass / 12.0f) * (width * width + height * height);

    invInertia.x = ixx > 1e-6f ? 1.0f / ixx : 0.0f;
    invInertia.y = iyy > 1e-6f ? 1.0f / iyy : 0.0f;
    invInertia.z = izz > 1e-6f ? 1.0f / izz : 0.0f;
}

void RigidBody::wakeUp() {
    isAwake = true;
    sleepTimer = 0.0f;
}

}
