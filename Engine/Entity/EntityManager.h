#pragma once
#include <vector>
#include <memory>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <unordered_set>

#include "Entity.h"
#include "EntityId.h"

class Input;
class World;
class Graphics;

class EntityManager {
public:
    EntityManager() = default;

    template<typename T, typename... Args>
    EntityId create(Args&&... args) {
        static_assert(std::is_base_of_v<Entity, T>, "EntityManager::create<T>: T must derive from Entity");

        uint32_t idx;

        // update 중에 슬롯 재사용하면 “같은 프레임 2회 업데이트” 같은 버그가 터질 수 있음
        // 그래서 update 중엔 freeList 재사용 금지하고 항상 뒤에 붙임
        if (!updating && !freeList.empty()) {
            idx = freeList.back();
            freeList.pop_back();
        } else {
            idx = static_cast<uint32_t>(slots.size());
            slots.emplace_back();
        }

        Slot& s = slots[idx];
        s.ptr = std::make_unique<T>(std::forward<Args>(args)...);

        EntityId id{ idx, s.generation };
        s.ptr->_setId(id);
        s.ptr->onSpawn();
        return id;
    }

    Entity* get(EntityId id);
    const Entity* get(EntityId id) const;

    template<typename T>
    T* getAs(EntityId id) {
        static_assert(std::is_base_of_v<Entity, T>, "getAs<T>: T must derive from Entity");
        return dynamic_cast<T*>(get(id));
    }

    bool alive(EntityId id) const;

    // 즉시 파괴
    void destroy(EntityId id);

    // 안전한 지연 파괴 (update 중에도 OK)
    void destroyLater(EntityId id);
    void flushDestroyQueue();

    void updateAll(float dt, Input& input, World& world);
    void renderAll(Graphics& g);

    void sweepDead();

    void clear();

    template<typename Fn>
    void forEachAlive(Fn&& fn) {
        for (auto& s : slots) {
            if (s.ptr && s.ptr->isAlive()) fn(*s.ptr);
        }
    }

private:
    struct Slot {
        uint32_t generation = 1;
        std::unique_ptr<Entity> ptr;
    };

    std::vector<Slot> slots;
    std::vector<uint32_t> freeList;

    bool updating = false;

    std::vector<EntityId> destroyQueue;
};
