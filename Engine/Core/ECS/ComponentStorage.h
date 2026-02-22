#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility>

#include "Entity.h"

namespace rex::core::ecs {

class IComponentPool {
public:
    virtual ~IComponentPool() = default;
    virtual void erase(EntityId id) = 0;
};

template <typename T>
class TypedComponentPool final : public IComponentPool {
public:
    using MapType = std::unordered_map<EntityId, T>;

    T& emplaceOrAssign(EntityId id, T value) {
        components[id] = std::move(value);
        return components[id];
    }

    T* find(EntityId id) {
        auto it = components.find(id);
        if (it == components.end()) return nullptr;
        return &it->second;
    }

    const T* find(EntityId id) const {
        auto it = components.find(id);
        if (it == components.end()) return nullptr;
        return &it->second;
    }

    void erase(EntityId id) override {
        components.erase(id);
    }

    MapType components;
};

class ComponentStorage {
public:
    void clear() {
        pools_.clear();
    }

    void eraseAllComponents(EntityId id) {
        for (auto& [type, pool] : pools_) {
            (void)type;
            pool->erase(id);
        }
    }

    template <typename T>
    TypedComponentPool<T>& pool() {
        const auto type = std::type_index(typeid(T));
        auto it = pools_.find(type);
        if (it == pools_.end()) {
            it = pools_.emplace(type, std::make_unique<TypedComponentPool<T>>()).first;
        }
        return *static_cast<TypedComponentPool<T>*>(it->second.get());
    }

    template <typename T>
    const TypedComponentPool<T>* tryPool() const {
        const auto it = pools_.find(std::type_index(typeid(T)));
        if (it == pools_.end()) return nullptr;
        return static_cast<const TypedComponentPool<T>*>(it->second.get());
    }

    template <typename T>
    TypedComponentPool<T>* tryPool() {
        const auto it = pools_.find(std::type_index(typeid(T)));
        if (it == pools_.end()) return nullptr;
        return static_cast<TypedComponentPool<T>*>(it->second.get());
    }

private:
    std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> pools_;
};

// TODO [Core-ECS-002]:
// 책임: 컴포넌트 풀 생성/조회/삭제 공통 저장소
// 요구사항:
//  - 타입 기반 풀 생성(type_index)
//  - 엔티티 삭제 시 전체 컴포넌트 정리
//  - Sparse Set/Archetype로 교체 가능한 추상 경계 유지
// 의존성:
//  - ECS/Entity
// 구현 단계: Phase C
// 성능 고려사항:
//  - 컴포넌트 조회 평균 O(1)
//  - 캐시 지역성 개선을 위한 저장 전략 확장
// 테스트 전략:
//  - add/get/remove 무결성 테스트
//  - 대량 엔티티 정리 테스트

} // namespace rex::core::ecs

