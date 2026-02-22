#include "HierarchyView.h"

#include <algorithm>
#include <utility>

namespace rex::ui::widgets::editor {

void HierarchyView::setNodes(std::vector<HierarchyNode> nodes) {
    nodes_ = std::move(nodes);
    if (selectedNode_ == 0 && !nodes_.empty()) {
        selectedNode_ = nodes_.front().id;
    }
}

void HierarchyView::setFilterText(const std::string& filterText) {
    filterText_ = filterText;
}

void HierarchyView::setOnSelect(SelectHandler handler) {
    onSelect_ = std::move(handler);
}

std::uint64_t HierarchyView::selectedNode() const {
    return selectedNode_;
}

} // namespace rex::ui::widgets::editor
