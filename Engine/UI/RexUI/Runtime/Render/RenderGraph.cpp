#include "RenderGraph.h"

#include <algorithm>

namespace rex::ui::runtime::render {

void RenderGraph::beginFrame() {
    flattened_.clear();
}

void RenderGraph::addDrawList(const DrawList& drawList) {
    flattened_.reserve(flattened_.size() + drawList.size());
    flattened_.insert(flattened_.end(), drawList.begin(), drawList.end());
}

const DrawList& RenderGraph::flattened() const {
    return flattened_;
}

void RenderGraph::clear() {
    flattened_.clear();
}

} // namespace rex::ui::runtime::render

