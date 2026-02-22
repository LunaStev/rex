#include "PanelWidget.h"

#include <algorithm>

namespace rex::ui::widgets::basic {

void PanelWidget::setOrientation(PanelOrientation orientation) {
    orientation_ = orientation;
}

PanelOrientation PanelWidget::orientation() const {
    return orientation_;
}

void PanelWidget::setBackgroundColor(const core::Color& color) {
    background_ = color;
}

void PanelWidget::setBorderColor(const core::Color& color) {
    border_ = color;
}

void PanelWidget::setBorderThickness(float thickness) {
    borderThickness_ = std::max(0.0f, thickness);
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

void PanelWidget::paint(core::PaintContext& ctx) const {
    if (background_.a > 0.0f) {
        ctx.drawRect(arrangedRect(), background_);
    }
    if (border_.a > 0.0f && borderThickness_ > 0.0f) {
        ctx.drawBorder(arrangedRect(), border_, borderThickness_);
    }
    core::Widget::paint(ctx);
}

} // namespace rex::ui::widgets::basic
