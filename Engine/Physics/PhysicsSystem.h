#pragma once
#include "../Core/Scene.h"
#include "../Core/Components.h"
#include "PhysicsWorld.h"
#include "RigidBody.h"

namespace rex {

class PhysicsSystem {
public:
    PhysicsSystem() {
        m_world = std::make_unique<PhysicsWorld>();
    }

    void update(Scene& scene, float dt) {
        // 1. Sync ECS -> Physics
        scene.each<RigidBodyComponent>([&](EntityId id, RigidBodyComponent& rb) {
            auto* transform = scene.getComponent<Transform>(id);
            if (!transform) return;

            if (!rb.internalBody) {
                // Lazy create internal body
                rb.internalBody = new RigidBody(rb.type);
                rb.internalBody->setMass(rb.mass);
                rb.internalBody->restitution = rb.restitution;
                rb.internalBody->position = transform->position;
                rb.internalBody->scale = transform->scale;
                m_world->addBody(rb.internalBody);
            }
        });

        // 2. Step Simulation
        m_world->step(dt);

        // 3. Sync Physics -> ECS
        scene.each<RigidBodyComponent>([&](EntityId id, RigidBodyComponent& rb) {
            if (!rb.internalBody) return;
            auto* transform = scene.getComponent<Transform>(id);
            if (transform) {
                transform->position = rb.internalBody->position;
                rb.velocity = rb.internalBody->velocity;
            }
        });
    }

    PhysicsWorld& getWorld() { return *m_world; }

private:
    std::unique_ptr<PhysicsWorld> m_world;
};

}
