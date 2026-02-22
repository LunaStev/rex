#include "LayoutCache.h"

#include <bit>
#include <cstddef>

namespace rex::ui::runtime::layout {

namespace {
inline std::uint64_t hashCombine(std::uint64_t seed, std::uint64_t value) {
    seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
    return seed;
}

inline std::uint64_t hashFloat(float v) {
    return static_cast<std::uint64_t>(std::bit_cast<std::uint32_t>(v));
}
} // namespace

std::uint64_t LayoutCache::makeKeyHash(const LayoutCacheKey& key) {
    std::uint64_t h = key.elementId;
    h = hashCombine(h, key.styleHash);
    h = hashCombine(h, key.contentHash);
    h = hashCombine(h, hashFloat(key.constraints.min.w));
    h = hashCombine(h, hashFloat(key.constraints.min.h));
    h = hashCombine(h, hashFloat(key.constraints.max.w));
    h = hashCombine(h, hashFloat(key.constraints.max.h));
    return h;
}

void LayoutCache::beginFrame(std::uint64_t frameStamp) {
    frameStamp_ = frameStamp;
}

std::optional<core::LayoutResult> LayoutCache::find(const LayoutCacheKey& key) const {
    const std::uint64_t h = makeKeyHash(key);
    const auto it = entries_.find(h);
    if (it == entries_.end()) {
        return std::nullopt;
    }
    core::LayoutResult result = it->second.result;
    result.cacheHit = true;
    return result;
}

void LayoutCache::store(const LayoutCacheEntry& entry) {
    const std::uint64_t h = makeKeyHash(entry.key);
    LayoutCacheEntry stored = entry;
    stored.frameStamp = frameStamp_;
    stored.result.cacheHit = false;
    entries_[h] = stored;
}

void LayoutCache::clear() {
    entries_.clear();
}

} // namespace rex::ui::runtime::layout

