#pragma once

#include <functional>
#include <unordered_set>
#include <utility>

#include "ComponentStorage.h"

namespace rex::core::ecs {

class World {
public:
    EntityId createEntity() {
        EntityId id = nextId_++;
        while (alive_.find(id) != alive_.end()) {
            id = nextId_++;
        }
        alive_.insert(id);
        return id;
    }

    bool isAlive(EntityId id) const {
        return alive_.find(id) != alive_.end();
    }

    void destroyEntity(EntityId id) {
        if (alive_.erase(id) == 0) return;
        storage_.eraseAllComponents(id);
    }

    void clear() {
        alive_.clear();
        storage_.clear();
        nextId_ = 0;
    }

    template <typename T, typename... Args>
    T& addComponent(EntityId id, Args&&... args) {
        if (!isAlive(id)) {
            alive_.insert(id);
            if (id >= nextId_) nextId_ = id + 1;
        }
        auto& pool = storage_.pool<T>();
        return pool.emplaceOrAssign(id, T(std::forward<Args>(args)...));
    }

    template <typename T>
    bool removeComponent(EntityId id) {
        auto* pool = storage_.template tryPool<T>();
        if (!pool) return false;
        const bool existed = pool->find(id) != nullptr;
        pool->erase(id);
        return existed;
    }

    template <typename T>
    T* getComponent(EntityId id) {
        auto* pool = storage_.template tryPool<T>();
        if (!pool) return nullptr;
        return pool->find(id);
    }

    template <typename T>
    const T* getComponent(EntityId id) const {
        auto* pool = storage_.template tryPool<T>();
        if (!pool) return nullptr;
        return pool->find(id);
    }

    template <typename T>
    bool hasComponent(EntityId id) const {
        return getComponent<T>(id) != nullptr;
    }

    template <typename T, typename Func>
    void each(Func&& func) {
        auto* pool = storage_.template tryPool<T>();
        if (!pool) return;
        for (auto& [id, component] : pool->components) {
            func(id, component);
        }
    }

    template <typename T, typename Func>
    void each(Func&& func) const {
        auto* pool = storage_.template tryPool<T>();
        if (!pool) return;
        for (const auto& [id, component] : pool->components) {
            func(id, component);
        }
    }

    template <typename T1, typename T2, typename Func>
    void each(Func&& func) {
        auto* base = storage_.template tryPool<T1>();
        if (!base) return;
        for (auto& [id, c1] : base->components) {
            auto* c2 = getComponent<T2>(id);
            if (!c2) continue;
            func(id, c1, *c2);
        }
    }

    template <typename T1, typename T2, typename... TRest, typename Func>
    void each(Func&& func) {
        auto* base = storage_.template tryPool<T1>();
        if (!base) return;
        for (auto& [id, c1] : base->components) {
            auto* c2 = getComponent<T2>(id);
            if (!c2) continue;
            if (((getComponent<TRest>(id) != nullptr) && ...)) {
                func(id, c1, *c2, *getComponent<TRest>(id)...);
            }
        }
    }

    const ComponentStorage& storage() const {
        return storage_;
    }

    ComponentStorage& storage() {
        return storage_;
    }

private:
    EntityId nextId_ = 0;
    std::unordered_set<EntityId> alive_;
    ComponentStorage storage_;
};

// TODO [Core-ECS-003]:
// 책임: 엔티티/컴포넌트 수명주기를 소유하는 월드 추상화
// 요구사항:
//  - create/destroy entity API
//  - add/get/remove/has component API
//  - query(each) 진입점 제공
// 의존성:
//  - ECS/ComponentStorage
// 구현 단계: Phase C
// 성능 고려사항:
//  - 핫패스 조회 O(1)
//  - 대량 each 순회 시 분기 최소화
// 테스트 전략:
//  - 월드 수명주기 테스트
//  - 다중 컴포넌트 query 테스트

} // namespace rex::core::ecs
