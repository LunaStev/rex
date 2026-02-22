#pragma once
#include "../Core/Scene.h"
#include "../Core/Components.h"
#include "PhysicsWorld.h"
#include "RigidBody.h"
#include <cmath>
#include <unordered_map>
#include <unordered_set>

namespace rex {

class PhysicsSystem {
public:
    PhysicsSystem() {
        m_world = std::make_unique<PhysicsWorld>();
    }

    void update(Scene& scene, float dt) {
        constexpr float DEG2RAD = 0.01745329251994329577f;
        constexpr float RAD2DEG = 57.295779513082320876f;

        std::unordered_set<EntityId> activeEntities;

        // 1. Sync ECS -> Physics
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
                m_world->addBody(rb.internalBody);
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

            // Static/Kinematic transforms are controlled by scene-side transforms.
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

        // Remove orphaned physics bodies.
        for (auto it = m_bodyPool.begin(); it != m_bodyPool.end(); ) {
            if (activeEntities.find(it->first) == activeEntities.end()) {
                m_world->removeBody(it->second.get());
                it = m_bodyPool.erase(it);
            } else {
                ++it;
            }
        }

        // 2. Step Simulation
        m_world->step(dt);

        // 3. Sync Physics -> ECS
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
                    euler.z * RAD2DEG
                };
            }
            rb.velocity = rb.internalBody->velocity;
            rb.angularVelocity = rb.internalBody->angularVelocity;
        });
    }

    PhysicsWorld& getWorld() { return *m_world; }

private:
    std::unique_ptr<PhysicsWorld> m_world;
    std::unordered_map<EntityId, std::unique_ptr<RigidBody>> m_bodyPool;
};

}
