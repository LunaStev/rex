#include "PanelWidget.h"

#include <algorithm>

namespace rex::ui::widgets::basic {

void PanelWidget::setOrientation(PanelOrientation orientation) {
    orientation_ = orientation;
}

PanelOrientation PanelWidget::orientation() const {
    return orientation_;
}

void PanelWidget::arrange(const core::Rect& finalRect) {
    core::Widget::arrange(finalRect);

    const auto& items = children();
    if (items.empty()) return;
    if (orientation_ == PanelOrientation::Overlay) return;

    if (orientation_ == PanelOrientation::Horizontal) {
        const float cellWidth = finalRect.w / static_cast<float>(items.size());
        float x = finalRect.x;
        for (const auto& child : items) {
            if (!child) continue;
            child->arrange({x, finalRect.y, cellWidth, finalRect.h});
            x += cellWidth;
        }
        return;
    }

    const float cellHeight = finalRect.h / static_cast<float>(items.size());
    float y = finalRect.y;
    for (const auto& child : items) {
        if (!child) continue;
        child->arrange({finalRect.x, y, finalRect.w, cellHeight});
        y += cellHeight;
    }
}

} // namespace rex::ui::widgets::basic

