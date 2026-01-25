#pragma once
#include <cstdint>
#include <limits>

struct EntityId {
    uint32_t index;
    uint32_t generation;
};

static constexpr EntityId NullEntity {
    std::numeric_limits<uint32_t>::max(),
    std::numeric_limits<uint32_t>::max()
};

inline bool isNull(EntityId id) {
    return id.index == NullEntity.index;
}
