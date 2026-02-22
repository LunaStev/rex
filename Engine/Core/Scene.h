#pragma once

#include <utility>

#include "ECS/World.h"

namespace rex {

using EntityId = core::ecs::EntityId;

class Scene {
public:
    EntityId createEntity() {
        return world_.createEntity();
    }

    void destroyEntity(EntityId id) {
        world_.destroyEntity(id);
    }

    void clear() {
        world_.clear();
    }

    template <typename T, typename... Args>
    T& addComponent(EntityId id, Args&&... args) {
        return world_.addComponent<T>(id, std::forward<Args>(args)...);
    }

    template <typename T>
    bool removeComponent(EntityId id) {
        return world_.removeComponent<T>(id);
    }

    template <typename T>
    T* getComponent(EntityId id) {
        return world_.getComponent<T>(id);
    }

    template <typename T>
    const T* getComponent(EntityId id) const {
        return world_.getComponent<T>(id);
    }

    template <typename T>
    bool hasComponent(EntityId id) const {
        return world_.hasComponent<T>(id);
    }

    template <typename T, typename Func>
    void each(Func&& func) {
        world_.each<T>(std::forward<Func>(func));
    }

    template <typename T, typename Func>
    void each(Func&& func) const {
        world_.each<T>(std::forward<Func>(func));
    }

    core::ecs::World& world() {
        return world_;
    }

    const core::ecs::World& world() const {
        return world_;
    }

private:
    core::ecs::World world_{};
};

// TODO [Core-Scene-001]:
// 책임: 레벨/공간 단위 오케스트레이션 API 제공
// 요구사항:
//  - ECS 저장소 세부구현을 직접 소유하지 않음
//  - 월드(ecs::World) 수명주기 제어
//  - 레거시 API(create/add/get/each) 호환 유지
// 의존성:
//  - Core/ECS/World
// 구현 단계: Phase C
// 성능 고려사항:
//  - 래퍼 호출 오버헤드 무시 수준 유지
//  - 대규모 반복 호출 시 인라이닝 유도
// 테스트 전략:
//  - 기존 호출부 회귀 테스트
//  - ecs::World 위임 정확성 테스트

} // namespace rex

