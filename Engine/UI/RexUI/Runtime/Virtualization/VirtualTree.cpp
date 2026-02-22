#include "VirtualTree.h"

#include <algorithm>

namespace rex::ui::runtime::virtualization {

void VirtualTree::setExpanded(std::uint64_t nodeId, bool expanded) {
    expanded_[nodeId] = expanded;
}

bool VirtualTree::isExpanded(std::uint64_t nodeId) const {
    const auto it = expanded_.find(nodeId);
    if (it == expanded_.end()) return true;
    return it->second;
}

void VirtualTree::setViewport(float offset, float extent) {
    viewportOffset_ = std::max(0.0f, offset);
    viewportExtent_ = std::max(0.0f, extent);
}

std::uint64_t VirtualTree::firstVisibleNode() const {
    constexpr float kNodeExtent = 20.0f;
    return static_cast<std::uint64_t>(viewportOffset_ / kNodeExtent);
}

std::uint64_t VirtualTree::visibleNodeCount() const {
    constexpr float kNodeExtent = 20.0f;
    if (viewportExtent_ <= 0.0f) return 0;
    return static_cast<std::uint64_t>(viewportExtent_ / kNodeExtent) + 2;
}

} // namespace rex::ui::runtime::virtualization

