#pragma once
#include <vector>
#include <memory>
#include <cstdint>
#include <type_traits>
#include <utility>

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
        if (!freeList.empty()) {
            idx = freeList.back();
            freeList.pop_back();
        } else {
            idx = static_cast<uint32_t>(slots.size());
            slots.emplace_back();
        }

        Slot& s = slots[idx];
        s.ptr = std::make_unique<T>(std::forward<Args>(args)...);
        return EntityId{ idx, s.generation };
    }

    Entity* get(EntityId id);
    const Entity* get(EntityId id) const;

    bool alive(EntityId id) const;

    void destroy(EntityId id);

    void updateAll(float dt, Input& input, World& world);
    void renderAll(Graphics& g);

    void sweepDead();

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
};
