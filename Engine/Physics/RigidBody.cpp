#include "RigidBody.h"

namespace rex {

RigidBody::RigidBody(BodyType t) : type(t) {
    if (type == BodyType::Static) {
        setMass(0.0f);
    } else {
        setMass(1.0f);
    }
}

void RigidBody::applyForce(const Vec3& f) {
    if (type != BodyType::Dynamic) return;
    force.x += f.x;
    force.y += f.y;
    force.z += f.z;
}

void RigidBody::applyImpulse(const Vec3& impulse) {
    if (type != BodyType::Dynamic) return;
    velocity.x += impulse.x * invMass;
    velocity.y += impulse.y * invMass;
    velocity.z += impulse.z * invMass;
}

}
