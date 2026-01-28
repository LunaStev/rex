#pragma once
#include <cstdint>
#include <limits>

struct EntityId {
    uint32_t index = std::numeric_limits<uint32_t>::max();
    uint32_t generation = std::numeric_limits<uint32_t>::max();

    bool operator==(const EntityId& o) const { return index == o.index && generation == o.generation; }
    bool operator!=(const EntityId& o) const { return !(*this == o); }
};

static constexpr EntityId NullEntity{};

inline bool isNull(EntityId id) {
    return id.index == NullEntity.index;
}

struct EntityIdHash {
    std::size_t operator()(const EntityId& id) const noexcept {
        return (std::size_t)id.index ^ ((std::size_t)id.generation << 1);
    }
};
