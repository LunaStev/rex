#include "EntityManager.h"
#include "../Input/Input.h"
#include "../World/World.h"
#include "../Graphics/Graphics.h"

Entity* EntityManager::get(EntityId id) {
    if (isNull(id)) return nullptr;
    if (id.index >= slots.size()) return nullptr;

    Slot& s = slots[id.index];
    if (!s.ptr) return nullptr;
    if (s.generation != id.generation) return nullptr;
    return s.ptr.get();
}

const Entity* EntityManager::get(EntityId id) const {
    if (isNull(id)) return nullptr;
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
    if (isNull(id)) return;
    if (id.index >= slots.size()) return;

    Slot& s = slots[id.index];
    if (!s.ptr) return;
    if (s.generation != id.generation) return;

    // 훅 먼저
    s.ptr->onDestroy();

    s.ptr.reset();
    s.generation++;
    freeList.push_back(id.index);
}

void EntityManager::destroyLater(EntityId id) {
    if (isNull(id)) return;
    destroyQueue.push_back(id);
}

void EntityManager::flushDestroyQueue() {
    if (destroyQueue.empty()) return;
    for (auto id : destroyQueue) destroy(id);
    destroyQueue.clear();
}

void EntityManager::updateAll(float dt, Input& input, World& world) {
    updating = true;

    // slots.size()는 update 중에 늘어날 수 있으니, "기존 엔티티만" 업데이트하고 싶으면 snapshot을 떠도 됨
    // 여기선 “생성은 되지만 같은 프레임에 재업데이트는 안 되게” create에서 슬롯 재사용을 막았으니까 그대로 감
    for (auto& s : slots) {
        if (s.ptr && s.ptr->isAlive()) {
            s.ptr->update(dt, input, world);
        }
    }

    updating = false;

    // 죽은 애 정리 (kill 기반)
    sweepDead();

    // destroyLater 처리
    flushDestroyQueue();
}

void EntityManager::renderAll(Graphics& g) {
    for (auto& s : slots) {
        if (s.ptr && s.ptr->isAlive() && s.ptr->isVisible()) {
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

void EntityManager::clear() {
    // destroy 훅 보장
    for (uint32_t i = 0; i < slots.size(); ++i) {
        auto& s = slots[i];
        if (s.ptr) {
            EntityId id{ i, s.generation };
            destroy(id);
        }
    }
    slots.clear();
    freeList.clear();
    destroyQueue.clear();
    updating = false;
}
