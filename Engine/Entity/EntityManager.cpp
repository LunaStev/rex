#include "EntityManager.h"
#include "../Input/Input.h"
#include "../World/World.h"
#include "../Graphics/Graphics.h"

Entity* EntityManager::get(EntityId id) {
    if (id.index >= slots.size()) return nullptr;
    Slot& s = slots[id.index];
    if (!s.ptr) return nullptr;
    if (s.generation != id.generation) return nullptr;
    return s.ptr.get();
}

const Entity* EntityManager::get(EntityId id) const {
    if (id.index >= slots.size()) return nullptr;
    const Slot& s = slots[id.index];
    if (!s.ptr) return nullptr;
    if (s.generation != id.generation) return nullptr;
    return s.ptr.get();
}

bool EntityManager::alive(EntityId id) const {
    const Entity* e = get(id);
    return e && e->isAlive();
}

void EntityManager::destroy(EntityId id) {
    if (id.index >= slots.size()) return;
    Slot& s = slots[id.index];
    if (!s.ptr) return;
    if (s.generation != id.generation) return;

    s.ptr.reset();
    s.generation++;
    freeList.push_back(id.index);
}

void EntityManager::updateAll(float dt, Input& input, World& world) {
    for (auto& s : slots) {
        if (s.ptr && s.ptr->isAlive()) {
            s.ptr->update(dt, input, world);
        }
    }
}

void EntityManager::renderAll(Graphics& g) {
    for (auto& s : slots) {
        if (s.ptr && s.ptr->isAlive()) {
            s.ptr->render(g);
        }
    }
}

void EntityManager::sweepDead() {
    for (uint32_t i = 0; i < slots.size(); ++i) {
        auto& s = slots[i];
        if (s.ptr && !s.ptr->isAlive()) {
            EntityId id{ i, s.generation };
            destroy(id);
        }
    }
}
