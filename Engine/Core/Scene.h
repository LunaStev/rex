#pragma once
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <functional>

namespace rex {

using EntityId = uint32_t;

class ComponentPool {
public:
    virtual ~ComponentPool() = default;
};

template<typename T>
class TypedComponentPool : public ComponentPool {
public:
    std::unordered_map<EntityId, T> components;
};

class Scene {
public:
    EntityId createEntity() { return m_nextId++; }
    
    template<typename T, typename... Args>
    T& addComponent(EntityId id, Args&&... args) {
        auto type = std::type_index(typeid(T));
        if (!m_pools.count(type)) m_pools[type] = std::make_unique<TypedComponentPool<T>>();
        auto* pool = static_cast<TypedComponentPool<T>*>(m_pools[type].get());
        return pool->components[id] = T(std::forward<Args>(args)...);
    }
    
    template<typename T>
    T* getComponent(EntityId id) {
        auto type = std::type_index(typeid(T));
        if (!m_pools.count(type)) return nullptr;
        auto* pool = static_cast<TypedComponentPool<T>*>(m_pools[type].get());
        if (pool->components.count(id)) return &pool->components[id];
        return nullptr;
    }

    template<typename T, typename Func>
    void each(Func&& func) {
        auto type = std::type_index(typeid(T));
        if (!m_pools.count(type)) return;
        auto* pool = static_cast<TypedComponentPool<T>*>(m_pools[type].get());
        for (auto& [id, component] : pool->components) {
            func(id, component);
        }
    }

private:
    EntityId m_nextId = 0;
    std::unordered_map<std::type_index, std::unique_ptr<ComponentPool>> m_pools;
};

}
