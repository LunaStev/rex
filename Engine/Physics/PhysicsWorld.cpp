#include "PhysicsWorld.h"
#include "RigidBody.h"
#include <algorithm>

namespace rex {

PhysicsWorld::PhysicsWorld() {}
PhysicsWorld::~PhysicsWorld() {}

void PhysicsWorld::addBody(RigidBody* b) { m_bodies.push_back(b); }
void PhysicsWorld::removeBody(RigidBody* b) {
    m_bodies.erase(std::remove(m_bodies.begin(), m_bodies.end(), b), m_bodies.end());
}

void PhysicsWorld::step(float dt) {
    m_accumulator += dt;
    while (m_accumulator >= FIXED_STEP) {
        integrate(FIXED_STEP);
        resolveCollisions();
        m_accumulator -= FIXED_STEP;
    }
}

void PhysicsWorld::integrate(float dt) {
    for (auto* b : m_bodies) {
        if (b->type != BodyType::Dynamic) continue;

        // Apply Gravity
        b->velocity.x += m_gravity.x * dt;
        b->velocity.y += m_gravity.y * dt;
        b->velocity.z += m_gravity.z * dt;

        // Update Position
        b->position.x += b->velocity.x * dt;
        b->position.y += b->velocity.y * dt;
        b->position.z += b->velocity.z * dt;
        
        // Reset forces
        b->force = {0,0,0};
    }
}

void PhysicsWorld::resolveCollisions() {
    for (size_t i = 0; i < m_bodies.size(); ++i) {
        for (size_t j = i + 1; j < m_bodies.size(); ++j) {
            RigidBody* a = m_bodies[i];
            RigidBody* b = m_bodies[j];

            if (a->invMass == 0 && b->invMass == 0) continue;

            if (a->isColliding(*b)) {
                // Simplified Collision Response (Elastic)
                Vec3 normal = {0, 1, 0}; // Placeholder normal (needs proper SAT or GJK)
                
                Vec3 relativeVel = { b->velocity.x - a->velocity.x, b->velocity.y - a->velocity.y, b->velocity.z - a->velocity.z };
                float velAlongNormal = relativeVel.x * normal.x + relativeVel.y * normal.y + relativeVel.z * normal.z;

                if (velAlongNormal > 0) continue;

                float e = std::min(a->restitution, b->restitution);
                float j_impulse = -(1 + e) * velAlongNormal;
                j_impulse /= (a->invMass + b->invMass);

                Vec3 impulse = { normal.x * j_impulse, normal.y * j_impulse, normal.z * j_impulse };
                
                if (a->type == BodyType::Dynamic) {
                    a->velocity.x -= a->invMass * impulse.x;
                    a->velocity.y -= a->invMass * impulse.y;
                    a->velocity.z -= a->invMass * impulse.z;
                }
                if (b->type == BodyType::Dynamic) {
                    b->velocity.x += b->invMass * impulse.x;
                    b->velocity.y += b->invMass * impulse.y;
                    b->velocity.z += b->invMass * impulse.z;
                }
            }
        }
    }
}

RaycastHit PhysicsWorld::raycast(const Vec3& origin, const Vec3& direction, float maxDist) {
    // Basic Raycast Placeholder
    return RaycastHit{};
}

}
