#include "WidgetTree.h"

#include <functional>

namespace rex::ui::runtime::tree {

void WidgetTree::setRoot(const WidgetPtr& root) {
    root_ = root;
    rebuildIndex();
}

WidgetTree::WidgetPtr WidgetTree::root() const {
    return root_;
}

ElementTree& WidgetTree::elements() {
    return elementTree_;
}

const ElementTree& WidgetTree::elements() const {
    return elementTree_;
}

void WidgetTree::rebuildIndex() {
    widgetIndex_.clear();
    elementTree_.clear();

    if (!root_) return;

    std::function<void(core::Widget*, core::ElementId)> visit = [&](core::Widget* widget, core::ElementId parentId) {
        if (!widget) return;

        core::ElementId id = widget->id();
        if (id == core::kInvalidElementId || widgetIndex_.find(id) != widgetIndex_.end()) {
            id = idGenerator_.next();
            widget->setId(id);
        }

        widgetIndex_[id] = widget;
        elementTree_.upsert({
            id,
            parentId,
            widget->arrangedRect(),
            true,
            1
        });

        for (const auto& child : widget->children()) {
            visit(child.get(), id);
        }
    };

    visit(root_.get(), core::kInvalidElementId);
}

core::Widget* WidgetTree::findWidget(core::ElementId id) const {
    const auto it = widgetIndex_.find(id);
    if (it == widgetIndex_.end()) {
        return nullptr;
    }
    return it->second;
}

} // namespace rex::ui::runtime::tree

