#include "VirtualList.h"

#include <algorithm>

namespace rex::ui::runtime::virtualization {

void VirtualList::setItemCount(std::uint64_t itemCount) {
    itemCount_ = itemCount;
}

void VirtualList::setItemExtent(float itemExtent) {
    itemExtent_ = std::max(0.001f, itemExtent);
}

void VirtualList::setViewport(float offset, float extent) {
    viewportOffset_ = std::max(0.0f, offset);
    viewportExtent_ = std::max(0.0f, extent);
}

std::uint64_t VirtualList::firstVisibleIndex() const {
    if (itemCount_ == 0) return 0;
    const auto idx = static_cast<std::uint64_t>(viewportOffset_ / itemExtent_);
    return std::min(idx, itemCount_ - 1);
}

std::uint64_t VirtualList::visibleCount() const {
    if (itemCount_ == 0 || viewportExtent_ <= 0.0f) return 0;
    const std::uint64_t count = static_cast<std::uint64_t>(viewportExtent_ / itemExtent_) + 2;
    const std::uint64_t first = firstVisibleIndex();
    if (first >= itemCount_) return 0;
    return std::min(count, itemCount_ - first);
}

} // namespace rex::ui::runtime::virtualization

